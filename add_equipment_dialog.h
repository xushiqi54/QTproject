#ifndef ADD_EQUIPMENT_DIALOG_H
#define ADD_EQUIPMENT_DIALOG_H

#include <QDialog>

namespace Ui {
class Add_Equipment_Dialog;
}

class Add_Equipment_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Add_Equipment_Dialog(QWidget *parent = nullptr);
    ~Add_Equipment_Dialog();

signals:
    void sendData(QString Ip,QString Name);

private slots:
    void on_Send_pushButton_clicked();

private:
    Ui::Add_Equipment_Dialog *ui;
};

#endif // ADD_EQUIPMENT_DIALOG_H
