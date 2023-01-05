#include "serialport.h"

// 串口类

SerialPort::SerialPort(QObject *parent) : QObject(parent){

}

SerialPort::~SerialPort(){

}

// 初始化串口对象
void SerialPort::init(){
    qSerialPort = new QSerialPort(this);
    qSerialPort->setFlowControl(QSerialPort::NoFlowControl); // 无流控

    /*
    // 设置1ms定时器
    qSerialPortTimer = new QTimer(this);
    qSerialPortTimer->setInterval(1);
    connect(qSerialPortTimer, &QTimer::timeout, this, xxxx);
    */
}

// 更新端口列表
void SerialPort::updatePortList(){
    QStringList portList;
    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo& info, infoList){
        portList << info.portName();
    }
    emit getPortList(portList);
}
