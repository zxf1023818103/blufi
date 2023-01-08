#ifndef BLUFICLIENT_H
#define BLUFICLIENT_H

#include <QObject>
#include <QLowEnergyController>

class BlufiClient : public QObject
{
    Q_OBJECT
public:
    static const quint16 BLUFI_SERVICE_UUID = 0xFFFF;

    static const quint16 BLUFI_P2E_UUID = 0xFF01;

    static const quint16 BLUFI_E2P_UUID = 0xFF02;

    explicit BlufiClient(const QBluetoothDeviceInfo& info, QObject *parent = nullptr);

private:
    QLowEnergyController *m_controller = nullptr;

    QLowEnergyService *m_service = nullptr;

    QLowEnergyCharacteristic m_p2eCharacteristic;

    QLowEnergyCharacteristic m_e2pCharacteristic;

signals:
    void ready();

    void serviceNotFound();

    void dataReceived(const QByteArrayView& data);

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

    void onDestroyed(QObject *obj);

public slots:
    void connectToDevice();

    void send(const QByteArrayView &data);
};

#endif // BLUFICLIENT_H
