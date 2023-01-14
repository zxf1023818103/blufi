#include "bluficlientapplication.h"
#include "bluficlientframecoderadapter.h"
#include <QTimer>
#include <QMetaEnum>

BlufiClientApplication::BlufiClientApplication(QObject *parent)
    : QObject{parent}
{
    m_scanner = new BlufiScanner(this);
    m_scanner->setAddressFilter(m_addressFilter);
    m_scanner->setNameFilter(m_nameFilter);

    m_scanner->enableStopDiscoveryAtClientReady(!m_addressFilter.isEmpty() || !m_nameFilter.isEmpty());

    connect(m_scanner, &BlufiScanner::blufiClientReady, this, &BlufiClientApplication::onBlufiClientReady);
    connect(m_scanner, &BlufiScanner::clientAllDestroyed, this, &BlufiClientApplication::onBlufiClientAllDestroyed);
}

void BlufiClientApplication::start()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);
    
    m_scanner->start();
}

void BlufiClientApplication::onBlufiClientReady(BlufiClient *client)
{
    BlufiClientFrameCoderAdapter *adapter = new BlufiClientFrameCoderAdapter(client);
    connect(adapter, &BlufiClientFrameCoderAdapter::dataFrameReceived, this, &BlufiClientApplication::onDataFrameReceived);
    connect(adapter, &BlufiClientFrameCoderAdapter::controlFrameReceived, this, &BlufiClientApplication::onControlFrameReceived);
    connect(adapter, &BlufiClientFrameCoderAdapter::connectionStatusReceived, this, &BlufiClientApplication::onConnectionStatusReceived);

    adapter->frameCoder()->sendStaSsid(m_ssid);
    adapter->frameCoder()->sendStaPassword(m_psk);
    adapter->frameCoder()->sendStaConnectionRequest();
}

void BlufiClientApplication::onDataFrameReceived(BlufiClient *client, BlufiFrameCoder *frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data.toHex(' ');
}

void BlufiClientApplication::onControlFrameReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data.toHex(' ');
}

void BlufiClientApplication::onBlufiClientAllDestroyed()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    deleteLater();
}

void BlufiClientApplication::onConnectionStatusReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone)
{
    qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << connectionStatus;

    if (connectionStatus == BlufiFrameCoder::STATUS_CONNECTED) {
        client->controller()->disconnectFromDevice();
    }
}
