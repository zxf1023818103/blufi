#ifndef BLUFIFRAMECODER_H
#define BLUFIFRAMECODER_H

#include <QObject>
#include <functional>

using BlufiNegotiateDataHandler = std::function<const QByteArray&(const QByteArrayView& data, bool toPhone)>;

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
    Q_ENUM(FrameTypes);

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
    };
    Q_ENUM(ControlFrameTypes);

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
    };
    Q_ENUM(DataFrameTypes);

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
    };
    Q_ENUM(Errors);

    enum SecurityModes {
        SEC_NONE = 0,
        SEC_CHECKSUM,
        SEC_ENCRYPT,
        SEC_BOTH,
    };
    Q_ENUM(SecurityModes);

    enum ConnectionStatus {
        STATUS_CONNECTED = 0,
        STATUS_DISCONNECTED,
        STATUS_CONNECTING,
        STATUS_IP_ASSIGNING,
    };
    Q_ENUM(ConnectionStatus);

    enum WiFiModes {
        WIFI_MODE_NULL = 0,  /**< null mode */
        WIFI_MODE_STA,       /**< WiFi station mode */
        WIFI_MODE_AP,        /**< WiFi soft-AP mode */
        WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
    };
    Q_ENUM(WiFiModes);

    enum WiFiAuthModes {
        WIFI_AUTH_OPEN = 0,         /**< authenticate mode : open */
        WIFI_AUTH_WEP,              /**< authenticate mode : WEP */
        WIFI_AUTH_WPA_PSK,          /**< authenticate mode : WPA_PSK */
        WIFI_AUTH_WPA2_PSK,         /**< authenticate mode : WPA2_PSK */
        WIFI_AUTH_WPA_WPA2_PSK,     /**< authenticate mode : WPA_WPA2_PSK */
        WIFI_AUTH_WPA2_ENTERPRISE,  /**< authenticate mode : WPA2_ENTERPRISE */
        WIFI_AUTH_WPA3_PSK,         /**< authenticate mode : WPA3_PSK */
        WIFI_AUTH_WPA2_WPA3_PSK,    /**< authenticate mode : WPA2_WPA3_PSK */
        WIFI_AUTH_WAPI_PSK,         /**< authenticate mode : WAPI_PSK */
    };
    Q_ENUM(WiFiAuthModes);

    enum WiFiFailureReason {
        WIFI_REASON_UNSPECIFIED              = 1,
        WIFI_REASON_AUTH_EXPIRE              = 2,
        WIFI_REASON_AUTH_LEAVE               = 3,
        WIFI_REASON_ASSOC_EXPIRE             = 4,
        WIFI_REASON_ASSOC_TOOMANY            = 5,
        WIFI_REASON_NOT_AUTHED               = 6,
        WIFI_REASON_NOT_ASSOCED              = 7,
        WIFI_REASON_ASSOC_LEAVE              = 8,
        WIFI_REASON_ASSOC_NOT_AUTHED         = 9,
        WIFI_REASON_DISASSOC_PWRCAP_BAD      = 10,
        WIFI_REASON_DISASSOC_SUPCHAN_BAD     = 11,
        WIFI_REASON_BSS_TRANSITION_DISASSOC  = 12,
        WIFI_REASON_IE_INVALID               = 13,
        WIFI_REASON_MIC_FAILURE              = 14,
        WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT   = 15,
        WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT = 16,
        WIFI_REASON_IE_IN_4WAY_DIFFERS       = 17,
        WIFI_REASON_GROUP_CIPHER_INVALID     = 18,
        WIFI_REASON_PAIRWISE_CIPHER_INVALID  = 19,
        WIFI_REASON_AKMP_INVALID             = 20,
        WIFI_REASON_UNSUPP_RSN_IE_VERSION    = 21,
        WIFI_REASON_INVALID_RSN_IE_CAP       = 22,
        WIFI_REASON_802_1X_AUTH_FAILED       = 23,
        WIFI_REASON_CIPHER_SUITE_REJECTED    = 24,

        WIFI_REASON_INVALID_PMKID            = 53,

        WIFI_REASON_BEACON_TIMEOUT           = 200,
        WIFI_REASON_NO_AP_FOUND              = 201,
        WIFI_REASON_AUTH_FAIL                = 202,
        WIFI_REASON_ASSOC_FAIL               = 203,
        WIFI_REASON_HANDSHAKE_TIMEOUT        = 204,
        WIFI_REASON_CONNECTION_FAIL          = 205,
        WIFI_REASON_AP_TSF_RESET             = 206,
        WIFI_REASON_ROAMING                  = 207,
    };
    Q_ENUM(WiFiFailureReason);

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

    bool m_sendErrorAckEnabled = false;

    bool m_sendNegotiationDataAckEnable = false;

    void sendFrame(quint8 type, const QByteArray &data, bool toPhone, bool ack);

    void parseDecryptedData(const QByteArray &data, bool toPhone);

    void dispatchFrame(quint8 frameType, quint8 frameSubType, const QByteArray &data, bool toPhone);

