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
    void finished();

    void blufiClientReady(BlufiClient *client);

private slots:
    void onDeviceDiscovered(const QBluetoothDeviceInfo &info);

    void onBlufiClientReady();

    void onBlufiServiceNotFound();

    void onScanFinished();

private:
    bool m_deepScan = false;

    QBluetoothDeviceDiscoveryAgent *m_discoveryAgent = nullptr;

    QRegularExpression *m_nameFilter = nullptr;

    QRegularExpression *m_addressFilter = nullptr;

public slots:
    void start();

    void setNameFilter(const QString &nameFilter);

    void setAddressFilter(const QString &addressFilter);

    inline void enableDeepScan(bool deepScan) { m_deepScan = deepScan; }

    inline bool isDeepScan() { return m_deepScan; }
};

#endif // BLUFISCANNER_H
