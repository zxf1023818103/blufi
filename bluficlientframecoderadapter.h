#ifndef BLUFICLIENTFRAMECODERADAPTER_H
#define BLUFICLIENTFRAMECODERADAPTER_H

#include <QObject>
#include "bluficlient.h"
#include "blufiframecoder.h"

class BlufiClientFrameCoderAdapter : public QObject
{
    Q_OBJECT
    
public:
    explicit BlufiClientFrameCoderAdapter(BlufiClient *client);

    inline BlufiFrameCoder * frameCoder() { return m_frameCoder; }

private:
    BlufiClient *m_client;

    BlufiFrameCoder *m_frameCoder;

public slots:
    void onDataSent(const QByteArray &data, bool toPhone);

    void onDataReceived(const QByteArray &data);

private slots:
    void onFrameCoderDataFrameReceived(BlufiFrameCoder::DataFrameTypes type, const QByteArray& data, bool toPhone);

    void onFrameCoderControlFrameReceived(BlufiFrameCoder::ControlFrameTypes type, const QByteArray& data, bool toPhone);

    void onConnectionStatusReceived(BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone);

signals:
    void dataFrameReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::DataFrameTypes type, const QByteArray& data, bool toPhone);

    void controlFrameReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ControlFrameTypes type, const QByteArray& data, bool toPhone);

    void connectionStatusReceived(BlufiClient* client, BlufiFrameCoder* frameCoder, BlufiFrameCoder::ConnectionStatus connectionStatus, bool toPhone);
};

#endif // BLUFICLIENTFRAMECODERADAPTER_H