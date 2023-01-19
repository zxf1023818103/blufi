#include "blufiframecoder.h"
#include "bluficlient.h"
#include <QByteArray>
#include <QDebug>
#include <QMetaEnum>

extern "C"
{

#pragma pack(1)

    struct BlufiFrameHeader
    {
        union
        {
            struct
            {
                quint8 frameType : 2;
                quint8 frameSubType : 6;
            };
            quint8 type;
        };
        quint8 encrypted : 1;
        quint8 checksumEnabled : 1;
        quint8 toPhone : 1;
        quint8 ackRequired : 1;
        quint8 fragmented : 1;
        quint8 : 3;
        quint8 sequenceNo;
        quint8 dataSize;
        union
        {
            struct
            {
                quint8 totalSizeLow;
                quint8 totalSizeHigh;
                quint8 content[1];
            };
            quint8 data[1];
        };
    };

#pragma pack()
}

BlufiFrameCoder::BlufiFrameCoder(QObject *parent)
    : QObject{parent}
{
    connect(this, &BlufiFrameCoder::dataFrameReceived, this, &BlufiFrameCoder::parseDataFrame);
    connect(this, &BlufiFrameCoder::controlFrameReceived, this, &BlufiFrameCoder::parseControlFrame);
    connect(this, &BlufiFrameCoder::negotiationDataReceived, this, &BlufiFrameCoder::onNegotiationDataReceived);
}

void BlufiFrameCoder::sendFrame(quint8 type, const QByteArray &data, bool toPhone, bool ack)
{
    qsizetype remaining = data.size();
    do
    {
        QByteArray packet;
        BlufiFrameHeader *header;

        if (remaining > m_fragmentSize)
        {
            packet.resize(Q_OFFSETOF(BlufiFrameHeader, data) + 2 + m_fragmentSize);

            header = reinterpret_cast<BlufiFrameHeader *>(packet.data());

            header->fragmented = 1;
            header->dataSize = m_fragmentSize;
            header->totalSizeLow = static_cast<quint8>(remaining);
            header->totalSizeHigh = static_cast<quint8>(remaining >> 8);
            std::memcpy(reinterpret_cast<void *>(header->content), data.constData() + data.size() - remaining, m_fragmentSize);
        }
        else
        {
            packet.resize(Q_OFFSETOF(BlufiFrameHeader, data) + remaining);

            header = reinterpret_cast<BlufiFrameHeader *>(packet.data());
            header->dataSize = static_cast<quint8>(remaining);
            std::memcpy(reinterpret_cast<void *>(header->data), data.constData() + data.size() - remaining, remaining);
        }

        header->type = type;

        SecurityModes securityMode = header->frameType == FRAME_CONTROL ? m_controlFrameSecurityMode : m_dataFrameSecurityMode;

        header->encrypted = securityMode == SEC_ENCRYPT || securityMode == SEC_BOTH;
        header->checksumEnabled = securityMode == SEC_BOTH || securityMode == SEC_CHECKSUM;
        header->toPhone = toPhone ? 1 : 0;
        header->ackRequired = ack ? 1 : 0;
        header->sequenceNo = m_sendSequenceNo++;

        if ((securityMode == SEC_BOTH || securityMode == SEC_CHECKSUM) && m_checksumHandler)
        {
            header->checksumEnabled = 1;
            const QByteArrayView &dataToBeValidated = QByteArrayView(packet.constData() + Q_OFFSETOF(BlufiFrameHeader, sequenceNo), static_cast<qsizetype>(header->dataSize) + 2);
            quint16 checksum = m_checksumHandler(header->sequenceNo, dataToBeValidated);
            quint8 checksumLow = static_cast<quint8>(checksum);
            quint8 checksumHigh = static_cast<quint8>(checksum >> 8);
            packet.append(checksumLow);
            packet.append(checksumHigh);
        }

        if (header->frameType == FRAME_CONTROL || (header->frameSubType != DATA_NEGOTIATION && header->frameSubType != DATA_ERROR))
        {
            if ((securityMode == SEC_BOTH || securityMode == SEC_ENCRYPT) && m_cryptHandler)
            {
                const QByteArrayView &dataToBeEncrypted = QByteArrayView(packet.constData() + Q_OFFSETOF(BlufiFrameHeader, data), static_cast<qsizetype>(header->dataSize));
                const QByteArray &encryptedData = m_cryptHandler(header->sequenceNo, dataToBeEncrypted, false);

                if (dataToBeEncrypted.size() == encryptedData.size())
                {
                    header->encrypted = 1;
                    std::memcpy(reinterpret_cast<void *>(packet.data() + Q_OFFSETOF(BlufiFrameHeader, data)), encryptedData.constData(), header->dataSize);
                }
                else
                {
                    sendError(ERR_ENCRYPT, toPhone, m_sendErrorAckEnabled);
                }
            }
        }

        if (header->fragmented)
        {
            remaining -= header->dataSize - 2;
        }
        else
        {
            remaining -= header->dataSize;
        }

        emit encodedDataGenerated(packet, toPhone);
    } while (remaining > 0);
}

