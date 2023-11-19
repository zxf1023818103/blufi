#ifndef BLUFISERVERFRAMECODERADAPTER_H
#define BLUFISERVERFRAMECODERADAPTER_H

#include <QObject>
#include "blufiserver.h"
#include "blufiframecoder.h"

class BlufiServerFrameCoderAdapter : public QObject
{
    Q_OBJECT

public:
    explicit BlufiServerFrameCoderAdapter(BlufiServer *server);

    inline BlufiFrameCoder * frameCoder() { return m_frameCoder; }

private:
    BlufiServer *m_server;

    BlufiFrameCoder *m_frameCoder;

public slots:
    void onDataSent(const QByteArray &data, bool toPhone);

    void onDataReceived(const QByteArray &data);

private slots:
    void onFrameCoderDataFrameReceived(BlufiFrameCoder::DataFrameTypes type, const QByteArray& data, bool toPhone);

    void onFrameCoderControlFrameReceived(BlufiFrameCoder::ControlFrameTypes type, const QByteArray& data, bool toPhone);

    void onConnectionStatusReceived(BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone);

signals:
    void dataFrameReceived(BlufiServer* server, BlufiFrameCoder* frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArray& data, bool toPhone);

    void controlFrameReceived(BlufiServer* server, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArray& data, bool toPhone);

    void connectionStatusReceived(BlufiServer* server, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone);
};

#endif // BLUFISERVERFRAMECODERADAPTER_H
