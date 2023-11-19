#include "blufiserverapplication.h"
#include "blufiframecoder.h"
#include "blufiserverframecoderadapter.h"

BlufiServerApplication::BlufiServerApplication(QObject *parent)
    : QObject{parent}
{
    m_server = new BlufiServer(this);
    connect(m_server, &BlufiServer::destroyed, this, &BlufiServerApplication::onBlufiServerDestroyed);

    m_adapter = new BlufiServerFrameCoderAdapter(m_server);
    connect(m_adapter, &BlufiServerFrameCoderAdapter::dataFrameReceived, this, &BlufiServerApplication::onDataFrameReceived);
    connect(m_adapter, &BlufiServerFrameCoderAdapter::controlFrameReceived, this, &BlufiServerApplication::onControlFrameReceived);
    connect(m_adapter, &BlufiServerFrameCoderAdapter::connectionStatusReceived, this, &BlufiServerApplication::onConnectionStatusReceived);
}

void BlufiServerApplication::start()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);
    
    m_server->start();
}

void BlufiServerApplication::onBlufiServerDestroyed()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);
    
    deleteLater();
}

void BlufiServerApplication::onDataFrameReceived(BlufiServer *server, BlufiFrameCoder *frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << server->controller()->remoteName() << server->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data.toHex(' ');

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
    }
}

void BlufiServerApplication::onControlFrameReceived(BlufiServer* server, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << server->controller()->remoteName() << server->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data.toHex(' ');

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
            m_adapter->frameCoder()->sendBlufiVersion(1, 3);
            break;
        }

        case BlufiFrameCoder::CONTROL_GATT_DISCONNECT: {
            m_server->controller()->disconnectFromDevice();
            break;
        }

        case BlufiFrameCoder::CONTROL_WIFI_LIST_GET: {

            break;
        }
    }
}

void BlufiServerApplication::onConnectionStatusReceived(BlufiServer* server, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone)
{
    qDebug() << server->controller()->remoteName() << server->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << connectionStatus;

    if (connectionStatus == BlufiFrameCoder::STATUS_CONNECTED) {
        server->controller()->disconnectFromDevice();
    }
}
