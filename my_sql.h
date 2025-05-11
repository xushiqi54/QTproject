#ifndef MY_SQL_H
#define MY_SQL_H

#include<QSqlDatabase>
#include<QMessageBox>
#include <QSqlError>
#include<QDebug>
#include<QSqlQuery>
#include<QSqlTableModel>
#include<qsqltablemodel.h>

class my_sql : public QObject
{
Q_OBJECT
public:
    my_sql();
    my_sql(QString Table);
    QSqlTableModel* getModel(){return m;}
    //读取SQL数据
    static QSqlQuery executeQuery(const QString& query);

public slots:
    void Insert(QString IP,QString Name,QString T);
    void Find();
    void Insert2Equipment(QString Device_id,QString IP,QString Battery,QString Condition,QString Update_time);
    void Insert2User(QString USer_name,QString User_passwoed,QString User_power,QString logtime);
    void Insert2Devicedata(QString Device_id,QString Smoke,QString temperature,QString humidness,QString Updata_time);
private:
    QSqlDatabase db;
    QSqlTableModel* m;

};

#endif // MY_SQL_H
