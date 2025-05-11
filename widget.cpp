#include "widget.h"
#include "ui_widget.h"
#include <QtMultimedia/QtMultimedia>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QFileDialog>
#include <QUrl>
#include <QDebug>
#include <QTabBar>
#include <QMessageBox>
#include <QSqlQuery>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , m_isUserLoggedIn(false)
    ,m_menuBar(nullptr) // 初始化菜单栏指针
{
    ui->setupUi(this);

    // 初始化登录相关UI
    setupLoginUI();

    //设置设备表
    equipment_table = ui->equipment_tableWidget;
    equipment_table->setColumnCount(5);
    equipment_table->setHorizontalHeaderLabels({"Device_id","IP", "Battery","Condition","Update_time"});
    equipment_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    equipment_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    equipment_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    num_equipment=0;//初始设备数
    //设置设备表指针
    equipment_sql = new my_sql("equipment");
    //加载设备数据
    loadData_equipment();
    // 连接点击信号
    connect(equipment_table, &QTableWidget::cellDoubleClicked, [this](int row, int){
        QTableWidgetItem *item = equipment_table->item(row, 0);
        if(item) showDetail(item->text());
     });
    // 启用右键菜单策略
    equipment_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(equipment_table, &QTableWidget::customContextMenuRequested,
            this, &Widget::onCustomContextMenuRequested);

    // 初始化用户表
    userTable = ui->user_tableWidget;
    userTable->setColumnCount(4);
    userTable->setHorizontalHeaderLabels({"UserName", "Password", "Power","logtime"});
    userTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    userTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    userTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置用户表指针
    user_sql = new my_sql("users");
    //加载用户数据
    load_User();
    // 启用右键菜单
    userTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(userTable, &QTableWidget::customContextMenuRequested,
            this, &Widget::onUserContextMenuRequested);

    // 初始化设备信息表
    DeviceDataTable = ui->devicedata_tableWidget;
    DeviceDataTable->setColumnCount(5);
    DeviceDataTable->setHorizontalHeaderLabels({"Device_Id", "Smoke", "temperature","humidness","Updata_time"});
    DeviceDataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    DeviceDataTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    DeviceDataTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //设置设备数据表指针
    device_sql = new my_sql("devicedata");


    m_pullFlowThread = new PullFlowThread;
    m_mqtt = new my_mqtt_client();
    m_mqtt->setHost(ui->Host_lineEdit->text());
    m_mqtt->setport(ui->Port_lineEdit->text());

    connect(ui->Host_lineEdit,&QLineEdit::textChanged,m_mqtt,&my_mqtt_client::setHost);
    connect(ui->Port_lineEdit,&QLineEdit::textChanged,m_mqtt,&my_mqtt_client::setport);

    //数据库初始化
    m_sql = new my_sql();



    //连接mqtt接收数据与sql插入数据
    connect(m_mqtt,&my_mqtt_client::sendDataToSQL,m_sql,&my_sql::Insert);

    connect(ui->tabWidget_2->tabBar(), &QTabBar::tabBarClicked, this, &Widget::on_tabWidget_2_tabBarClicked);




    //接收消息信号
    connect(m_mqtt, &my_mqtt_client::receiveMess, this, [this](QString msg) {
            ui->receive_textEdit_2->insertPlainText(msg);  // 在 UI 中显示消息
        });


    connect(m_pullFlowThread,&PullFlowThread::displayVideo,this,&Widget::onDisplayVideo);



    setFocus();
    // 显示登录对话框
    showLoginDialog();
}

Widget::~Widget()
{
    // 请求关闭拉流线程
    m_pullFlowThread->closePullFlow();
    // 阻塞等待线程退出
    while (!m_pullFlowThread->isClosePullFlow())
        unblockSleep(10);

    delete m_pullFlowThread;
    m_pullFlowThread = nullptr;

    delete m_mqtt;
    m_mqtt = nullptr;

    delete m_sql;
    m_sql = nullptr;

    delete equipment_sql;
    equipment_sql = nullptr;

    delete ui;
}

