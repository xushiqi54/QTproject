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
        qDebug()<<" 连接成功 "<<endl;
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

//    QString id = "001";
//    QString Type = "msg";
//QString msg = "Tmp = 16.8";

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
void my_sql::Insert2Equipment(int ID, QString Name, QString IP)
{
    QString sql = QString("insert into equipment values (%1, '%2', '%3');").arg(ID).arg(Name).arg(IP);

    qDebug()<<sql;

    QSqlQuery query;
        query.prepare("INSERT INTO equipment (id, name, ip) VALUES (?, ?, ?)"); // 修复 SQL 注入
        query.addBindValue(ID);
        query.addBindValue(Name);
        query.addBindValue(IP);

        if (query.exec()) {
            qDebug() << " 插入成功 ";
        } else {
            qDebug() << " 插入失败： " << query.lastError().text();
        }
}
