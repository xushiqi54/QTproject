#include "add_user_dialog.h"
#include "ui_add_user_dialog.h"

add_user_Dialog::add_user_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::add_user_Dialog)
{
    ui->setupUi(this);
}

add_user_Dialog::~add_user_Dialog()
{
    delete ui;
}


void add_user_Dialog::on_Send_user_pushButton_clicked()
{
    emit sendUser(ui->user_name_lineEdit->text(),ui->user_password_lineEdit->text(),ui->user_power_lineEdit->text());
    add_user_Dialog::close();
}