void BlufiFrameCoder::parseReceivedData(const QByteArray &data, bool toPhone)
{
    const BlufiFrameHeader *header = reinterpret_cast<const BlufiFrameHeader *>(data.constData());
    size_t dataSize = static_cast<size_t>(data.size());
    if (dataSize >= Q_OFFSETOF(BlufiFrameHeader, data) && dataSize >= Q_OFFSETOF(BlufiFrameHeader, data) + header->dataSize)
    {
        if (toPhone == header->toPhone)
        {
            if (header->sequenceNo == m_receiveSequenceNo)
            {
                m_receiveSequenceNo++;

                if (header->encrypted && m_cryptHandler) {
                    const QByteArrayView &dataToBeDecrypted = QByteArrayView(data.constData() + Q_OFFSETOF(BlufiFrameHeader, data), static_cast<qsizetype>(header->dataSize));
                    const QByteArray &decryptedData = m_cryptHandler(header->sequenceNo, dataToBeDecrypted, true);

                    if (decryptedData.size() == dataToBeDecrypted.size()) {
                        QByteArray packet;
                        packet.reserve(data.size());
                        packet.append(data.first(Q_OFFSETOF(BlufiFrameHeader, data)));
                        packet.append(decryptedData);
                        packet.append(data.last(2));
                        parseDecryptedData(packet, toPhone);
                    }
                    else {
                        sendError(ERR_DECRYPT, toPhone, m_sendErrorAckEnabled);
                    }
                }
                else {
                    parseDecryptedData(data, toPhone);
                }
            }
            else {
                sendError(ERR_SEQUENCE, toPhone, m_sendErrorAckEnabled);
            }
        }
        else {
            qWarning() << qobject_cast<BlufiClient *>(parent())->controller()->remoteName() << qobject_cast<BlufiClient *>(parent())->controller()->remoteAddress() << __func__ << ERR_DATA_FORMAT;
        }
    }
    else {
        qWarning() << qobject_cast<BlufiClient *>(parent())->controller()->remoteName() << qobject_cast<BlufiClient *>(parent())->controller()->remoteAddress() << __func__ << ERR_DATA_FORMAT;
    }
}

void BlufiFrameCoder::dispatchFrame(quint8 frameType, quint8 frameSubType, const QByteArray &data, bool toPhone)
{
    if (frameType == FRAME_CONTROL)
    {
        if (QMetaEnum::fromType<ControlFrameTypes>().valueToKey(frameSubType) != nullptr)
        {
            emit controlFrameReceived(static_cast<ControlFrameTypes>(frameSubType), data, toPhone);
        }
        else
        {
            /// unknown control frame type
            sendError(ERR_DATA_FORMAT, !toPhone, m_sendErrorAckEnabled);
        }
    }
    else if (frameType == FRAME_DATA)
    {
        if (QMetaEnum::fromType<DataFrameTypes>().valueToKey(frameSubType) != nullptr)
        {
            emit dataFrameReceived(static_cast<DataFrameTypes>(frameSubType), data, toPhone);
        }
        else
        {
            /// unknown data frame type
            sendError(ERR_DATA_FORMAT, !toPhone, m_sendErrorAckEnabled);
        }
    }
    else
    {
        /// unknown frame type
        sendError(ERR_DATA_FORMAT, !toPhone, m_sendErrorAckEnabled);
    }
}

