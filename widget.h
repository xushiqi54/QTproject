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
#include"add_user_dialog.h"
#include "login_dialog.h" // 添加登录对话框头文件
#include <QMenuBar>
#include<QTime>

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
    // 用户认证函数
    bool authenticateUser(const QString &username, const QString &password); // 新增用户认证函数

public slots:

    void on_tabWidget_2_tabBarClicked(int index);

    //拉流按钮点击
    void on_pullflow_pushButton_clicked();
    // 显示视频
    void onDisplayVideo(const QImage &image);




private slots:
    //消息管理发送按钮点击
    void on_send_pushButton_clicked();
    //消息管理接收按钮点击
    void on_receive_pushButton_clicked();
    //消息管理连接按钮点击
    void on_connnect_pushButton_clicked();
    //消息管理订阅按钮点击
    void on_Subscribe_pushButton_clicked();

    //接收新增设备
    void receiveEquipment_From_AddEquipment(QString ip,QString Name);
    //显示设备详情
    void showDetail(QString name);
    //新增设备按钮点击
    void on_add_epuipment_pushButton_clicked();
    //设备更新按钮点击
    void on_refresh_Equipment_pushButton_clicked();

    // 上下文菜单请求处理
    void onCustomContextMenuRequested(const QPoint &pos);
    //修改设备
    void modifyDevice();
    //删除设备
    void deleteDevice();


    //新增用户按钮点击
    void on_add_user_pushButton_clicked();
    //刷新用户按钮点击
    void on_refresh_user_pushButton_clicked();
    //接收新增用户
    void receiveUser_From_AddUser(QString name,QString password,QString power);
    //修改用户信息
    void modifyUser();
    //显示修改用户对话框
    void showModifyUserDialog(QString UserName);
    //删除用户
    void deleteUser();
    //修改用户上下文
    void onUserContextMenuRequested(const QPoint &pos);

    // 新增登出函数
    void onLogoutActionTriggered();


private:
    Ui::Widget *ui;
    QTableWidget *equipment_table;
    QTableWidget* userTable;
    QTableWidget* DeviceDataTable;
    QMediaPlayer *player;
    QVideoWidget *videoWidget;
    QMqttClient *client;
    PullFlowThread *m_pullFlowThread;
    my_mqtt_client *m_mqtt;
    my_sql* m_sql;
    my_sql* equipment_sql;
    my_sql* user_sql;
    my_sql* device_sql;
    int num_equipment;
    bool m_isUserLoggedIn; // 新增用户登录状态
    QAction *m_logoutAction; // 新增登出菜单项
    QString m_currentUsername; // 新增：存储当前登录用户的用户名
    QMenuBar *m_menuBar; // 添加菜单栏指针
//函数
private:
    //加载设备
    void loadData_equipment();
    //加载用户
    void load_User();
    //加载设备数据
    void load_devicedata();
    //显示修改界面
    void showModifyDialog(QString deviceId);
    //获取修改设备ID
    QString getSelectedDeviceId();
    //获取修改用户名
    QString getSelectedUsername();

    void setupLoginUI(); // 设置登录相关UI元素
    void setupMainUI();  // 设置主界面UI元素
    void showLoginDialog(); // 显示登录对话框
    void updateUIForLoginState(); // 根据登录状态更新UI
};
#endif // WIDGET_H
