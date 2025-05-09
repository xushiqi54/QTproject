#include "my_mqtt_client.h"
#include"widget.h"

my_mqtt_client::my_mqtt_client()
{
    m_client = new QMqttClient(this);
    m_client->setHostname(Host);
    m_client->setPort(Port.toUInt());
    m_client->connectToHost();

    connect(m_client, &QMqttClient::connected, this, &my_mqtt_client::brokerConnected);
    connect(m_client, &QMqttClient::stateChanged, this, &my_mqtt_client::updateLogStateChange);
    connect(m_client, &QMqttClient::disconnected, this, &my_mqtt_client::brokerDisconnected);

    connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
            const QString content = QDateTime::currentDateTime().toString()
                        + " PingResponse\n ";
            qDebug() << content;
        });
}

void my_mqtt_client::MyMQTTSubscribe(QString str)
{
    auto subscription = m_client->subscribe(str, 0);
    if (!subscription) {
        qDebug() << "Could not subscribe. Is there a valid connection?";
        return;
    }
}

void my_mqtt_client::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
                    + QLatin1String(": State Change")
                    + QString::number(m_client->state())
                    + QLatin1Char('\n');
    qDebug() << content;
}

void my_mqtt_client::brokerConnected()
{
    qDebug() << "Connected!";
    if(m_client->state() == QMqttClient::Connected){
        m_client->subscribe(QString("test"));
        connect(m_client, &QMqttClient::messageReceived, this, &my_mqtt_client::receiveMess);
    }
}

void my_mqtt_client::brokerDisconnected()
{
    qDebug() << "Disconnected!";
}

void my_mqtt_client::receiveMess(const QByteArray &message, const QMqttTopicName &topic)
{
   /*QString content;
   content = QDateTime::currentDateTime().toString() + QLatin1Char('\n');
   content += QLatin1String(" Received Topic: ") + topic.name() + QLatin1Char('\n');
   content += QLatin1String(" Message: ") + message + QLatin1Char('\n');
   */
   const QString content = QDateTime::currentDateTime().toString()
                + " Received Topic: "
                + topic.name()
                + " Message: "
                + message
                + u'\n';
   // 解析JSON文档
   QJsonParseError error;
   QJsonDocument doc = QJsonDocument::fromJson(message, &error);

   if (error.error != QJsonParseError::NoError) {
       qDebug() << " JSON解析失败： " << error.errorString();
       return;
   }

   // 转换为JSON对象
   QJsonObject rootObj = doc.object();

   QString ip,name,t;
   // 提取IP地址
   if (rootObj.contains("Ip") && rootObj["Ip"].isString()) {
        ip = rootObj["Ip"].toString();
       qDebug() << " IP地址: " << ip; // 输出: "195.2.2.1"
   }

   // 提取设备名称
   if (rootObj.contains("name") && rootObj["name"].isString()) {
        name = rootObj["name"].toString();
       qDebug() << " 设备名称: " << name; // 输出: "001"
   }

   // 提取设备温度
   if (rootObj.contains("T") && rootObj["T"].isString()) {
        t = rootObj["T"].toString();
       qDebug() << "设备温度:" << t; // 输出: "001"
   }

   emit sendDataToSQL(ip,name,t);
   qDebug()<<message<<endl;
   qDebug() << content<<endl;
}

void my_mqtt_client::MyMQTTSendMessage(const QString topic, const QString message)
{
    if (m_client->publish(topic, message.toUtf8()) == -1){
        qDebug() << "Could not publish message";
    }
}




