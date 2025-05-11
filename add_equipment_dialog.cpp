#include "add_equipment_dialog.h"
#include "ui_add_equipment_dialog.h"

Add_Equipment_Dialog::Add_Equipment_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Add_Equipment_Dialog)
{
    ui->setupUi(this);
}

Add_Equipment_Dialog::~Add_Equipment_Dialog()
{
    delete ui;
}

void Add_Equipment_Dialog::on_Send_pushButton_clicked()
{
    emit sendData(ui->Ip_lineEdit->text(),ui->neme_lineEdit->text());
    Add_Equipment_Dialog::close();
}
