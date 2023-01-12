#include "blufiapplication.h"
#include "bluficlientframecoderadapter.h"
#include <QTimer>

BlufiApplication::BlufiApplication(QObject *parent)
    : QObject{parent}
{
    m_scanner = new BlufiScanner(this);
    m_scanner->setAddressFilter(m_addressFilter);
    m_scanner->setNameFilter(m_nameFilter);

    connect(m_scanner, &BlufiScanner::blufiClientReady, this, &BlufiApplication::onBlufiClientReady);
    connect(m_scanner, &BlufiScanner::clientAllDestroyed, this, &BlufiApplication::onBlufiClientAllDestroyed);
}

void BlufiApplication::parseDataFrame(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArrayView& data, bool toPhone)
{
    switch (type) {
        case BlufiFrameCoder::DATA_NEGOTIATION: {
            
            break;
        }
        case BlufiFrameCoder::DATA_STA_BSSID: {
            
            break;
        }
        case BlufiFrameCoder::DATA_STA_SSID: {
            
            break;
        }
        case BlufiFrameCoder::DATA_STA_PASSWORD: {
            
            break;
        }
        case BlufiFrameCoder::DATA_AP_SSID: {
            
            break;
        }
        case BlufiFrameCoder::DATA_AP_PASSWORD: {
            
            break;
        }
        case BlufiFrameCoder::DATA_AP_MAX_CONNECTIONS: {
            
            break;
        }
        case BlufiFrameCoder::DATA_AP_AUTH_MODE: {
            
            break;
        }
        case BlufiFrameCoder::DATA_AP_CHANNELS: {
            
            break;
        }
        case BlufiFrameCoder::DATA_USERNAME: {
            
            break;
        }
        case BlufiFrameCoder::DATA_CA_CERT: {
            
            break;
        }
        case BlufiFrameCoder::DATA_CLIENT_CERT: {
            
            break;
        }
        case BlufiFrameCoder::DATA_SERVER_CERT: {
            
            break;
        }
        case BlufiFrameCoder::DATA_CLIENT_KEY: {
            
            break;
        }
        case BlufiFrameCoder::DATA_SERVER_KEY: {
            
            break;
        }
        case BlufiFrameCoder::DATA_STATUS: {
            
            break;
        }
        case BlufiFrameCoder::DATA_VERSION: {
            
            break;
        }
        case BlufiFrameCoder::DATA_WIFI_LIST: {
            
            break;
        }
        case BlufiFrameCoder::DATA_ERROR: {
            
            break;
        }
        case BlufiFrameCoder::DATA_CUSTOM: {
            
            break;
        }
        case BlufiFrameCoder::DATA_MAX_RECONNECT_TIME: {
            
            break;
        }
        case BlufiFrameCoder::DATA_CONNECTION_END_REASON: {
            
            break;
        }
        case BlufiFrameCoder::DATA_RSSI_AT_CONNECTION_END: {
            
            break;
        }
        default:
            break;
    }
}

void BlufiApplication::parseControlFrame(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArrayView& data, bool toPhone)
{
    switch (type) {
        case BlufiFrameCoder::CONTROL_ACK: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_SECURITY_MODE_SET: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_OPMODE_SET: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_STA_CONNECT: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_STA_DISCONNECT: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_STATUS_GET: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_AP_DISCONNECT: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_VERSION_GET: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_GATT_DISCONNECT: {
            
            break;
        }
        case BlufiFrameCoder::CONTROL_WIFI_LIST_GET: {
            
            break;
        }
        default:
            break;
    }
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

    adapter->frameCoder()->sendStaSsid(m_ssid);
    adapter->frameCoder()->sendStaPassword(m_psk);
    adapter->frameCoder()->sendStaConnectionRequest();

    QTimer *timer = new QTimer(adapter);
    connect(timer, &QTimer::timeout, adapter->frameCoder(), &BlufiFrameCoder::sendWifiStatusQueryRequest);
    connect(client->controller(), &QLowEnergyController::disconnected, timer, &QTimer::stop);
    timer->setInterval(1000);
    timer->start();
}

void BlufiApplication::onDataFrameReceived(BlufiClient *client, BlufiFrameCoder *frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data.toHex(' ');

    parseDataFrame(client, frameCoder, type, data, toPhone);
}

void BlufiApplication::onControlFrameReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data.toHex(' ');

    parseControlFrame(client, frameCoder, type, data, toPhone);
}

void BlufiApplication::onBlufiClientAllDestroyed()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    emit finished();
}
