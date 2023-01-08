#include "blufiapplication.h"

BlufiApplication::BlufiApplication(QObject *parent)
    : QObject{parent}
{
    m_scanner = new BlufiScanner(this);
    connect(m_scanner, &BlufiScanner::blufiClientReady, this, &BlufiApplication::onBlufiClientReady);
}

void BlufiApplication::start()
{
    m_scanner->start();
}

void BlufiApplication::onBlufiClientReady(BlufiClient *client)
{
    BlufiFrameCoder *coder = new BlufiFrameCoder(client);
    connect(coder, &BlufiFrameCoder::encodedDataGenerated, this, [client](const QByteArrayView &data, bool toPhone) {
        client->send(data);
    });
    connect(client, &BlufiClient::dataReceived, this, [coder](const QByteArrayView &data) {
        coder->parseReceivedData(data, true);
    });
}
