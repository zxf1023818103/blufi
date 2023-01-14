#include "blufiserverapplication.h"

BlufiServerApplication::BlufiServerApplication(QObject *parent)
    : QObject{parent}
{
    m_server = new BlufiServer(this);
    connect(m_server, &BlufiServer::destroyed, this, &BlufiServerApplication::onBlufiServerDestroyed);
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
