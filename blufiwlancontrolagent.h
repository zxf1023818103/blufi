#ifndef BLUFIWLANCONTROLAGENT_H
#define BLUFIWLANCONTROLAGENT_H

#include <QObject>

class BlufiWlanControlAgent : public QObject
{
    Q_OBJECT
public:
    explicit BlufiWlanControlAgent(QObject *parent = nullptr);

public slots:
    void connectToAp(const QString &ssid, const QString &psk);

    void connectToEnterpriseAp(const QString &ssid, const QString &identity, const QString &password);

    void disconnectFromAp();

signals:

};

#endif // BLUFIWLANCONTROLAGENT_H
