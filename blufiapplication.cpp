#include "blufiapplication.h"

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
    BlufiFrameCoder *coder = new BlufiFrameCoder(client);

    connect(coder, &BlufiFrameCoder::encodedDataGenerated, this, [this, client](const QByteArray &data, bool toPhone) {

        qDebug() << client->controller()->remoteName() << client->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
                << "send" << QByteArray().toHex();

        client->send(data);
    });
    
    connect(client, &BlufiClient::dataReceived, this, [coder](const QByteArray &data) {
        coder->parseReceivedData(data, true);
    });

    connect(coder, &BlufiFrameCoder::dataFrameReceived, this, &BlufiApplication::onDataFrameReceived);

    connect(coder, &BlufiFrameCoder::controlFrameReceived, this, &BlufiApplication::onControlFrameReceived);

    coder->sendStaSsid(m_ssid, false);
    coder->sendStaPassword(m_psk, false);
    coder->sendStaConnectionRequest(false);
}

void BlufiApplication::onDataFrameReceived(BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << qobject_cast<BlufiClient*>(sender()->parent())->controller()->remoteName() << qobject_cast<BlufiClient*>(sender()->parent())->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data;
}

void BlufiApplication::onControlFrameReceived(BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone)
{
    qDebug() << qobject_cast<BlufiClient*>(sender()->parent())->controller()->remoteName() << qobject_cast<BlufiClient*>(sender()->parent())->controller()->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__) << type << data;
}

void BlufiApplication::onBlufiClientAllDestroyed()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    emit finished();
}
