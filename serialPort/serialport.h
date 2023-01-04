#ifndef SERIALPORT_H
#define SERIALPORT_H

// 串口类

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QTimer>


class SerialPort : public QObject
{
    Q_OBJECT
public:
    explicit SerialPort(QObject *parent = nullptr);
    ~SerialPort();

    //串口对象
    QSerialPort* qSerialPort = nullptr;
    // 定时器
    QTimer* qSerialPortTimer = nullptr;

    // 初始化串口对象
    void init();


private:
    // 数据缓冲区
    QByteArray recvBuf;

signals:
    // 获取端口列表
    void getPortList(QStringList ports);

public slots:
    // 更新端口列表
    void updatePortList();

};

#endif // SERIALPORT_H
