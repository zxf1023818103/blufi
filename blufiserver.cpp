#include "blufiserver.h"
#include <QLowEnergyServiceData>

BlufiServer::BlufiServer(QObject *parent)
    : QObject{parent}
{
    m_controller = QLowEnergyController::createPeripheral(this);
    connect(m_controller, &QLowEnergyController::connected, this, &BlufiServer::onConnected);
    connect(m_controller, &QLowEnergyController::disconnected, this, &BlufiServer::onDisconnected);
    connect(m_controller, &QLowEnergyController::connectionUpdated, this, &BlufiServer::onConnectionUpdated);
    connect(m_controller, &QLowEnergyController::discoveryFinished, this, &BlufiServer::onServiceDiscoveryFinished);
    connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BlufiServer::onServiceDiscovered);
    connect(m_controller, &QLowEnergyController::errorOccurred, this, &BlufiServer::onControllerErrorOccurred);
    connect(m_controller, &QLowEnergyController::mtuChanged, this, &BlufiServer::onMtuChanged);
    connect(m_controller, &QLowEnergyController::stateChanged, this, &BlufiServer::onControllerStateChanged);

//    m_service = m_controller->addService(QLowEnergyServiceData().setUuid(QBluetoothUuid::createUuid()))
}

void BlufiServer::onConnected()
{

}

void BlufiServer::onDisconnected()
{

}

void BlufiServer::onConnectionUpdated(const QLowEnergyConnectionParameters &parameters)
{

}

void BlufiServer::onServiceDiscoveryFinished()
{

}

void BlufiServer::onServiceDiscovered(const QBluetoothUuid &newService)
{

}

void BlufiServer::onControllerErrorOccurred(QLowEnergyController::Error newError)
{

}

void BlufiServer::onMtuChanged(int mtu)
{

}

void BlufiServer::onControllerStateChanged(QLowEnergyController::ControllerState state)
{

}

void BlufiServer::onServiceStateChanged(QLowEnergyService::ServiceState newState)
{

}

void BlufiServer::onCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value)
{

}

void BlufiServer::onCharacteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value)
{

}

void BlufiServer::onCharacteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value)
{

}

void BlufiServer::onDescriptorRead(const QLowEnergyDescriptor &info, const QByteArray &value)
{

}

void BlufiServer::onDescriptorWritten(const QLowEnergyDescriptor &info, const QByteArray &value)
{

}

void BlufiServer::onServiceErrorOccurred(QLowEnergyService::ServiceError error)
{

}
