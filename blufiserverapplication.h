#ifndef BLUFISERVERAPPLICATION_H
#define BLUFISERVERAPPLICATION_H

#include <QObject>
#include "blufiserver.h"
#include "blufiframecoder.h"
#include "blufiserverframecoderadapter.h"

class BlufiServerApplication : public QObject
{
    Q_OBJECT
public:
    explicit BlufiServerApplication(QObject *parent = nullptr);

private:
    BlufiServer *m_server;

    BlufiServerFrameCoderAdapter *m_adapter;

public slots:
    void start();

private slots:
    void onBlufiServerDestroyed();

    void onDataFrameReceived(BlufiServer* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArray &data, bool toPhone);

    void onControlFrameReceived(BlufiServer* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArray &data, bool toPhone);

    void onConnectionStatusReceived(BlufiServer* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone);
};

#endif // BLUFISERVERAPPLICATION_H
