#ifndef DMDBUSHANDLER_H
#define DMDBUSHANDLER_H

#include <QObject>
#include <QDebug>
#include <QDBusConnection>

class  DMDBusInterface;
class  DeviceInfo;

class DMDbusHandler : public QObject
{
    Q_OBJECT
public:
    static DMDbusHandler *instance(QObject *parent = nullptr);
private:
    explicit DMDbusHandler(QObject *parent = nullptr);

signals:

public slots:
    void Quit();
    void Start();
    void getDeviceinfo();
    void MessageReport(const QString &msg);

private:
    DMDBusInterface        *m_dbus = nullptr;
    static DMDbusHandler *m_statichandeler;
};

#endif // DMDBUSHANDLER_H
