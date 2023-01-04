#include "serialcombox.h"

SerialComBox::SerialComBox(QWidget *parent) : QComboBox(parent){

}
SerialComBox::~SerialComBox(){}

// 更新项目
void SerialComBox::updateItem(QStringList comPorts){
    this->clear();
    this->addItems(comPorts);
    this->showPopup();
}

// 鼠标按下
void SerialComBox::mousePressEvent(QMouseEvent *event){

    if(event->button() == Qt::LeftButton){
        emit detectComPorts();
    }
    QComboBox::mousePressEvent(event);
}