signals:
    void encodedDataGenerated(const QByteArray &data, bool toPhone);

    void dataFrameReceived(BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone);

    void controlFrameReceived(BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone);

    void errorOccurred(BlufiFrameCoder::Errors error);

    void negotiationDataReceived(const QByteArrayView &staBssid, bool toPhone);

    void staBssidReceived(const QByteArrayView &staBssid, bool toPhone);

    void staSsidReceived(const QByteArrayView &staSsid, bool toPhone);

    void staPasswordReceived(const QByteArrayView &password, bool toPhone);

    void apSsidReceived(const QByteArrayView &apSsid, bool toPhone);

    void apPasswordReceived(const QByteArrayView &apPassword, bool toPhone);

    void apMaxConnectionsReceived(quint8 maxConnections, bool toPhone);

    void apAuthModeReceived(BlufiFrameCoder::WiFiAuthModes wifiAuthMode, bool toPhone);

    void apChannelsReceived(const QList<quint8> &channels, bool toPhone);

    void usernameReceived(const QByteArrayView &username, bool toPhone);

    void caCertReceived(const QByteArrayView &caCert, bool toPhone);

    void clientCertReceived(const QByteArrayView &clientCert, bool toPhone);
    
    void serverCertReceived(const QByteArrayView &serverCert, bool toPhone);

    void clientKeyReceived(const QByteArrayView &clientKey, bool toPhone);

    void serverKeyReceived(const QByteArrayView &serverKey, bool toPhone);

    void wifiModeReceived(BlufiFrameCoder::WiFiModes wifiMode, bool toPhone);

    void connectionStatusReceived(BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone);

    void apConnectionsReceived(quint8 apConnections, bool toPhone);

    void blufiVersionReceived(quint8 majorVersion, quint8 minorVersion, bool toPhone);

    void wifiListReceived(const QList<QPair<QByteArray, qint8>> &ssidRssiPairs, bool toPhone);

    void errorReceived(BlufiFrameCoder::Errors error, bool toPhone);

    void customDataReceived(const QByteArrayView &customData, bool toPhone);

    void maxConnectionRetriesReceived(qint8 maxConnectionRetries, bool toPhone);

    void connectionFailureReasonReceived(BlufiFrameCoder::WiFiFailureReason reason, bool toPhone);

    void connectionFailureRssiReceived(qint8 connectionFailureRssi, bool toPhone);

    void ackReceived(quint8 sequenceNo, bool toPhone);

    void dataFrameSecurityModeReceived(BlufiFrameCoder::SecurityModes mode, bool toPhone);

    void controlFrameSecurityModeReceived(BlufiFrameCoder::SecurityModes mode, bool toPhone);

    void staConnectRequestReceived(bool toPhone);

    void staDisonnectRequestReceived(bool toPhone);

    void wifiStatusGetRequestReceived(bool toPhone);

    void apClientDisconnectRequestReceived(const QByteArrayView &clientMacAddress, bool toPhone);

    void blufiVersionRequestReceived(bool toPhone);

    void gattDisconnectRequestReceived(bool toPhone);

    void wifiListGetRequestReceived(bool toPhone);

public slots:
    inline void setNegotiateDataHandler(BlufiNegotiateDataHandler negotiateDataHandler) { m_negotiateDataHandler = negotiateDataHandler; }

    inline void setCryptHandler(BlufiCryptHandler cryptHandler) { m_cryptHandler = cryptHandler; }

    inline void setChecksumHandler(BlufiChecksumHandler checksumHandler) { m_checksumHandler = checksumHandler; }

    inline void setFragmentSize(quint8 fragmentSize) { m_fragmentSize = fragmentSize; }

    inline void setControlFrameSecurityMode(BlufiFrameCoder::SecurityModes controlFrameSecurityMode) { m_controlFrameSecurityMode = controlFrameSecurityMode; }

    inline void setDataFrameSecurityMode(BlufiFrameCoder::SecurityModes dataFrameSecurityMode) { m_dataFrameSecurityMode = dataFrameSecurityMode; }

    void parseReceivedData(const QByteArray &data, bool toPhone);

    void sendDataFrame(BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone, bool ack = false);

    void sendControlFrame(BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone, bool ack = false);

    void sendAck(quint8 sequenceNo, bool toPhone);

    void sendError(BlufiFrameCoder::Errors, bool toPhone, bool ack = false);

    void sendStaSsid(const QString &ssid, bool ack = false);

    void sendStaPassword(const QString &password, bool ack = false);

    void sendStaConnectionRequest(bool ack = false);

    void sendWifiStatusQueryRequest(bool ack = false);

    void sendGattDisconnectRequest(bool ack = false);

private slots:
    void parseDataFrame(BlufiFrameCoder::DataFrameTypes type, const QByteArrayView& data, bool toPhone);

    void parseControlFrame(BlufiFrameCoder::ControlFrameTypes type, const QByteArrayView& data, bool toPhone);

    void onNegotiationDataReceived(const QByteArrayView& data, bool toPhone);

    void sendNegotiationData(const QByteArray& data, bool toPhone, bool ack);
};

#endif // BLUFIFRAMECODER_H
