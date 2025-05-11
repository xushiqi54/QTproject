#ifndef MY_MQTT_CLIENT_H
#define MY_MQTT_CLIENT_H
#define MQTT_AUTO_TOPIC "test"
#include <QObject>
#include <QDateTime>
#include <QtMqtt/qmqttclient.h>
#include <QJsonDocument>
#include <QJsonObject>

using namespace std;

class my_mqtt_client : public QObject
{
    Q_OBJECT
public:
    my_mqtt_client();
    ~my_mqtt_client(){};

    QMqttClient* m_client = nullptr;

    void MyMQTTSubscribe(QString);
    void MyMQTTSendMessage(const QString, const QString);


signals:
    void sendDataToSQL(QString Ip,QString name,QString T);

public slots:
    void setHost(const QString s){m_client->setHostname(s);}
    void setport(const QString p){m_client->setPort(static_cast<quint16>(p.toUInt()));}
    void brokerConnected();
    void updateLogStateChange();
    void brokerDisconnected();
    void receiveMess(const QByteArray &, const QMqttTopicName &);

private:
    QString Host;
    QString Port;

};

#endif // MY_MQTT_CLIENT_H
