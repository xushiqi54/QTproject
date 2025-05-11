#include "showdevicedata.h"
#include "ui_showdevicedata.h"

ShowDeviceData::ShowDeviceData(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowDeviceData)
{
    ui->setupUi(this);
}

ShowDeviceData::~ShowDeviceData()
{
    delete ui;
}