void BlufiFrameCoder::parseDecryptedData(const QByteArray &packet, bool toPhone)
{
    bool checksumOk = true;
    const BlufiFrameHeader *header = reinterpret_cast<const BlufiFrameHeader *>(packet.constData());

    if (header->checksumEnabled && m_checksumHandler)
    {
        if (packet.size() == Q_OFFSETOF(BlufiFrameHeader, data) + header->dataSize + 2)
        {
            const QByteArrayView &dataToBeValidated = QByteArrayView(packet.constData() + Q_OFFSETOF(BlufiFrameHeader, sequenceNo), static_cast<qsizetype>(header->dataSize) + 2);
            quint16 checksum = m_checksumHandler(header->sequenceNo, dataToBeValidated);

            const QByteArrayView &last2Bytes = packet.last(2);
            quint16 packetChecksum = last2Bytes[1];
            packetChecksum <<= 8;
            packetChecksum |= last2Bytes[0];

            checksumOk = checksum == packetChecksum;
        }
        else
        {
            checksumOk = false;
            sendError(ERR_DATA_FORMAT, !toPhone, m_sendErrorAckEnabled);
        }
    }

    if (checksumOk)
    {
        if (header->ackRequired)
        {
            sendAck(header->sequenceNo, !toPhone);
        }

        if (header->fragmented)
        {
            if (m_offset == 0)
            {
                quint16 totalSize = header->totalSizeHigh;
                totalSize <<= 8;
                totalSize |= header->totalSizeLow;

                m_totalSize = totalSize;
                m_buffer.clear();
                m_buffer.reserve(m_totalSize);
            }

            if (m_offset + header->dataSize - 2 <= m_totalSize)
            {
                m_buffer.append(reinterpret_cast<const char *>(header->content), header->dataSize - 2);
                m_offset += header->dataSize - 2;
            }
        }
        else
        {
            if (m_offset > 0)
            { /* if previous pkt is frag */
                m_buffer.append(reinterpret_cast<const char *>(header->data), header->dataSize);
                dispatchFrame(header->frameType, header->frameSubType, m_buffer, toPhone);
            }
            else
            {
                dispatchFrame(header->frameType, header->frameSubType, QByteArray(reinterpret_cast<const char *>(header->data), header->dataSize), toPhone);
            }

            m_offset = 0;
        }
    }
    else
    {
        sendError(ERR_CHECKSUM, toPhone, m_sendErrorAckEnabled);
    }
}

