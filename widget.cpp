#include "widget.h"
#include "ui_widget.h"
#include <QtMultimedia/QtMultimedia>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QFileDialog>
#include <QUrl>
#include<QDebug>
#include<QTabBar>
#include<QMessageBox>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    //设置设备表
    equipment_table = ui->equipment_tableWidget;
    equipment_table->setColumnCount(3);
    equipment_table->setHorizontalHeaderLabels({"ID","Name", "IP"});
    equipment_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    equipment_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    equipment_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    num_equipment=0;//初始设备数
    //设置设备表指针
    equipment_sql = new my_sql("equipment");
    //加载设备数据
    loadData_equipment();
    // 连接点击信号
    connect(equipment_table, &QTableWidget::cellClicked, [this](int row, int){
        QTableWidgetItem *item = equipment_table->item(row, 0);
        if(item) showDetail(item->data(Qt::UserRole).toInt());
     });
    // 启用右键菜单策略
    equipment_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(equipment_table, &QTableWidget::customContextMenuRequested,
            this, &Widget::onCustomContextMenuRequested);




    m_pullFlowThread = new PullFlowThread;
    m_mqtt = new my_mqtt_client();
    m_mqtt->setHost(ui->Host_lineEdit->text());
    m_mqtt->setport(ui->Port_lineEdit->text());

    connect(ui->Host_lineEdit,&QLineEdit::textChanged,m_mqtt,&my_mqtt_client::setHost);
    connect(ui->Port_lineEdit,&QLineEdit::textChanged,m_mqtt,&my_mqtt_client::setport);

    //数据库初始化
    m_sql = new my_sql();
    ui->Data_tableView->setModel(m_sql->getModel());
    ui->Data_tableView->show();//显示表格


    //连接mqtt接收数据与sql插入数据
    connect(m_mqtt,&my_mqtt_client::sendDataToSQL,m_sql,&my_sql::Insert);

    connect(ui->tabWidget_2->tabBar(), &QTabBar::tabBarClicked, this, &Widget::on_tabWidget_2_tabBarClicked);




    //接收消息信号
    connect(m_mqtt, &my_mqtt_client::receiveMess, this, [this](QString msg) {
            ui->receive_textEdit_2->insertPlainText(msg);  // 在 UI 中显示消息
        });


    connect(m_pullFlowThread,&PullFlowThread::displayVideo,this,&Widget::onDisplayVideo);



    setFocus();
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


void Widget::on_refresh_pushButton_clicked()
{
    m_sql->getModel()->select();
    ui->Data_tableView->viewport()->update();
}

//弹出添加设备的窗口
void Widget::on_add_eqiopment_pushButton_clicked()
{
    Add_Equipment_Dialog Add_Equipment_Dialog(this);
    connect(&Add_Equipment_Dialog,&Add_Equipment_Dialog::sendData,this,&Widget::receiveEquipment_From_AddEquipment);
    Add_Equipment_Dialog.exec();
}

//接收窗口传过来的数据，并存入数据库
void Widget::receiveData_From_AddEquipment(QString Ip, QString Name)
{
    qDebug()<<Ip<<' '<<Name<<endl;
    m_sql->Insert(Ip,Name,"0");
}

//接收窗口传过来的设备数据，并存入数据库
void Widget::receiveEquipment_From_AddEquipment(QString ip, QString Name)
{
    qDebug()<<num_equipment<<' '<<Name<<' '<<ip<<endl;
    equipment_sql->Insert2Equipment(num_equipment+1,Name,ip);
    loadData_equipment();
}



//加载设备数据
void Widget::loadData_equipment()
{
    equipment_table->setRowCount(0);
    QSqlQuery query = equipment_sql->executeQuery(
            "SELECT id, name, ip FROM equipment"); // 修改为你的表结构

        int row = 0;
        while (query.next()) {
            equipment_table->insertRow(row);

            // 第一列存储ID
            QTableWidgetItem *idItem = new QTableWidgetItem(
                    query.value(1).toString()); // 显示用户名
            idItem->setData(Qt::UserRole, query.value(0)); // 存储用户ID
            equipment_table->setItem(row, 0, idItem);

            equipment_table->setItem(row, 1, new QTableWidgetItem(query.value(1).toString()));
            equipment_table->setItem(row, 2, new QTableWidgetItem(query.value(2).toString()));

            row++;
        }
        num_equipment = row;//更新设备数
}

//显示设备详情
void Widget::showDetail(int id)
{
    qDebug()<<id<<endl;
    QDialog dialog(this);
        dialog.setWindowTitle("Details");

        QVBoxLayout *layout = new QVBoxLayout(&dialog);

        // 查询详细信息
        QSqlQuery query;
        query.prepare("SELECT * FROM users WHERE user_id = ?");
        query.addBindValue(id);

        if (query.exec() && query.next()) {
            QString details = QString("ID: %1\nName: %2\nEmail: %3")
                .arg(query.value(0).toString())
                .arg(query.value(1).toString())
                .arg(query.value(2).toString());

            layout->addWidget(new QLabel(details, &dialog));
        } else {
            layout->addWidget(new QLabel("Details not found", &dialog));
        }

        QPushButton *closeBtn = new QPushButton("Close", &dialog);
        connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);
        layout->addWidget(closeBtn);

        dialog.exec();
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

// 获取当前选中设备的ID
int Widget::getSelectedDeviceId()
{
    int row = equipment_table->currentRow();
    if (row == -1) return -1;

    QTableWidgetItem *item = equipment_table->item(row, 0);
    return item->data(Qt::UserRole).toInt();
}
// 修改设备
void Widget::modifyDevice()
{
    int deviceId = getSelectedDeviceId();
    if (deviceId == -1) return;

    showModifyDialog(deviceId);
}
// 删除设备
void Widget::deleteDevice()
{
    int deviceId = getSelectedDeviceId();
    if (deviceId == -1) return;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, " 确认删除  ",
                                "  确定要删除该设备吗？  ",
                                QMessageBox::Yes|QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        QSqlQuery query;
        query.prepare("DELETE FROM equipment WHERE id = ?");
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
void Widget::showModifyDialog(int deviceId)
{
    // 查询当前设备信息
    QSqlQuery query;
    query.prepare("SELECT * FROM equipment WHERE id = ?");
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
            "name = ?, ip = ? "
            "WHERE id = ?");
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

