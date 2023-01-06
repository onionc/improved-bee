#include "serialport.h"

// 串口类

QReadWriteLock rwLock; // 读写锁

SerialPort::SerialPort(QObject *parent) : QObject(parent){
    qSerialPort = new QSerialPort(this);
    qSerialPort->setFlowControl(QSerialPort::NoFlowControl); // 无流控
}

SerialPort::~SerialPort(){
    if(qSerialPort && qSerialPort->isOpen()){
        qSerialPort->close();
        delete qSerialPort;
    }
}

// 初始化串口对象
void SerialPort::init(){
    connect(qSerialPort, &QSerialPort::readyRead, this, &SerialPort::slot_recvSerialPortData); // 接收数据
}

// 更新端口列表
void SerialPort::updatePortList(){
    QStringList portList;
    QList<QSerialPortInfo> infoList = QSerialPortInfo::availablePorts();
    foreach(const QSerialPortInfo& info, infoList){
        portList << info.portName();
    }
    emit signal_getPortList(portList);
}

// 打开串口
void SerialPort::slot_openSerialPort(QString portName, int baudrate, qint8 dataBits, qint8 stopBits, qint8 parity){
    qSerialPort->setPortName(portName);
    qSerialPort->setBaudRate(baudrate);

    switch(dataBits){
        case 5: qSerialPort->setDataBits(QSerialPort::Data5); break;
        case 6: qSerialPort->setDataBits(QSerialPort::Data6); break;
        case 7: qSerialPort->setDataBits(QSerialPort::Data7); break;
        case 8:
        default:
            qSerialPort->setDataBits(QSerialPort::Data8);
            break;
    }

    switch(stopBits){
        case 3:
            qSerialPort->setStopBits(QSerialPort::TwoStop);
            break;
        case 2:
            qSerialPort->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        case 1:
        default:
            qSerialPort->setStopBits(QSerialPort::OneStop);
            break;
    }

    switch(parity){
        case 2: qSerialPort->setParity(QSerialPort::EvenParity); break;
        case 3: qSerialPort->setParity(QSerialPort::OddParity); break;
        case 4: qSerialPort->setParity(QSerialPort::SpaceParity); break;
        case 5: qSerialPort->setParity(QSerialPort::MarkParity); break;
        case 0:
        default:
            qSerialPort->setParity(QSerialPort::NoParity);
            break;
    }

    // 打开发送信号
    if(qSerialPort->open(QIODevice::ReadWrite)){
        emit signal_serialPortOpenState(true);
    }else{
        emit signal_serialPortOpenState(false);
    }


}
// 关闭串口
void SerialPort::slot_closeSerialPort(){
    qSerialPort->close();

    if(qSerialPort->isOpen()){
        emit signal_serialPortCloseState(false);
    }else{
        emit signal_serialPortCloseState(true);
    }
}

// 接收数据
void SerialPort::slot_recvSerialPortData(){
    qint64 recvLen = qSerialPort->bytesAvailable();
    if(recvLen<1) return;

    QByteArray t = qSerialPort->readAll();
    rwLock.lockForWrite();
    recvBuf.append(t);
    rwLock.unlock();

    qDebug()<<"buf:"<<recvBuf.size();
}

