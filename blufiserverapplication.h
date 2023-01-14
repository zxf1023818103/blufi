#ifndef BLUFISERVERAPPLICATION_H
#define BLUFISERVERAPPLICATION_H

#include <QObject>
#include "blufiserver.h"

class BlufiServerApplication : public QObject
{
    Q_OBJECT
public:
    explicit BlufiServerApplication(QObject *parent = nullptr);

private:
    BlufiServer *m_server;

public slots:
    void start();

private slots:
    void onBlufiServerDestroyed();
};

#endif // BLUFISERVERAPPLICATION_H
