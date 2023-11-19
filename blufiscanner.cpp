#include "blufiscanner.h"
#include "blufiframecoder.h"
#include <QBluetoothPermission>
#include <QCoreApplication>
#include <QBluetoothLocalDevice>

BlufiScanner::BlufiScanner(QObject *parent)
    : QObject{parent}
{

}

void BlufiScanner::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__) << info.name() << info.address() << info.deviceUuid();

    bool blufiServiceFound = false;

    auto serviceUuids = info.serviceUuids();
    for (auto i = serviceUuids.constBegin(); i != serviceUuids.constEnd(); i++) {
        bool ok;
        quint16 uuid = i->toUInt16(&ok);
        if (ok && uuid == BlufiFrameCoder::BLUFI_SERVICE_UUID) {
            blufiServiceFound = true;
            break;
        }
    }

    if (blufiServiceFound || m_deepScanEnabled) {
        BlufiClient *client = new BlufiClient(info, this);
        m_globalCount++;

        connect(client, &BlufiClient::ready, this, &BlufiScanner::onBlufiClientReady);
        connect(client, &BlufiClient::destroyed, this, &BlufiScanner::onBlufiClientDestroyed);
        
        client->setAddressFilter(m_addressFilter);
        client->setNameFilter(m_nameFilter);

        client->connectToDevice();
    }
}

void BlufiScanner::onBlufiClientReady()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    m_discoveryAgent->stop();

    emit blufiClientReady(qobject_cast<BlufiClient*>(sender()));
}

void BlufiScanner::onBlufiServiceNotFound()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    sender()->deleteLater();
}

void BlufiScanner::onScanFinished()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    if (m_globalCount == 0) {
        emit clientAllDestroyed();
    }
}

void BlufiScanner::start()
{
    if (!m_init) {
        QBluetoothPermission permission {};
        permission.setCommunicationModes(QBluetoothPermission::Default);

        switch (qApp->checkPermission(permission)) {
            case Qt::PermissionStatus::Undetermined: {
                qApp->requestPermission(permission, this, &BlufiScanner::start);
                return;
            }

            case Qt::PermissionStatus::Denied: {
                qApp->exit(-1);
                return;
            }

            case Qt::PermissionStatus::Granted: {
                break;
            }
        }

#ifndef Q_OS_IOS
        auto devices = QBluetoothLocalDevice::allDevices();
        if (devices.empty()) {
            qApp->exit(-1);
        }
        m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(devices.back().address(), this);
#else
        m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
#endif // Q_OS_IOS

        connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BlufiScanner::onDeviceDiscovered);
        connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BlufiScanner::onScanFinished);
        connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BlufiScanner::onScanFinished);

        m_init = true;
    }
    
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BlufiScanner::setNameFilter(const QString &nameFilter)
{
    if (!nameFilter.isEmpty()) {
        m_nameFilter = QRegularExpression(nameFilter);
    }
}

void BlufiScanner::setAddressFilter(const QString &addressFilter)
{
    if (!addressFilter.isEmpty()) {
        m_addressFilter = QRegularExpression(addressFilter, QRegularExpression::CaseInsensitiveOption);
    }
}

void BlufiScanner::onBlufiClientDestroyed()
{
    m_globalCount--;

    qDebug() << (QString(metaObject()->className()) + "::" + __func__) << m_globalCount;

    if (!m_discoveryAgent->isActive() && m_globalCount == 0) {
        emit clientAllDestroyed();
    }
}
