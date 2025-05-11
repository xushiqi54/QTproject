#ifndef ADD_USER_DIALOG_H
#define ADD_USER_DIALOG_H

#include <QDialog>

namespace Ui {
class add_user_Dialog;
}

class add_user_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit add_user_Dialog(QWidget *parent = nullptr);
    ~add_user_Dialog();

signals:
    void sendUser(QString username,QString password,QString power);

private slots:

    void on_Send_user_pushButton_clicked();

private:
    Ui::add_user_Dialog *ui;
};

#endif // ADD_USER_DIALOG_H
