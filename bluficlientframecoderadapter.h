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

signals:
    void finished();
};

#endif // BLUFICLIENTFRAMECODERADAPTER_H