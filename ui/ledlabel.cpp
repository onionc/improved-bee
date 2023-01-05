#include "ledlabel.h"

LedLabel::LedLabel(QWidget* parent): QLabel(parent)
{

    size = 10;
    color = 0;
}



void LedLabel::setSize(unsigned short size){
    this->size = size;
    setStyle();
}

void LedLabel::setColor(int color){
    this->color = color;
    setStyle();
}

void LedLabel::setStyle(){
    this->setText("");
    // 背景色
    QString backgroundColor("background-color:");
    switch(color){
        case 1:
            // red
            backgroundColor += "#AA2222";
            break;
        case 2:
            // green
            backgroundColor += "#339933";
            break;
        case 0:
        default:
            // 浅灰
            backgroundColor += "#d0d0d0";
            break;
    }

    // 设置大小；圆角；背景颜色；
    QString style = QString("min-width:%1px;min-height:%1px;max-width:%1px;max-height:%1px;border-radius:%2px;%3;").arg(size).arg(size/2).arg(backgroundColor);
    this->setStyleSheet(style);
}
