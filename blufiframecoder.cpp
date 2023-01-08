#include "blufiframecoder.h"
#include <QByteArray>
#include <QDebug>

extern "C" {

#pragma pack(1)

struct BlufiFrameHeader {
    union {
        struct {
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
    union {
        struct {
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
}

void BlufiFrameCoder::sendFrame(quint8 type, const QByteArrayView &data, bool toPhone, bool ack)
{
    qsizetype remaining = data.size();
    while (remaining > 0) {

        QByteArray packet;
        BlufiFrameHeader *header;

        if (remaining > m_fragmentSize) {
            packet.resize(Q_OFFSETOF(BlufiFrameHeader, data) + 2 + m_fragmentSize + 2);

            header = reinterpret_cast<BlufiFrameHeader*>(packet.data());

            header->fragmented = 1;
            header->dataSize = m_fragmentSize;
            header->totalSizeLow = static_cast<quint8>(remaining);
            header->totalSizeHigh = static_cast<quint8>(remaining >> 8);
            std::memcpy(reinterpret_cast<void*>(header->content), data.constData() + data.size() - remaining, m_fragmentSize);
        }
        else {
            packet.resize(Q_OFFSETOF(BlufiFrameHeader, data) + remaining + 2);

            header = reinterpret_cast<BlufiFrameHeader*>(packet.data());
            header->dataSize = static_cast<quint8>(remaining);
            std::memcpy(reinterpret_cast<void*>(header->data), data.constData() + data.size() - remaining, remaining);
        }

        header->type = type;

        SecurityModes securityMode = header->frameType == FRAME_CONTROL ? m_controlFrameSecurityMode : m_dataFrameSecurityMode;

        header->encrypted = securityMode == SEC_ENCRYPT || securityMode == SEC_BOTH;
        header->checksumEnabled = securityMode == SEC_BOTH || securityMode == SEC_CHECKSUM;
        header->toPhone = toPhone ? 1 : 0;
        header->ackRequired = ack ? 1 : 0;
        header->sequenceNo = m_sendSequenceNo++;

        if ((securityMode == SEC_BOTH || securityMode == SEC_CHECKSUM) && m_checksumHandler) {
            header->checksumEnabled = 1;
            const QByteArrayView &dataToBeValidated = QByteArrayView(packet.constData() + Q_OFFSETOF(BlufiFrameHeader, sequenceNo), static_cast<qsizetype>(header->dataSize) + 2);
            quint16 checksum = m_checksumHandler(header->sequenceNo, dataToBeValidated);
            quint8 checksumLow = static_cast<quint8>(checksum);
            quint8 checksumHigh = static_cast<quint8>(checksum >> 8);
            header->data[header->dataSize] = checksumLow;
            header->data[header->dataSize + 1] = checksumHigh;
        }

        if (header->frameType == FRAME_CONTROL || (header->frameSubType != DATA_NEGOTIATION && header->frameSubType != DATA_ERROR)) {
            if ((securityMode == SEC_BOTH || securityMode == SEC_ENCRYPT) && m_cryptHandler) {
                const QByteArrayView &dataToBeEncrypted = QByteArrayView(packet.constData() + Q_OFFSETOF(BlufiFrameHeader, data), static_cast<qsizetype>(header->dataSize));
                const QByteArray &encryptedData = m_cryptHandler(header->sequenceNo, dataToBeEncrypted, false);

                if (dataToBeEncrypted.size() == encryptedData.size()) {
                    header->encrypted = 1;
                    std::memcpy(reinterpret_cast<void*>(packet.data() + Q_OFFSETOF(BlufiFrameHeader, data)), encryptedData.constData(), header->dataSize);
                }
                else {
                    sendErrorFrame(ERR_ENCRYPT, toPhone);
                }
            }
        }

        if (header->fragmented) {
            remaining -= header->dataSize - 2;
        }
        else {
            remaining -= header->dataSize;
        }

        emit encodedDataGenerated(data, toPhone);
    }
}

void BlufiFrameCoder::parseReceivedData(const QByteArrayView &data, bool toPhone)
{
    const BlufiFrameHeader * header = reinterpret_cast<const BlufiFrameHeader*>(data.constData());
    size_t dataSize = static_cast<size_t>(data.size());
    if (dataSize >= Q_OFFSETOF(BlufiFrameHeader, data) && dataSize >= Q_OFFSETOF(BlufiFrameHeader, data) + header->dataSize) {
        if (toPhone == header->toPhone) {
            if (header->sequenceNo == m_receiveSequenceNo) {

                m_receiveSequenceNo++;

                QByteArray packet;

                bool decryptOk = true;
                if (header->encrypted && m_cryptHandler) {
                    const QByteArrayView &dataToBeDecrypted = QByteArrayView(data.constData() + Q_OFFSETOF(BlufiFrameHeader, data), static_cast<qsizetype>(header->dataSize));
                    const QByteArray &decryptedData = m_cryptHandler(header->sequenceNo, dataToBeDecrypted, true);

                    if (decryptedData.size() == dataToBeDecrypted.size()) {
                        packet.reserve(data.size());
                        packet.append(data.first(Q_OFFSETOF(BlufiFrameHeader, data)));
                        packet.append(decryptedData);
                        packet.append(data.last(2));
                    }
                    else {
                        decryptOk = false;
                    }
                }
                else {
                    packet.append(data);
                }

                if (decryptOk) {

                    bool checksumOk = true;
                    header = reinterpret_cast<const BlufiFrameHeader*>(packet.constData());

                    if (header->checksumEnabled && m_checksumHandler) {
                        if (data.size() == Q_OFFSETOF(BlufiFrameHeader, data) + header->dataSize + 2) {
                            const QByteArrayView &dataToBeValidated = QByteArrayView(packet.constData() + Q_OFFSETOF(BlufiFrameHeader, sequenceNo), static_cast<qsizetype>(header->dataSize) + 2);
                            quint16 checksum = m_checksumHandler(header->sequenceNo, dataToBeValidated);

                            const QByteArray &last2Bytes = packet.last(2);
                            quint16 packetChecksum = last2Bytes[1];
                            packetChecksum <<= 8;
                            packetChecksum |= last2Bytes[0];

                            checksumOk = checksum == packetChecksum;
                        }
                        else {
                            checksumOk = false;
                            sendErrorFrame(ERR_DATA_FORMAT, !toPhone);
                        }
                    }

                    if (checksumOk) {
                        if (header->ackRequired) {
                            sendAckFrame(header->sequenceNo, !toPhone);
                        }

                        if (header->fragmented) {
                            if (m_offset == 0) {
                                quint16 totalSize = header->totalSizeHigh;
                                totalSize <<= 8;
                                totalSize |= header->totalSizeLow;

                                m_totalSize = totalSize;
                                m_buffer.clear();
                                m_buffer.reserve(m_totalSize);
                            }

                            if (m_offset + header->dataSize  - 2 <= m_totalSize){
                                m_buffer.append(reinterpret_cast<const char*>(header->content), header->dataSize - 2);
                                m_offset += header->dataSize - 2;
                            }
                        }
                        else {

                            QByteArrayView frameData;

                            if (m_offset > 0) {   /* if previous pkt is frag */
                                m_buffer.append(reinterpret_cast<const char*>(header->data), header->dataSize);
                                frameData = QByteArrayView(m_buffer);
                            } else {
                                frameData = QByteArrayView(header->data, header->dataSize);
                            }

                            if (header->frameType == FRAME_CONTROL) {
                                if (header->frameSubType < CONTROL_FRAME_TYPE_END) {
                                    emit controlFrameReceived(static_cast<ControlFrameTypes>(header->frameSubType), frameData, header->toPhone ? true : false);
                                }
                                else {
                                    /// unknown control frame type
                                    sendErrorFrame(ERR_DATA_FORMAT, !toPhone);
                                }
                            }
                            else if (header->frameType == FRAME_DATA) {
                                if (header->frameSubType < DATA_FRAME_TYPE_END) {
                                    emit dataFrameReceived(static_cast<DataFrameTypes>(header->frameSubType), frameData, header->toPhone ? true : false);
                                }
                                else {
                                    /// unknown data frame type
                                    sendErrorFrame(ERR_DATA_FORMAT, !toPhone);
                                }
                            }
                            else {
                                /// unknown frame type
                                sendErrorFrame(ERR_DATA_FORMAT, toPhone);
                            }

                            m_offset = 0;
                        }
                    }
                    else {
                        sendErrorFrame(ERR_CHECKSUM, toPhone);
                    }
                }
                else {
                    sendErrorFrame(ERR_DECRYPT, toPhone);
                }
            }
            else {
                sendErrorFrame(ERR_SEQUENCE, toPhone);
            }
        }
        else {
            qDebug() << __func__ << "ERR_DATA_FORMAT";
        }
    }
    else {
        qDebug() << __func__ << "ERR_DATA_FORMAT";
    }
}

void BlufiFrameCoder::sendDataFrame(DataFrameTypes type, const QByteArrayView &data, bool toPhone, bool ack)
{
    quint8 frameType = type;
    frameType <<= 2;
    frameType |= FRAME_DATA;

    sendFrame(frameType, data, toPhone, ack);
}

void BlufiFrameCoder::sendControlFrame(ControlFrameTypes type, const QByteArrayView &data, bool toPhone, bool ack)
{
    quint8 frameType = type;
    frameType <<= 2;
    frameType |= FRAME_CONTROL;

    sendFrame(frameType, data, toPhone, ack);
}

void BlufiFrameCoder::sendAckFrame(quint8 sequenceNo, bool toPhone)
{
    QByteArray data;
    data.append(sequenceNo);
    sendControlFrame(CONTROL_ACK, data, toPhone, false);
}

void BlufiFrameCoder::sendErrorFrame(Errors error, bool toPhone)
{
    QByteArray data;
    data.append(static_cast<quint8>(error));
    sendDataFrame(DATA_ERROR, data, toPhone, false);
}