// 非阻塞延时函数，避免界面冻结
void Widget::unblockSleep(unsigned int ms)
{
    QTime dieTime = QTime::currentTime().addMSecs(ms);
    while(QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Widget::on_tabWidget_2_tabBarClicked(int index)
{

     qDebug() << "点击了选项卡：" << index;
     if(index==0)
     {
         //ui->verticalLayout->addWidget(videoWidget);
     }
     else if (index==1)
     {
         //ui->verticalLayout_2->addWidget(videoWidget);
     }
}





// 显示视频帧槽函数
void Widget::onDisplayVideo(const QImage &image)
{
    QPixmap pixmap;
    pixmap.convertFromImage(image);
    //qDebug()<<" 显示 "<<endl;
    ui->video_label->setPixmap(pixmap);

}

void Widget::on_pullflow_pushButton_clicked()
{

    QString Url1 = "rtmp://localhost:1935/live/";
    QString Url ="rtmp://liteavapp.qcloud.com/live/liteavdemoplayerstreamid";
    m_pullFlowThread->toStartPullFlow(Url1);
}


void Widget::on_receive_pushButton_clicked()
{


}

//连接
void Widget::on_connnect_pushButton_clicked()
{
    m_mqtt->m_client->connectToHost();
    if(m_mqtt->m_client->state() == QMqttClient::Disconnected)
    {
        QMessageBox::critical(this, "Error",
                              "Could not connect. Is there a valid connection?");
    }
    else
    {
        QMessageBox::information(this,"inf","connect_successful");
    }
}


//订阅
void Widget::on_Subscribe_pushButton_clicked()
{
    auto subscription = m_mqtt->m_client->subscribe(ui->Topic_lineEdit->text());
    if(!subscription)// 连接后订阅
    {
        QMessageBox::critical(this, "Error",
                              "Could not subscribe. Is there a valid connection?");
        return;
    }
    /*else
    {
        QMessageBox::critical(this, "Test",
                              " subscribe successful");
    }*/
}

void Widget::on_send_pushButton_clicked()
{
    QString topic = ui->Topic_lineEdit->text();
    QString msg = ui->send_textEdit->toPlainText();
    m_mqtt->MyMQTTSendMessage(topic,msg);

}


//接收窗口传过来的设备数据，并存入数据库
void Widget::receiveEquipment_From_AddEquipment(QString ip, QString Name)
{
    qDebug()<<num_equipment<<' '<<Name<<' '<<ip<<endl;
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 以自定义格式输出当前日期和时间，例如：yyyy-MM-dd hh:mm:ss
    QString logtime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    equipment_sql->Insert2Equipment(Name,ip,"0","off",logtime);
    loadData_equipment();
}


//加载设备数据
void Widget::loadData_equipment()
{
    equipment_table->setRowCount(0);
    QSqlQuery query = equipment_sql->executeQuery(
            "SELECT Device_id, IP, Battery, Condition1, Update_time1 FROM equipment"); // 修改为你的表结构

        int row = 0;
        while (query.next()) {
            equipment_table->insertRow(row);
            equipment_table->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
            equipment_table->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            equipment_table->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
            equipment_table->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
            equipment_table->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));

            row++;
        }
        num_equipment = row;//更新设备数
}

//显示设备详情
void Widget::showDetail(QString name)
{
    qDebug()<<name<<endl;
    QDialog dialog(this);
        dialog.setWindowTitle("Details");

        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        // 查询详细信息
        QSqlQuery query;
        query.prepare("SELECT * FROM devicedata WHERE Device_Id = ?");
        query.addBindValue(name);
        if (query.exec()) {
            qDebug()<<name<<endl;
            DeviceDataTable->setRowCount(0);
            int row = 0;
            while (query.next()) {
                DeviceDataTable->insertRow(row);
                DeviceDataTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
                DeviceDataTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
                DeviceDataTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
                DeviceDataTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));
                DeviceDataTable->setItem(row, 4, new QTableWidgetItem(query.value(4).toString()));
                row++;
             }
        }
        else {
            layout->addWidget(new QLabel("Details not found", &dialog));
            QPushButton *closeBtn = new QPushButton("Close", &dialog);
            connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
            layout->addWidget(closeBtn);
            dialog.exec();
            DeviceDataTable->setRowCount(0);
        }


}

