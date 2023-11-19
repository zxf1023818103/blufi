#ifndef BLUFISCANNER_H
#define BLUFISCANNER_H

#include <QObject>
#include <QBluetoothDeviceDiscoveryAgent>
#include <QRegularExpression>
#include "bluficlient.h"

class BlufiScanner : public QObject
{
    Q_OBJECT
public:
    explicit BlufiScanner(QObject *parent = nullptr);

signals:
    void clientAllDestroyed();

    void blufiClientReady(BlufiClient *client);

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);

    void onBlufiClientReady();

    void onBlufiServiceNotFound();

    void onScanFinished();

    void onBlufiClientDestroyed();

private:
    bool m_deepScanEnabled = false;

    bool m_stopDiscoveryAtClientReadyEnabled = false;

    bool m_init = false;

    QAtomicInt m_globalCount;

    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;

    QRegularExpression m_nameFilter;

    QRegularExpression m_addressFilter;

public slots:
    void start();

    void setNameFilter(const QString &nameFilter);

    void setAddressFilter(const QString &addressFilter);

    inline void enableDeepScan(bool deepScanEnabled) { m_deepScanEnabled = deepScanEnabled; }

    inline void enableStopDiscoveryAtClientReady(bool stopDiscoveryAtClientReadyEnabled) { m_stopDiscoveryAtClientReadyEnabled = stopDiscoveryAtClientReadyEnabled; }

    inline bool deepScanEnabled() { return m_deepScanEnabled; }
};

#endif // BLUFISCANNER_H
