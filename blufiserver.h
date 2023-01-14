#ifndef BLUFISERVER_H
#define BLUFISERVER_H

#include <QObject>
#include <QLowEnergyController>

class BlufiServer : public QObject
{
    Q_OBJECT
public:
    explicit BlufiServer(QObject *parent = nullptr);

private:
    QLowEnergyController *m_controller;

    QLowEnergyService *m_service;

private slots:
    void onConnected();

    void onDisconnected();

    void onConnectionUpdated(const QLowEnergyConnectionParameters &parameters);

    void onServiceDiscoveryFinished();

    void onServiceDiscovered(const QBluetoothUuid &newService);

    void onControllerErrorOccurred(QLowEnergyController::Error newError);

    void onMtuChanged(int mtu);

    void onControllerStateChanged(QLowEnergyController::ControllerState state);

    void onServiceStateChanged(QLowEnergyService::ServiceState newState);

    void onCharacteristicChanged(const QLowEnergyCharacteristic &info, const QByteArray &value);

    void onCharacteristicRead(const QLowEnergyCharacteristic &info, const QByteArray &value);

    void onCharacteristicWritten(const QLowEnergyCharacteristic &info, const QByteArray &value);

    void onDescriptorRead(const QLowEnergyDescriptor &info, const QByteArray &value);

    void onDescriptorWritten(const QLowEnergyDescriptor &info, const QByteArray &value);

    void onServiceErrorOccurred(QLowEnergyService::ServiceError error);
};

#endif // BLUFISERVER_H