//新增设备
void Widget::on_add_epuipment_pushButton_clicked()
{
    Add_Equipment_Dialog Add_Equipment_Dialog(this);
    connect(&Add_Equipment_Dialog,&Add_Equipment_Dialog::sendData,this,&Widget::receiveEquipment_From_AddEquipment);
    Add_Equipment_Dialog.exec();
}
//刷新设备
void Widget::on_refresh_Equipment_pushButton_clicked()
{
    loadData_equipment();
}

// 上下文菜单请求处理
void Widget::onCustomContextMenuRequested(const QPoint &pos)
{
    QTableWidgetItem *item = equipment_table->itemAt(pos);
        if (!item) return;

    // 创建菜单
    QMenu menu(this);
    QAction *modifyAction = menu.addAction("修改设备");
    QAction *deleteAction = menu.addAction("删除设备");

    // 连接信号
    connect(modifyAction, &QAction::triggered, this, &Widget::modifyDevice);
    connect(deleteAction, &QAction::triggered, this, &Widget::deleteDevice);

    menu.exec(equipment_table->viewport()->mapToGlobal(pos));
}

// 获取当前选中设备的Device_id
QString Widget::getSelectedDeviceId()
{
    int row = equipment_table->currentRow();
    if (row == -1) return "";

    QTableWidgetItem *item = equipment_table->item(row, 0);
    return item->text();
}

// 修改设备
void Widget::modifyDevice()
{
    QString deviceId = getSelectedDeviceId();
    if (deviceId == "") return;

    showModifyDialog(deviceId);
    loadData_equipment();
}
// 删除设备
void Widget::deleteDevice()
{
    QString deviceId = getSelectedDeviceId();
    if (deviceId == "") return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, " 确认删除  ",
                                "  确定要删除该设备吗？  ",
                                QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM equipment WHERE Device_id = ?");
        query.addBindValue(deviceId);

        if (query.exec()) {
            loadData_equipment();
            QMessageBox::information(this, " 成功  ", "  设备已删除  ");
        } else {
            QMessageBox::critical(this, "  错误  ", "  删除失败:   " + query.lastError().text());
        }
    }
}

// 显示修改对话框
void Widget::showModifyDialog(QString deviceId)
{
    // 查询当前设备信息
    QSqlQuery query;
    query.prepare("SELECT * FROM equipment WHERE Device_id = ?");
    query.addBindValue(deviceId);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "  错误  ", "  无法获取设备信息  ");
        return;
    }

    // 创建对话框
    QDialog dialog(this);
    dialog.setWindowTitle(" 修改设备  ");
    QFormLayout layout(&dialog);

    // 创建输入框
    QLineEdit *nameEdit = new QLineEdit(query.value("name").toString());
    QLineEdit *ipEdit = new QLineEdit(query.value("ip").toString());

    layout.addRow("设备名称:", nameEdit);
    layout.addRow("IP地址:", ipEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                             Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QSqlQuery updateQuery;
        updateQuery.prepare(
            "UPDATE equipment SET "
            "Device_id = ?, IP = ? "
            "WHERE Device_id = ?");
        updateQuery.addBindValue(nameEdit->text());
        updateQuery.addBindValue(ipEdit->text());
        updateQuery.addBindValue(deviceId);

        if (updateQuery.exec()) {
            loadData_equipment();
            QMessageBox::information(this, "  成功  ", "  设备信息已更新  ");
        } else {
            QMessageBox::critical(this, "  错误  ", "  更新失败:   " + updateQuery.lastError().text());
        }
    }
}


