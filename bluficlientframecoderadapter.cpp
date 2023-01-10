#include "bluficlientframecoderadapter.h"

BlufiClientFrameCoderAdapter::BlufiClientFrameCoderAdapter(BlufiClient *client)
    : QObject{client}
{
    m_client = client;
    m_frameCoder = new BlufiFrameCoder(this);
    connect(m_frameCoder, &BlufiFrameCoder::encodedDataGenerated, this, &BlufiClientFrameCoderAdapter::onDataSent);
    connect(m_frameCoder, &BlufiFrameCoder::dataFrameReceived, this, &BlufiClientFrameCoderAdapter::onFrameCoderDataFrameReceived);
    connect(m_frameCoder, &BlufiFrameCoder::controlFrameReceived, this, &BlufiClientFrameCoderAdapter::onFrameCoderControlFrameReceived);
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

void BlufiClientFrameCoderAdapter::onFrameCoderDataFrameReceived(BlufiFrameCoder::DataFrameTypes type, const QByteArray& data, bool toPhone)
{
    emit dataFrameReceived(m_client, m_frameCoder, type, data, toPhone);
}

void BlufiClientFrameCoderAdapter::onFrameCoderControlFrameReceived(BlufiFrameCoder::ControlFrameTypes type, const QByteArray& data, bool toPhone)
{
    emit controlFrameReceived(m_client, m_frameCoder, type, data, toPhone);
}
