#include "blufiserverframecoderadapter.h"

BlufiServerFrameCoderAdapter::BlufiServerFrameCoderAdapter(BlufiServer *server)
    : QObject{server}
{
    m_server = server;
    m_frameCoder = new BlufiFrameCoder(this);
    connect(m_frameCoder, &BlufiFrameCoder::encodedDataGenerated, this, &BlufiServerFrameCoderAdapter::onDataSent);
    connect(m_frameCoder, &BlufiFrameCoder::dataFrameReceived, this, &BlufiServerFrameCoderAdapter::onFrameCoderDataFrameReceived);
    connect(m_frameCoder, &BlufiFrameCoder::controlFrameReceived, this, &BlufiServerFrameCoderAdapter::onFrameCoderControlFrameReceived);
    connect(m_frameCoder, &BlufiFrameCoder::connectionStatusReceived, this, &BlufiServerFrameCoderAdapter::onConnectionStatusReceived);
    connect(m_server, &BlufiServer::dataReceived, this, &BlufiServerFrameCoderAdapter::onDataReceived);
}

void BlufiServerFrameCoderAdapter::onDataSent(const QByteArray &data, bool toPhone)
{
    qDebug() << m_server->controller()->remoteName() << m_server->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << data.toHex(' ');

    m_server->send(data);
}

void BlufiServerFrameCoderAdapter::onDataReceived(const QByteArray &data)
{
    qDebug() << m_server->controller()->remoteName() << m_server->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << data.toHex(' ');

    m_frameCoder->parseReceivedData(data, false);
}

void BlufiServerFrameCoderAdapter::onFrameCoderDataFrameReceived(BlufiFrameCoder::DataFrameTypes type, const QByteArray& data, bool toPhone)
{
    qDebug() << m_server->controller()->remoteName() << m_server->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << type << data.toHex(' ');

    emit dataFrameReceived(m_server, m_frameCoder, type, data, toPhone);
}

void BlufiServerFrameCoderAdapter::onFrameCoderControlFrameReceived(BlufiFrameCoder::ControlFrameTypes type, const QByteArray& data, bool toPhone)
{
    qDebug() << m_server->controller()->remoteName() << m_server->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << type << data.toHex(' ');

    emit controlFrameReceived(m_server, m_frameCoder, type, data, toPhone);
}

void BlufiServerFrameCoderAdapter::onConnectionStatusReceived(BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone)
{
    qDebug() << m_server->controller()->remoteName() << m_server->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << connectionStatus;

    emit connectionStatusReceived(m_server, m_frameCoder, connectionStatus, toPhone);
}