void BlufiFrameCoder::parseDataFrame(BlufiFrameCoder::DataFrameTypes type, const QByteArrayView& data, bool toPhone)
{
    switch (type) {
        case DATA_NEGOTIATION: {
            emit negotiationDataReceived(data, toPhone);
            break;
        }
        case DATA_STA_BSSID: {
            emit staBssidReceived(data, toPhone);
            break;
        }
        case DATA_STA_SSID: {
            emit staSsidReceived(data, toPhone);
            break;
        }
        case DATA_STA_PASSWORD: {
            emit staPasswordReceived(data, toPhone);
            break;
        }
        case DATA_AP_SSID: {
            emit apSsidReceived(data, toPhone);
            break;
        }
        case DATA_AP_PASSWORD: {
            emit apPasswordReceived(data, toPhone);
            break;
        }
        case DATA_AP_MAX_CONNECTIONS: {
            emit apMaxConnectionsReceived(data[0], toPhone);
            break;
        }
        case DATA_AP_AUTH_MODE: {
            if (QMetaEnum::fromType<WiFiAuthModes>().valueToKey(data[0]) != nullptr) {
                emit apAuthModeReceived(static_cast<WiFiAuthModes>(data[0]), toPhone);
            }
            break;
        }
        case DATA_AP_CHANNELS: {
            if (data[0] == data.size() - 1) {
                QList<quint8> channels;
                for (auto i = data.begin() + 1; i != data.end(); i++) {
                    channels.append(*i);
                }
                emit apChannelsReceived(channels, toPhone);
            }
            break;
        }
        case DATA_USERNAME: {
            emit usernameReceived(data, toPhone);
            break;
        }
        case DATA_CA_CERT: {
            emit caCertReceived(data, toPhone);
            break;
        }
        case DATA_CLIENT_CERT: {
            emit clientCertReceived(data, toPhone);
            break;
        }
        case DATA_SERVER_CERT: {
            emit serverCertReceived(data, toPhone);
            break;
        }
        case DATA_CLIENT_KEY: {
            emit clientKeyReceived(data, toPhone);
            break;
        }
        case DATA_SERVER_KEY: {
            emit serverKeyReceived(data, toPhone);
            break;
        }
        case DATA_STATUS: {
            if (data.size() >= 3) {
                if (QMetaEnum::fromType<WiFiModes>().valueToKey(data[0]) != nullptr) {
                    emit wifiModeReceived(static_cast<WiFiModes>(data[0]), toPhone);
                }
                if (QMetaEnum::fromType<ConnectionStatus>().valueToKey(data[1]) != nullptr) {
                    emit connectionStatusReceived(static_cast<ConnectionStatus>(data[1]), toPhone);
                }
                emit apConnectionsReceived(data[2], toPhone);
            
                auto i = data.begin() + 3;
                while (i + 2 < data.end()) {
                    quint8 type = *i++;
                    if (QMetaEnum::fromType<DataFrameTypes>().valueToKey(type) != nullptr) {
                        DataFrameTypes frameType = static_cast<DataFrameTypes>(type);
                        quint8 size = *i++;
                        auto end = i += size;
                        if (end <= data.end()) {
                            auto begin = i;
                            parseDataFrame(frameType, QByteArrayView(begin, end), toPhone);
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        break;
                    }
                }
            }
            break;
        }
        case DATA_VERSION: {
            if (data.size() == 2) {
                emit blufiVersionReceived(data[0], data[1], toPhone);
            }
            break;
        }
        case DATA_WIFI_LIST: {
            
            QList<QPair<QByteArray, qint8>> ssidRssiPairs;

            auto i = data.begin();
            while (i + 2 < data.end()) {
                quint8 size = *i++;
                qint8 rssi;
                std::memcpy(reinterpret_cast<void*>(&rssi), reinterpret_cast<const void*>(i), sizeof rssi);

                auto begin = i;
                auto end = i + size - 1;
                if (end < data.end()) {
                    ssidRssiPairs += qMakePair(QByteArray().append(QByteArrayView(begin, end)), rssi);
                }
                else {
                    break;
                }
            }

            emit wifiListReceived(ssidRssiPairs, toPhone);

            break;
        }
        case DATA_ERROR: {
            if (QMetaEnum::fromType<Errors>().valueToKey(data[0]) != nullptr) {
                emit errorReceived(static_cast<Errors>(data[0]), toPhone);
            }
            break;
        }
        case DATA_CUSTOM: {
            emit customDataReceived(data, toPhone);
            break;
        }
        case DATA_MAX_RECONNECT_TIME: {
            emit maxConnectionRetriesReceived(data[0], toPhone);
            break;
        }
        case DATA_CONNECTION_END_REASON: {
            if (QMetaEnum::fromType<WiFiFailureReason>().valueToKey(data[0]) != nullptr) {
                emit connectionFailureReasonReceived(static_cast<WiFiFailureReason>(data[0]), toPhone);
            }
            break;
        }
        case DATA_RSSI_AT_CONNECTION_END: {
            qint8 rssi;
            std::memcpy(reinterpret_cast<void*>(&rssi), reinterpret_cast<const void*>(data.constData()), sizeof rssi);
            emit connectionFailureRssiReceived(rssi, toPhone);
            break;
        }
        default:
            break;
    }
}

void BlufiFrameCoder::parseControlFrame(BlufiFrameCoder::ControlFrameTypes type, const QByteArrayView& data, bool toPhone)
{
    switch (type) {
        case CONTROL_ACK: {
            if (!data.isEmpty()) {
                emit ackReceived(data[0], toPhone);
            }
            break;
        }
        case CONTROL_SECURITY_MODE_SET: {
            if (!data.isEmpty()) {
                quint8 controlFrameSecurityModeValue = (data[0] & 0xf0) >> 4;
                if (QMetaEnum::fromType<SecurityModes>().valueToKey(controlFrameSecurityModeValue) != nullptr) {
                    emit controlFrameSecurityModeReceived(static_cast<SecurityModes>(controlFrameSecurityModeValue), toPhone);
                }

                quint8 dataFrameSecurityModeValue = data[0] & 0x0f;
                if (QMetaEnum::fromType<SecurityModes>().valueToKey(dataFrameSecurityModeValue) != nullptr) {
                    emit dataFrameSecurityModeReceived(static_cast<SecurityModes>(dataFrameSecurityModeValue), toPhone);
                }
            }
            break;
        }
        case CONTROL_OPMODE_SET: {
            if (!data.isEmpty()) {
                if (QMetaEnum::fromType<WiFiModes>().valueToKey(data[0]) != nullptr) {
                    emit wifiModeReceived(static_cast<WiFiModes>(data[0]), toPhone);
                }
            }
            break;
        }
        case CONTROL_STA_CONNECT: {
            emit staConnectRequestReceived(toPhone);
            break;
        }
        case CONTROL_STA_DISCONNECT: {
            emit staDisonnectRequestReceived(toPhone);
            break;
        }
        case CONTROL_STATUS_GET: {
            emit wifiStatusGetRequestReceived(toPhone);
            break;
        }
        case CONTROL_AP_DISCONNECT: {
            if (data.size() % 6 == 0) {
                for (auto i = data.begin(); i < data.end(); i += 6) {
                    emit apClientDisconnectRequestReceived(QByteArrayView(i, i + 6), toPhone);
                }
            }
            break;
        }
        case CONTROL_VERSION_GET: {
            emit blufiVersionRequestReceived(toPhone);
            break;
        }
        case CONTROL_GATT_DISCONNECT: {
            emit gattDisconnectRequestReceived(toPhone);
            break;
        }
        case CONTROL_WIFI_LIST_GET: {
            emit wifiListGetRequestReceived(toPhone);
            break;
        }
        default:
            break;
    }
}

void BlufiFrameCoder::onNegotiationDataReceived(const QByteArrayView& data, bool toPhone)
{
    if (m_negotiateDataHandler) {
        sendNegotiationData(m_negotiateDataHandler(data, toPhone), toPhone, m_sendNegotiationDataAckEnable);
    }
}

void BlufiFrameCoder::sendDataFrame(DataFrameTypes type, const QByteArray &data, bool toPhone, bool ack)
{
    quint8 frameType = type;
    frameType <<= 2;
    frameType |= FRAME_DATA;

    sendFrame(frameType, data, toPhone, ack);
}

void BlufiFrameCoder::sendControlFrame(ControlFrameTypes type, const QByteArray &data, bool toPhone, bool ack)
{
    quint8 frameType = type;
    frameType <<= 2;
    frameType |= FRAME_CONTROL;

    sendFrame(frameType, data, toPhone, ack);
}

void BlufiFrameCoder::sendAck(quint8 sequenceNo, bool toPhone)
{
    QByteArray data;
    data.append(sequenceNo);
    sendControlFrame(CONTROL_ACK, data, toPhone, false);
}

void BlufiFrameCoder::sendError(Errors error, bool toPhone, bool ack)
{
    qWarning() << qobject_cast<BlufiClient *>(parent())->controller()->remoteName() << qobject_cast<BlufiClient *>(parent())->controller()->remoteAddress() << __func__ << error;

    QByteArray data;
    data.append(static_cast<quint8>(error));
    sendDataFrame(DATA_ERROR, data, toPhone, ack);
}

void BlufiFrameCoder::sendStaSsid(const QString &ssid, bool ack)
{
    sendDataFrame(DATA_STA_SSID, ssid.toUtf8(), false, ack);
}

void BlufiFrameCoder::sendStaPassword(const QString &password, bool ack)
{
    sendDataFrame(DATA_STA_PASSWORD, password.toUtf8(), false, ack);
}

void BlufiFrameCoder::sendStaConnectionRequest(bool ack)
{
    sendControlFrame(CONTROL_STA_CONNECT, QByteArray(), false, ack);
}

void BlufiFrameCoder::sendWifiStatusQueryRequest(bool ack)
{
    sendControlFrame(CONTROL_STATUS_GET, QByteArray(), false, ack);
}

void BlufiFrameCoder::sendNegotiationData(const QByteArray& data, bool toPhone, bool ack)
{
    sendDataFrame(DATA_NEGOTIATION, data, toPhone, ack);
}

void BlufiFrameCoder::sendGattDisconnectRequest(bool ack)
{
    sendControlFrame(CONTROL_GATT_DISCONNECT, QByteArray(), false);
}
