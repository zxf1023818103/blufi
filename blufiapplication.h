#ifndef BLUFIAPPLICATION_H
#define BLUFIAPPLICATION_H

#include <QObject>
#include "blufiscanner.h"
#include "blufiframecoder.h"

class BlufiApplication : public QObject
{
    Q_OBJECT
public:
    explicit BlufiApplication(QObject *parent = nullptr);

private:
    QString m_ssid;

    QString m_psk;

    QString m_nameFilter;

    QString m_addressFilter;

    BlufiScanner * m_scanner;

public slots:
    inline void setSsid(const QString &ssid) { m_ssid = ssid; }

    inline void setPsk(const QString &psk) { m_psk = psk; }

    inline void setNameFilter(const QString &nameFilter) { m_nameFilter = nameFilter; }

    inline void setAddressFilter(const QString &addressFilter) { m_addressFilter = addressFilter; }

    void start();

private slots:
    void onBlufiClientReady(BlufiClient *client);

    void onDataFrameReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone);

    void onControlFrameReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone);

    void onBlufiClientAllDestroyed();

    void onConnectionStatusReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone);
};

#endif // BLUFIAPPLICATION_H