//添加用户按钮点击
void Widget::on_add_user_pushButton_clicked()
{
    add_user_Dialog add_user_Dialog(this);
    connect(&add_user_Dialog,&add_user_Dialog::sendUser,this,&Widget::receiveUser_From_AddUser);
    add_user_Dialog.exec();
}
// 刷新用户按钮点击
void Widget::on_refresh_user_pushButton_clicked()
{
    qDebug()<<" refresh "<<endl;
    load_User();
}

void Widget::receiveUser_From_AddUser(QString name, QString password, QString power)
{
    qDebug()<<name<<' '<<password<<endl;
    // 仅获取当前日期
    // 获取当前日期和时间
    QDateTime currentDateTime = QDateTime::currentDateTime();
    // 以自定义格式输出当前日期和时间，例如：yyyy-MM-dd hh:mm:ss
    QString logtime = currentDateTime.toString("yyyy-MM-dd hh:mm:ss");
    user_sql->Insert2User(name,password,power,logtime);
    load_User();
}

void Widget::modifyUser()
{
    QString UserName = getSelectedUsername();
    if (UserName == "") return;

    showModifyUserDialog(UserName);
    load_User();
}
// 显示用户修改对话框
void Widget::showModifyUserDialog(QString UserName)
{
    // 查询当前设备信息
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = ?");
    query.addBindValue(UserName);

    if (!query.exec() || !query.next()) {
        QMessageBox::critical(this, "  错误  ", "  无法获取用户信息  ");
        return;
    }

    // 创建对话框
    QDialog dialog(this);
    dialog.setWindowTitle(" 修改用户信息  ");
    QFormLayout layout(&dialog);

    // 创建输入框
    QLineEdit *usernameEdit = new QLineEdit(query.value("username").toString());
    QLineEdit *userPassowrdEdit = new QLineEdit(query.value("password").toString());
    QLineEdit *userPowerEdit = new QLineEdit(query.value("power").toString());


    layout.addRow("用户名称:", usernameEdit);
    layout.addRow("用户密码:", userPassowrdEdit);
    layout.addRow("用户权限:", userPowerEdit);

    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                             Qt::Horizontal, &dialog);
    layout.addRow(&buttonBox);

    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        QSqlQuery updateQuery;
        updateQuery.prepare(
            "UPDATE users SET "
            "username = ?, password = ?, power = ? "
            "WHERE username = ?");
        updateQuery.addBindValue(usernameEdit->text());
        updateQuery.addBindValue(userPassowrdEdit->text());
        updateQuery.addBindValue(userPowerEdit->text());
        updateQuery.addBindValue(UserName);

        if (updateQuery.exec()) {
            loadData_equipment();
            QMessageBox::information(this, "  成功  ", "  用户信息已更新  ");
        } else {
            QMessageBox::critical(this, "  错误  ", "  更新失败:   " + updateQuery.lastError().text());
        }
    }
}


//获取当前选中用户的name
QString Widget::getSelectedUsername()
{
    int row = userTable->currentRow();
    if (row == -1) return "";

    QTableWidgetItem *item = userTable->item(row, 0);
    return item->text();
}
void Widget::deleteUser()
{
    QString UserName = getSelectedUsername();
    if (UserName=="") return;

    QString username = getSelectedUsername();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, " 确认删除  ",
                                    "  确定要删除该用户吗？  ",
                                    QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM users WHERE username = ?");
        query.addBindValue(username);

        if (query.exec()) {
            loadData_equipment();
            QMessageBox::information(this, " 成功  ", "  用户已删除  ");
        } else {
            QMessageBox::critical(this, "  错误  ", "  删除失败:   " + query.lastError().text());
        }
   }
    load_User();
}

void Widget::onUserContextMenuRequested(const QPoint &pos)
{
    QTableWidgetItem *item = userTable->itemAt(pos);
            if (!item) return;

    // 创建菜单
    QMenu menu(this);
    QAction *modifyAction = menu.addAction(" 修改用户信息 ");
    QAction *deleteAction = menu.addAction(" 删除用户信息 ");

    // 连接信号
    connect(modifyAction, &QAction::triggered, this, &Widget::modifyUser);
    connect(deleteAction, &QAction::triggered, this, &Widget::deleteUser);

    menu.exec(equipment_table->viewport()->mapToGlobal(pos));
}

