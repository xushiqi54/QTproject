#ifndef SHOWDEVICEDATA_H
#define SHOWDEVICEDATA_H

#include <QDialog>

namespace Ui {
class ShowDeviceData;
}

class ShowDeviceData : public QDialog
{
    Q_OBJECT

public:
    explicit ShowDeviceData(QWidget *parent = nullptr);
    ~ShowDeviceData();

private:
    Ui::ShowDeviceData *ui;
};

#endif // SHOWDEVICEDATA_H
