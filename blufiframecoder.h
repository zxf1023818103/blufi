#ifndef BLUFIFRAMECODER_H
#define BLUFIFRAMECODER_H

#include <QObject>
#include <functional>

using BlufiNegotiateDataHandler = std::function<const QByteArray&(const QByteArrayView& data)>;

using BlufiCryptHandler = std::function<const QByteArray&(quint8 iv, const QByteArrayView& data, bool decrypt)>;

using BlufiChecksumHandler = std::function<quint16(quint8 iv8, const QByteArrayView& data)>;

class BlufiFrameCoder : public QObject
{
    Q_OBJECT
public:

    enum FrameTypes {
        FRAME_CONTROL = 0,
        FRAME_DATA,
    };

    enum ControlFrameTypes {
        CONTROL_ACK = 0,
        CONTROL_SECURITY_MODE_SET,
        CONTROL_OPMODE_SET,
        CONTROL_STA_CONNECT,
        CONTROL_STA_DISCONNECT,
        CONTROL_STATUS_GET,
        CONTROL_AP_DISCONNECT,
        CONTROL_VERSION_GET,
        CONTROL_GATT_DISCONNECT,
        CONTROL_WIFI_LIST_GET,
        CONTROL_FRAME_TYPE_END,
    };

    enum DataFrameTypes {
        DATA_NEGOTIATION = 0,
        DATA_STA_BSSID,
        DATA_STA_SSID,
        DATA_STA_PASSWORD,
        DATA_AP_SSID,
        DATA_AP_PASSWORD,
        DATA_AP_MAX_CONNECTIONS,
        DATA_AP_AUTH_MODE,
        DATA_AP_CHANNELS,
        DATA_USERNAME,
        DATA_CA_CERT,
        DATA_CLIENT_CERT,
        DATA_SERVER_CERT,
        DATA_CLIENT_KEY,
        DATA_SERVER_KEY,
        DATA_STATUS,
        DATA_VERSION,
        DATA_WIFI_LIST,
        DATA_ERROR,
        DATA_CUSTOM,
        DATA_MAX_RECONNECT_TIME,
        DATA_CONNECTION_END_REASON,
        DATA_RSSI_AT_CONNECTION_END,
        DATA_FRAME_TYPE_END,
    };

    enum Errors {
        ERR_SEQUENCE = 0,
        ERR_CHECKSUM,
        ERR_DECRYPT,
        ERR_ENCRYPT,
        ERR_SECURITY,
        ERR_DH_MALLOC,
        ERR_DH_PARAM,
        ERR_READ_PARAM,
        ERR_MAKE_PUBLIC,
        ERR_DATA_FORMAT,
        ERR_CALCULATE_MD5,
        ERR_WIFI_SCAN,
        ERR_END,
    };

    enum SecurityModes {
        SEC_NONE = 0,
        SEC_CHECKSUM,
        SEC_ENCRYPT,
        SEC_BOTH,
        SEC_MODE_END,
    };

    explicit BlufiFrameCoder(QObject *parent = nullptr);

private:
    BlufiNegotiateDataHandler m_negotiateDataHandler;

    BlufiCryptHandler m_cryptHandler;

    BlufiChecksumHandler m_checksumHandler;

    quint8 m_receiveSequenceNo = 0;

    quint8 m_sendSequenceNo = 0;

    quint16 m_totalSize = 0;

    quint16 m_offset = 0;

    QByteArray m_buffer;

    SecurityModes m_controlFrameSecurityMode = SEC_NONE;

    SecurityModes m_dataFrameSecurityMode = SEC_NONE;

    quint8 m_fragmentSize = 230;

    void sendFrame(quint8 type, const QByteArray &data, bool toPhone, bool ack);

    void parseDecryptedData(const QByteArray &data, bool toPhone);

    void dispatchFrame(quint8 frameType, quint8 frameSubType, const QByteArray &data, bool toPhone);

signals:
    void encodedDataGenerated(const QByteArray &data, bool toPhone);

    void dataFrameReceived(BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone);

    void controlFrameReceived(BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone);

    void errorOccurred(BlufiFrameCoder::Errors error);

public slots:
    inline void setNegotiateDataHandler(BlufiNegotiateDataHandler negotiateDataHandler) { m_negotiateDataHandler = negotiateDataHandler; }

    inline void setCryptHandler(BlufiCryptHandler cryptHandler) { m_cryptHandler = cryptHandler; }

    inline void setChecksumHandler(BlufiChecksumHandler checksumHandler) { m_checksumHandler = checksumHandler; }

    inline void setFragmentSize(quint8 fragmentSize) { m_fragmentSize = fragmentSize; }

    inline void setControlFrameSecurityMode(BlufiFrameCoder::SecurityModes controlFrameSecurityMode) { m_controlFrameSecurityMode = controlFrameSecurityMode; }

    inline void setDataFrameSecurityMode(BlufiFrameCoder::SecurityModes dataFrameSecurityMode) { m_dataFrameSecurityMode = dataFrameSecurityMode; }

    void parseReceivedData(const QByteArray &data, bool toPhone);

    void sendDataFrame(BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone, bool ack);

    void sendControlFrame(BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone, bool ack);

    void sendAck(quint8 sequenceNo, bool toPhone);

    void sendError(BlufiFrameCoder::Errors, bool toPhone);

    void sendStaSsid(const QString &ssid, bool toPhone);

    void sendStaPassword(const QString &password, bool toPhone);

    void sendStaConnectionRequest(bool toPhone);
};

#endif // BLUFIFRAMECODER_H
