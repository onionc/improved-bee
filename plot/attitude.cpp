#include "attitude.h"
#include "ui_attitude.h"

Attitude::Attitude(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Attitude)
{
    ui->setupUi(this);

    // 标题
    setWindowTitle("姿态角显示");

}

Attitude::~Attitude()
{
    delete ui;
}
