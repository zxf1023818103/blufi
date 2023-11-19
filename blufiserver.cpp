#include "blufiserver.h"
#include "blufiframecoder.h"
#include <QLowEnergyServiceData>
#include <QLowEnergyCharacteristicData>
#include <QLowEnergyDescriptorData>
#include <QLowEnergyAdvertisingParameters>
#include <QCoreApplication>
#include <QBluetoothPermission>
#include <QBluetoothLocalDevice>
#include <QTimer>

BlufiServer::BlufiServer(QObject *parent)
    : QObject{parent}
{
    
}

void BlufiServer::start()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    if (!m_init) {
        QBluetoothPermission permission {};
        permission.setCommunicationModes(QBluetoothPermission::Default);

        switch (qApp->checkPermission(permission)) {
            case Qt::PermissionStatus::Undetermined: {
                qApp->requestPermission(permission, this, &BlufiServer::start);
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
        m_controller = QLowEnergyController::createPeripheral(devices.back().address());
#else
        m_controller = QLowEnergyController::createPeripheral();
#endif // Q_OS_IOS

        m_controller = QLowEnergyController::createPeripheral(this);
        connect(m_controller, &QLowEnergyController::connected, this, &BlufiServer::onConnected);
        connect(m_controller, &QLowEnergyController::disconnected, this, &BlufiServer::onDisconnected);
        connect(m_controller, &QLowEnergyController::connectionUpdated, this, &BlufiServer::onConnectionUpdated);
        connect(m_controller, &QLowEnergyController::discoveryFinished, this, &BlufiServer::onServiceDiscoveryFinished);
        connect(m_controller, &QLowEnergyController::serviceDiscovered, this, &BlufiServer::onServiceDiscovered);
        connect(m_controller, &QLowEnergyController::errorOccurred, this, &BlufiServer::onControllerErrorOccurred);
        connect(m_controller, &QLowEnergyController::mtuChanged, this, &BlufiServer::onMtuChanged);
        connect(m_controller, &QLowEnergyController::stateChanged, this, &BlufiServer::onControllerStateChanged);

        QLowEnergyServiceData serviceData;
        serviceData.setUuid(QBluetoothUuid(BlufiFrameCoder::BLUFI_SERVICE_UUID));
        serviceData.setType(QLowEnergyServiceData::ServiceTypePrimary);

        QLowEnergyDescriptorData e2pCccdData(QBluetoothUuid::DescriptorType::ClientCharacteristicConfiguration,
                                             QLowEnergyCharacteristic::CCCDDisable);

        QLowEnergyDescriptorData e2pExtendData(
            QBluetoothUuid::DescriptorType::CharacteristicExtendedProperties,
            // From bluetooth specs: length 2 bytes
            // bit 0: reliable write, bit 1: writable auxiliaries
            QByteArray::fromHex("0300"));

        QLowEnergyCharacteristicData e2p;
        e2p.setUuid(QBluetoothUuid(BlufiFrameCoder::BLUFI_E2P_UUID));
        e2p.setProperties(QLowEnergyCharacteristic::Notify);
        e2p.addDescriptor(e2pCccdData);
        e2p.addDescriptor(e2pExtendData);
        serviceData.addCharacteristic(e2p);

        QLowEnergyCharacteristicData p2e;
        p2e.setUuid(QBluetoothUuid(BlufiFrameCoder::BLUFI_P2E_UUID));
        p2e.setProperties(QLowEnergyCharacteristic::WriteNoResponse | QLowEnergyCharacteristic::Write);
        serviceData.addCharacteristic(p2e);

        m_service = m_controller->addService(serviceData);
        connect(m_service, &QLowEnergyService::characteristicChanged, this, &BlufiServer::onCharacteristicChanged);
        connect(m_service, &QLowEnergyService::characteristicRead, this, &BlufiServer::onCharacteristicRead);
        connect(m_service, &QLowEnergyService::characteristicWritten, this, &BlufiServer::onCharacteristicWritten);
        connect(m_service, &QLowEnergyService::errorOccurred, this, &BlufiServer::onServiceErrorOccurred);
        connect(m_service, &QLowEnergyService::descriptorRead, this, &BlufiServer::onDescriptorRead);
        connect(m_service, &QLowEnergyService::descriptorWritten, this, &BlufiServer::onDescriptorWritten);

        m_init = true;
    }

    QLowEnergyAdvertisingParameters parameters;
    parameters.setMode(QLowEnergyAdvertisingParameters::AdvInd);
    
    QLowEnergyAdvertisingData data;
    data.setDiscoverability(QLowEnergyAdvertisingData::DiscoverabilityGeneral);
    data.setServices({QBluetoothUuid(BlufiFrameCoder::BLUFI_SERVICE_UUID)});
    data.setIncludePowerLevel(true);
    data.setLocalName(m_name);

    m_controller->startAdvertising(parameters, data);
}

void BlufiServer::stop()
{
    qDebug() << (QString(metaObject()->className()) + "::" + __func__);

    m_controller->stopAdvertising();
}

void BlufiServer::send(const QByteArray &data)
{
    m_service->writeCharacteristic(m_service->characteristic(QBluetoothUuid(BlufiFrameCoder::BLUFI_E2P_UUID)), QByteArray().append(data), QLowEnergyService::WriteWithResponse);
}

void BlufiServer::onConnected()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);
}

void BlufiServer::onDisconnected()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);
}

void BlufiServer::onConnectionUpdated(const QLowEnergyConnectionParameters &parameters)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << "latency" << parameters.latency()
             << "maximiumInterval" << parameters.maximumInterval()
             << "minimiumInterval" << parameters.minimumInterval()
             << "supervisionTimeout" << parameters.supervisionTimeout();
}

void BlufiServer::onServiceDiscoveryFinished()
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__);
}

void BlufiServer::onServiceDiscovered(const QBluetoothUuid &newService)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << newService;
}

void BlufiServer::onControllerErrorOccurred(QLowEnergyController::Error newError)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << newError;

    if (newError != QLowEnergyController::NoError) {
        deleteLater();
    }
}

void BlufiServer::onMtuChanged(int mtu)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << mtu;
}

void BlufiServer::onControllerStateChanged(QLowEnergyController::ControllerState state)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << state;
}

void BlufiServer::onServiceStateChanged(QLowEnergyService::ServiceState newState)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << newState;
}

void BlufiServer::onCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
    if (info.uuid().toUInt16() == BlufiFrameCoder::BLUFI_P2E_UUID) {
        emit dataReceived(value);
    }
}

void BlufiServer::onCharacteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
}

void BlufiServer::onCharacteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
}

void BlufiServer::onDescriptorRead(const QLowEnergyDescriptor &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
}

void BlufiServer::onDescriptorWritten(const QLowEnergyDescriptor &info, const QByteArray &value)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << info.uuid() << value.toHex(' ');
}

void BlufiServer::onServiceErrorOccurred(QLowEnergyService::ServiceError error)
{
    qDebug() << m_controller->remoteName() << m_controller->remoteAddress() << (QString(metaObject()->className()) + "::" + __func__)
             << error;
}
