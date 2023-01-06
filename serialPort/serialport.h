#ifndef SERIALPORT_H
#define SERIALPORT_H

// 串口类

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QByteArray>
#include <QTimer>
#include <QDebug>
#include <QReadWriteLock>

extern QReadWriteLock rwLock; // 读写锁

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


public:
    // 数据缓冲区
    QByteArray recvBuf;

signals:
    // 获取端口列表信号
    void signal_getPortList(QStringList ports);

    // 串口打开时的信号
    void signal_serialPortOpenState(bool checked);
    // 串口关闭时的信号
    void signal_serialPortCloseState(bool checked);

public slots:
    // 更新端口列表
    void updatePortList();

    /**
     * @brief openSerialPort 打开串口
     * @param portName 串口名
     * @param baudrate 波特率
     * @param dataBits 数据位：5 6 7 8
     * @param stopBits 停止位：1-1个停止位，2-2个停止位，3-1.5个停止位
     * @param parity 校验位:0-无校验，2-偶校验，3-奇检验，4-space校验，5-mark校验
     */
    void slot_openSerialPort(QString portName, int baudrate, qint8 dataBits, qint8 stopBits, qint8 parity);
    // 关闭串口
    void slot_closeSerialPort();
    // 接收数据
    void slot_recvSerialPortData();

};

#endif // SERIALPORT_H
