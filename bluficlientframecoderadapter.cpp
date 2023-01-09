#include "bluficlientframecoderadapter.h"

BlufiClientFrameCoderAdapter::BlufiClientFrameCoderAdapter(BlufiClient *client)
    : QObject{client}
{
    m_client = client;
    m_frameCoder = new BlufiFrameCoder(this);
    connect(m_frameCoder, &BlufiFrameCoder::encodedDataGenerated, this, &BlufiClientFrameCoderAdapter::onDataSent);
    connect(m_client, &BlufiClient::dataReceived, this, &BlufiClientFrameCoderAdapter::onDataReceived);
}

void BlufiClientFrameCoderAdapter::onDataSent(const QByteArray &data, bool toPhone)
{
    qDebug() << m_client->controller()->remoteName() << m_client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << data.toHex(' ');

    m_client->send(data);
}

void BlufiClientFrameCoderAdapter::onDataReceived(const QByteArray &data)
{
    qDebug() << m_client->controller()->remoteName() << m_client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << data.toHex(' ');

    m_frameCoder->parseReceivedData(data, true);
}
