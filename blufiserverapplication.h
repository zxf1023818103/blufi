#ifndef BLUFISERVERAPPLICATION_H
#define BLUFISERVERAPPLICATION_H

#include <QObject>

class BlufiServerApplication : public QObject
{
    Q_OBJECT
public:
    explicit BlufiServerApplication(QObject *parent = nullptr);

public slots:
    void start();
};

#endif // BLUFISERVERAPPLICATION_H
