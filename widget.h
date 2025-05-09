#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMediaPlayer>
#include <QVideoWidget>
#include<QtMqtt/qmqttclient.h>
#include <QTableWidget>
#include<QAction>
#include<QMenu>
#include<QDialogButtonBox>
#include<QFormLayout>
#include "pullflowthread.h"
#include"my_mqtt_client.h"
#include"my_sql.h"
#include"add_equipment_dialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    void unblockSleep(unsigned int ms);

public slots:

    void on_tabWidget_2_tabBarClicked(int index);

    void on_pullflow_pushButton_clicked();
    void onDisplayVideo(const QImage &image);




private slots:
    void on_send_pushButton_clicked();

    void on_receive_pushButton_clicked();

    void on_connnect_pushButton_clicked();

    void on_Subscribe_pushButton_clicked();

    void on_refresh_pushButton_clicked();

    void on_add_eqiopment_pushButton_clicked();

    void receiveData_From_AddEquipment(QString ip,QString Name);

    void receiveEquipment_From_AddEquipment(QString ip,QString Name);

    void showDetail(int id);

    void on_add_epuipment_pushButton_clicked();

    void on_refresh_Equipment_pushButton_clicked();

    // 上下文菜单请求处理
    void onCustomContextMenuRequested(const QPoint &pos);
    //修改设备
    void modifyDevice();
    //删除设备
    void deleteDevice();


private:
    Ui::Widget *ui;
    QTableWidget *equipment_table;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    QMqttClient *client;
    PullFlowThread *m_pullFlowThread;
    my_mqtt_client *m_mqtt;
    my_sql* m_sql;
    my_sql* equipment_sql;
    int num_equipment;


//函数
private:
    //加载设备
    void loadData_equipment();
    //显示修改界面
    void showModifyDialog(int deviceId);
    //获取修改设备ID
    int getSelectedDeviceId();
};
#endif // WIDGET_H