//加载用户数据
void Widget::load_User()
{
    qDebug()<<"loaduser"<<endl;
    userTable->setRowCount(0);
    QSqlQuery query = user_sql->executeQuery(
            "SELECT username, password, power,logtime FROM users"); // 修改为你的表结构

    int row = 0;
    while (query.next()) {
        userTable->insertRow(row);
        userTable->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));
        userTable->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
        userTable->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));
        userTable->setItem(row, 3, new QTableWidgetItem(query.value(3).toString()));

        row++;
    }
}

void Widget::load_devicedata()
{

}

// 新增用户认证函数
bool Widget::authenticateUser(const QString &username, const QString &password)
{
    // 在实际应用中，这里应该查询数据库或其他认证系统
    // 示例：从数据库验证用户
    QSqlQuery query;
    query.prepare("SELECT * FROM users WHERE username = ? AND password = ?");
    query.addBindValue(username);
    query.addBindValue(password);

    if (query.exec() && query.next()) {
        qDebug() << " 用户认证成功:" << username;
        return true;
    }

    qDebug() << " 用户认证失败: " << username;
    return false;
}

// 设置登录相关UI元素
void Widget::setupLoginUI()
{
    // 创建菜单栏并添加到布局
        m_menuBar = new QMenuBar(this);
        QVBoxLayout *mainLayout = new QVBoxLayout(this);

        // 如果UI已经有布局，先移除
        if (layout()) {
            QLayoutItem *item;
            while ((item = layout()->takeAt(0)) != nullptr) {
                if (item->widget())
                    mainLayout->addWidget(item->widget());
                delete item;
            }
            delete layout();
        }
        // 添加菜单栏到布局顶部
            mainLayout->setMenuBar(m_menuBar);

            QMenu *systemMenu = m_menuBar->addMenu("  系统  ");

            // 添加登出菜单项
            m_logoutAction = new QAction("  退出登录  ", this);
            connect(m_logoutAction, &QAction::triggered, this, &Widget::onLogoutActionTriggered);
            systemMenu->addAction(m_logoutAction);

            // 初始状态下禁用登出菜单项
            m_logoutAction->setEnabled(false);
}

// 显示登录对话框
void Widget::showLoginDialog()
{
    LoginDialog loginDialog(this);

    while (true) {
        if (loginDialog.exec() == QDialog::Accepted) {
            QString username = loginDialog.getUsername();
            QString password = loginDialog.getPassword();

            if (authenticateUser(username, password)) {
                m_isUserLoggedIn = true;
                m_currentUsername = username; // 保存用户名
                updateUIForLoginState();
                break;
            } else {
                QMessageBox::critical(this, "  登录失败  ", "  用户名或密码错误!  ");
            }
        } else {
            QApplication::quit();
            return;
        }
    }
}

// 根据登录状态更新UI
void Widget::updateUIForLoginState()
{
    m_logoutAction->setEnabled(m_isUserLoggedIn);

    // 根据登录状态启用/禁用主界面元素
    ui->tabWidget_2->setEnabled(m_isUserLoggedIn);
    ui->receive_pushButton->setEnabled(m_isUserLoggedIn);
    ui->connnect_pushButton->setEnabled(m_isUserLoggedIn);
    // ... 其他需要控制的UI元素 ...

    if (m_isUserLoggedIn) {
        QMessageBox::information(this, "  登录成功  ", "  欢迎回来，  " + m_currentUsername);
        // 加载数据等操作
        show();
        loadData_equipment();

    }
}

//登录界面
void Widget::onLogoutActionTriggered()
{
    QMessageBox::StandardButton reply = QMessageBox::question(
            this, "  确认退出  ", "  确定要退出登录吗?  ", QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            m_isUserLoggedIn = false;
            m_currentUsername.clear(); // 清除当前用户名
            updateUIForLoginState();
            // 隐藏主窗口
            hide();
            showLoginDialog();
        }

}


