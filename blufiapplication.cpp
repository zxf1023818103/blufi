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

void BlufiApplication::start()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);
    
    m_scanner->start();
}

void BlufiApplication::onBlufiClientReady(BlufiClient *client)
{
    BlufiClientFrameCoderAdapter *adapter = new BlufiClientFrameCoderAdapter(client);
    connect(adapter->frameCoder(), &BlufiFrameCoder::dataFrameReceived, this, &BlufiApplication::onDataFrameReceived);
    connect(adapter->frameCoder(), &BlufiFrameCoder::controlFrameReceived, this, &BlufiApplication::onControlFrameReceived);

    adapter->frameCoder()->sendStaSsid(m_ssid, false);
    adapter->frameCoder()->sendStaPassword(m_psk, false);
    adapter->frameCoder()->sendStaConnectionRequest(false);

    QTimer *timer = new QTimer(adapter->frameCoder());
    connect(timer, &QTimer::timeout, adapter->frameCoder(), &BlufiFrameCoder::sendWifiStatusQueryRequest);
    timer->setInterval(1000);
    timer->start();
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
