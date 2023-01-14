#ifndef BLUFISERVERFRAMECODERADAPTER_H
#define BLUFISERVERFRAMECODERADAPTER_H

#include <QObject>

class BlufiServerFrameCoderAdapter : public QObject
{
    Q_OBJECT
public:
    explicit BlufiServerFrameCoderAdapter(QObject *parent = nullptr);

signals:

};

#endif // BLUFISERVERFRAMECODERADAPTER_H
