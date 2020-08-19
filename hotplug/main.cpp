#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusReply>
#include <QDebug>


int main()
{
    //构造一个method_call消息，服务名称为：com.deepin.diskmanager，对象路径为：/test/objects
    //接口名称为com.scorpio.test.value，method名称为minValue
    //通过QDBusMessage访问Service
    QDBusMessage message = QDBusMessage::createMethodCall("com.deepin.diskmanager",
                           "/com/deepin/diskmanager",
                           "com.deepin.diskmanager",
                           "updateUsb");
    //发送消息
    QDBusMessage response = QDBusConnection::systemBus().call(message);

    return 0;
}
