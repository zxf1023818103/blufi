#include "bluficlient.h"

BlufiClient::BlufiClient(const QBluetoothDeviceInfo& info, QObject *parent)
    : QObject{parent}
{
    m_controller = QLowEnergyController::createCentral(info, this);
    connect(m_controller, &QLowEnergyController::connected, this, &BlufiClient::onConnected);
    connect(m_controller, &QLowEnergyController::disconnected, this, &BlufiClient::onDisconnected);
    connect(m_controller, &QLowEnergyController::connectionUpdated, this, &BlufiClient::onConnectionUpdated);
    connect(m_controller, &QLowEnergyController::discoveryFinished, this, &BlufiClient::onServiceDiscoveryFinished);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BlufiClient::onServiceDiscovered);
    connect(m_controller, &QLowEnergyController::errorOccurred, this, &BlufiClient::onControllerErrorOccurred);
    connect(m_controller, &QLowEnergyController::mtuChanged, this, &BlufiClient::onMtuChanged);
    connect(m_controller, &QLowEnergyController::stateChanged, this, &BlufiClient::onControllerStateChanged);
}

void BlufiClient::onConnected()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);

    bool nameFilterMatched = true;
    if (m_nameFilter.isValid()) {
        nameFilterMatched = m_nameFilter.match(m_controller->remoteName()).hasMatch();
    }

    bool addressFilterMatched = true;
    if (m_addressFilter.isValid()) {
        addressFilterMatched = m_addressFilter.match(m_controller->remoteAddress().toString()).hasMatch();
    }

    if (nameFilterMatched && addressFilterMatched) {
        m_controller->discoverServices();
    }
    else {
        deleteLater();
    }
}

void BlufiClient::onDisconnected()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);
}

void BlufiClient::onConnectionUpdated(const QLowEnergyConnectionParameters &parameters)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << "latency" << parameters.latency()
             << "maximiumInterval" << parameters.maximumInterval()
             << "minimiumInterval" << parameters.minimumInterval()
             << "supervisionTimeout" << parameters.supervisionTimeout();
}

void BlufiClient::onServiceDiscoveryFinished()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);

    if (m_service != nullptr) {
        m_service->discoverDetails();
    }
    else {
        emit serviceNotFound();
    }
}

void BlufiClient::onServiceDiscovered(const QBluetoothUuid &newService)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << newService;

    if (m_service == nullptr) {
        bool ok;
        quint16 uuid = newService.toUInt16(&ok);
        if (ok && uuid == BLUFI_SERVICE_UUID) {
            m_service = m_controller->createServiceObject(newService, this);
            connect(m_service, &QLowEnergyService::stateChanged, this, &BlufiClient::onServiceStateChanged);
            connect(m_service, &QLowEnergyService::characteristicChanged, this, &BlufiClient::onCharacteristicChanged);
            connect(m_service, &QLowEnergyService::characteristicRead, this, &BlufiClient::onCharacteristicRead);
            connect(m_service, &QLowEnergyService::characteristicWritten, this, &BlufiClient::onCharacteristicWritten);
            connect(m_service, &QLowEnergyService::descriptorRead, this, &BlufiClient::onDescriptorRead);
            connect(m_service, &QLowEnergyService::descriptorWritten, this, &BlufiClient::onDescriptorWritten);
            connect(m_service, &QLowEnergyService::errorOccurred, this, &BlufiClient::onServiceErrorOccurred);
        }
    }
}

void BlufiClient::onControllerErrorOccurred(QLowEnergyController::Error newError)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << newError;
}

void BlufiClient::onMtuChanged(int mtu)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << mtu;
}

void BlufiClient::onControllerStateChanged(QLowEnergyController::ControllerState state)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << state;
    
    if (state == QLowEnergyController::UnconnectedState) {
        deleteLater();
    }
}

void BlufiClient::connectToDevice()
{
    m_controller->connectToDevice();
}

void BlufiClient::send(const QByteArray &data)
{
    m_service->writeCharacteristic(m_p2eCharacteristic, QByteArray().append(data), QLowEnergyService::WriteWithResponse);
}

void BlufiClient::onServiceStateChanged(QLowEnergyService::ServiceState newState)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << newState;

    if (newState == QLowEnergyService::RemoteServiceDiscovered) {
        auto characteristics = m_service->characteristics();
        for (auto i = characteristics.constBegin(); i != characteristics.constEnd(); i++) {

            bool ok;
            quint16 uuid = i->uuid().toUInt16(&ok);
            if (uuid == BLUFI_E2P_UUID) {
                m_e2pCharacteristic = *i;
            }
            else if (uuid == BLUFI_P2E_UUID) {
                m_p2eCharacteristic = *i;
            }
        }

        if (m_e2pCharacteristic.isValid() && m_p2eCharacteristic.isValid()) {
            m_service->writeDescriptor(m_e2pCharacteristic.clientCharacteristicConfiguration(), QLowEnergyCharacteristic::CCCDEnableNotification);
        }
        else {
            emit serviceNotFound();
        }
    }
}

void BlufiClient::onCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');

    if (info == m_e2pCharacteristic) {
        emit dataReceived(value);
    }
}

void BlufiClient::onCharacteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
}

void BlufiClient::onCharacteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');

    if (info == m_e2pCharacteristic) {
        emit dataReceived(value);
    }
}

void BlufiClient::onDescriptorRead(const QLowEnergyDescriptor &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
}

void BlufiClient::onDescriptorWritten(const QLowEnergyDescriptor &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
    
    if (info == m_e2pCharacteristic.clientCharacteristicConfiguration() && value == QLowEnergyCharacteristic::CCCDEnableNotification) {
        emit ready();
    }
}

void BlufiClient::onServiceErrorOccurred(QLowEnergyService::ServiceError error)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << error;
}
