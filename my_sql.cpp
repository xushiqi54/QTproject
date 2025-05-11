#include "my_sql.h"

my_sql::my_sql()
{
    qDebug()<<QSqlDatabase::drivers();

    //连接数据库
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("MyDatabase_BiYe");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("200354");

    qDebug()<<QSqlDatabase::drivers();

    if(db.open())
    {
        qDebug()<<" 连接成功 "<<endl;
        m = new QSqlTableModel(nullptr, db);
        m->setTable("Data");
        m->setEditStrategy(QSqlTableModel::OnManualSubmit); // 可选：设置编辑策略
        m->select();  // 新增：立即加载数据
    }
    else
    {
        QMessageBox::critical(nullptr, " 数据库错误 ", " 连接失败： " + db.lastError().text()); // 新增：弹窗提示    }
        qDebug() << " 连接失败： " << db.lastError().text();
    }
}

my_sql::my_sql(QString Table)
{
    qDebug()<<QSqlDatabase::drivers();

    //连接数据库
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setDatabaseName("MyDatabase_BiYe");
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("200354");

    qDebug()<<QSqlDatabase::drivers();

    if(db.open())
    {
        qDebug()<<" 连接成功 "<<Table<<endl;
        m = new QSqlTableModel(nullptr, db);
        m->setTable(Table);
        m->setEditStrategy(QSqlTableModel::OnManualSubmit); // 可选：设置编辑策略
        m->select();  // 新增：立即加载数据
    }
    else
    {
        QMessageBox::critical(nullptr, " 数据库错误 ", " 连接失败： " + db.lastError().text()); // 新增：弹窗提示    }
        qDebug() << " 连接失败： " << db.lastError().text();
    }
}

QSqlQuery my_sql::executeQuery(const QString &query)
{
    QSqlQuery q;
        if (!q.exec(query)) {
            QMessageBox::critical(nullptr, "Query Error",
                "Query failed: " + q.lastError().text());
        }
        return q;
}


void my_sql::Insert(QString Ip,QString Name,QString T)
{


    QString sql = QString("insert into data values (%1, '%2', '%3');").arg(Name).arg(Ip).arg(T);

    qDebug()<<sql;

    QSqlQuery query;
        query.prepare("INSERT INTO Data (name, ip, temperature) VALUES (?, ?, ?)"); // 修复 SQL 注入
        query.addBindValue(Name);
        query.addBindValue(Ip);
        query.addBindValue(T);

        if (query.exec()) {
            qDebug() << " 插入成功 ";
            m->select();  // 新增：插入后刷新模型
        } else {
            qDebug() << " 插入失败： " << query.lastError().text();
        }
}
void my_sql::Find()
{
    m->select();
}

//向设备表中插入设备数据
void my_sql::Insert2Equipment(QString Device_id,QString IP,QString Battery,QString Condition,QString Update_time)
{
    QString sql = QString("insert into equipment values ('%1', '%2', '%3','%4','%5');").arg(Device_id).arg(Battery).arg(Condition).arg(Update_time);

    qDebug()<<sql;

    QSqlQuery query;
    query.prepare("INSERT INTO equipment (Device_id, IP, Battery, Condition1, Update_time1) "
                     "VALUES (:deviceId, :ip, :battery, :Condition, :Update_time1)");

       // 正确绑定参数
       query.bindValue(":deviceId", Device_id);
       query.bindValue(":ip", IP);
       query.bindValue(":battery", Battery);
       query.bindValue(":Condition", Condition);
       query.bindValue(":Update_time1", Update_time);

        if (query.exec()) {
            qDebug() << " 插入成功 ";
        } else {
            qDebug() << " 插入失败： " << query.lastError().text();
        }
}

//向用户表中插入数据
void my_sql::Insert2User(QString USer_name, QString User_passwoed, QString User_power,QString logtime)
{
    QString sql = QString("insert into users values ('%1', '%2', '%3','%4');").arg(USer_name).arg(User_passwoed).arg(User_power).arg(logtime);

    qDebug()<<sql;

    QSqlQuery query;
        query.prepare("INSERT INTO users (username, password, power,logtime) VALUES (?, ?, ?,?)"); // 修复 SQL 注入
        query.addBindValue(USer_name);
        query.addBindValue(User_passwoed);
        query.addBindValue(User_power);
        query.addBindValue(logtime);

        if (query.exec()) {
            qDebug() << " 插入成功 ";
        } else {
            qDebug() << " 插入失败： " << query.lastError().text();
        }
}

void my_sql::Insert2Devicedata(QString Device_id, QString Smoke, QString temperature, QString humidness, QString Updata_time)
{
    QString sql = QString("insert into users values ('%1', '%2', '%3','%4');").arg(Device_id).arg(Smoke).arg(temperature).arg(humidness).arg(Updata_time);

    qDebug()<<sql;


    QSqlQuery query;
    query.prepare("INSERT INTO devicedata (Device_id,Smoke,temperature,humidness,Updata_time) VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(Device_id);
    query.addBindValue(Smoke);
    query.addBindValue(temperature);
    query.addBindValue(humidness);
    query.addBindValue(Updata_time);
    if (query.exec()) {
        qDebug() << " 插入成功 ";
    } else {
        qDebug() << " 插入失败： " << query.lastError().text();
    }
}
