#include "blufiscanner.h"

BlufiScanner::BlufiScanner(QObject *parent)
    : QObject{parent}
{
    m_discoveryAgent = new QBluetoothDeviceDiscoveryAgent(this);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this, &BlufiScanner::onDeviceDiscovered);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::finished, this, &BlufiScanner::onScanFinished);
    connect(m_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::canceled, this, &BlufiScanner::onScanFinished);
}

void BlufiScanner::onDeviceDiscovered(const QBluetoothDeviceInfo &info)
{
    if (m_nameFilter) {
        if (!m_nameFilter->match(info.name()).hasMatch()) {
            return;
        }
    }

    if (m_addressFilter) {
        if (!m_addressFilter->match(info.address().toString()).hasMatch()) {
            return;
        }
    }

    bool blufiServiceFound = false;

    auto serviceUuids = info.serviceUuids();
    for (auto i = serviceUuids.constBegin(); i != serviceUuids.constEnd(); i++) {
        bool ok;
        quint16 uuid = i->toUInt16(&ok);
        if (ok && uuid == BlufiClient::BLUFI_SERVICE_UUID) {
            blufiServiceFound = true;
            break;
        }
    }

    if (blufiServiceFound || m_deepScan) {
        BlufiClient *client = new BlufiClient(info, this);
        connect(client, &BlufiClient::ready, this, &BlufiScanner::onBlufiClientReady);
        client->connectToDevice();
    }
}

void BlufiScanner::onBlufiClientReady()
{
    qDebug() << __func__;

    emit blufiClientReady(qobject_cast<BlufiClient*>(sender()));
}

void BlufiScanner::onBlufiServiceNotFound()
{
    qDebug() << __func__;

    sender()->deleteLater();
}

void BlufiScanner::onScanFinished()
{
    qDebug() << __func__;

    emit finished();
}

void BlufiScanner::start()
{
    m_discoveryAgent->start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void BlufiScanner::setNameFilter(const QString &nameFilter)
{
    if (m_nameFilter != nullptr) {
        delete m_nameFilter;
        m_nameFilter = nullptr;
    }

    if (!nameFilter.isEmpty()) {
        m_nameFilter = new QRegularExpression(nameFilter);
    }
}

void BlufiScanner::setAddressFilter(const QString &addressFilter)
{
    if (m_addressFilter != nullptr) {
        delete m_addressFilter;
        m_addressFilter = nullptr;
    }

    if (!addressFilter.isEmpty()) {
        m_addressFilter = new QRegularExpression(addressFilter);
    }
}
