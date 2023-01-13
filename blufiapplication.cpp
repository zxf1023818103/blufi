#include "blufiapplication.h"
#include "bluficlientframecoderadapter.h"
#include <QTimer>
#include <QMetaEnum>

BlufiApplication::BlufiApplication(QObject *parent)
    : QObject{parent}
{
    m_scanner = new BlufiScanner(this);
    m_scanner->setAddressFilter(m_addressFilter);
    m_scanner->setNameFilter(m_nameFilter);

    connect(m_scanner, &BlufiScanner::blufiClientReady, this, &BlufiApplication::onBlufiClientReady);
    connect(m_scanner, &BlufiScanner::clientAllDestroyed, this, &BlufiApplication::onBlufiClientAllDestroyed);
}

void BlufiApplication::start()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);
    
    m_scanner->start();
}

void BlufiApplication::onBlufiClientReady(BlufiClient *client)
{
    BlufiClientFrameCoderAdapter *adapter = new BlufiClientFrameCoderAdapter(client);
    connect(adapter, &BlufiClientFrameCoderAdapter::dataFrameReceived, this, &BlufiApplication::onDataFrameReceived);
    connect(adapter, &BlufiClientFrameCoderAdapter::controlFrameReceived, this, &BlufiApplication::onControlFrameReceived);
    connect(adapter, &BlufiClientFrameCoderAdapter::connectionStatusReceived, this, &BlufiApplication::onConnectionStatusReceived);

    adapter->frameCoder()->sendStaSsid(m_ssid);
    adapter->frameCoder()->sendStaPassword(m_psk);
    adapter->frameCoder()->sendStaConnectionRequest();
}

void BlufiApplication::onDataFrameReceived(BlufiClient *client, BlufiFrameCoder *frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data.toHex(' ');
}

void BlufiApplication::onControlFrameReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data.toHex(' ');
}

void BlufiApplication::onBlufiClientAllDestroyed()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    deleteLater();
}

void BlufiApplication::onConnectionStatusReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone)
{
    qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << connectionStatus;

    if (connectionStatus == BlufiFrameCoder::STATUS_CONNECTED) {
        client->controller()->disconnectFromDevice();
    }
}
