#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include <QLabel>
#include <QTimer>
#include <QVector>
#include <QComboBox>
#include <QTableWidget>
#include "ui/comboboxdelegate.h"
#include "ui/tablewidgetdrag.h"
#include "serialPort/serialport.h"
#include "serialPort/serialcombox.h"
#include "parse.h"
#include "datatype.h"
#include "plot/plot.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    // 表格
    ComboboxDelegate comboboxDelegate; // // 下拉框代理
    void addRow(int curRow, QString name="<name>", EnumClass::typeListEnum type=EnumClass::t_char, QString data="", Qt::CheckState accumCheck=Qt::Unchecked, Qt::CheckState curve1Checked=Qt::Unchecked, Qt::CheckState curve2Checked=Qt::Unchecked, Qt::CheckState curve3Checked=Qt::Unchecked); // 表格新增一行

    void switchTable(bool formatPage=true); // 切换表格的显示, framePage=true 显示协议编辑框，=false显示数据框

    // 数据协议
    bool frameChecked; // 数据协议确认
    bool frameLittleEndian; // =true 小端字节序, =false 大端
    quint32 frameHz; // 频率

    // 从文件中读取协议，确认数据帧
    void loadFrameByFile();

    // 串口
    bool isOpenSerialPort; // 串口是否打开
    QThread *serialPortThread = nullptr; // 串口线程
    SerialPort* serialPort = nullptr; // 串口对象

    // 定时器
    QTimer *runTimer = nullptr;

    // 解析
    Parse parse;
    QVector<SProperty> frameData; // 协议
    QByteArray recvBuf; // 接收的字节数据
    QVector<NAV_Data> oneSecData; // 1s的数据
    QVector<NAV_Data> tenSecData; // 10s的数据
    quint64 dataCount; // 计数


    // 存储文件
    bool bSaveRawFlag; // 原始数据标志
    bool bSave1sFlag; // 1s数据
    bool bSave10sFlag; // 10数据
    util::WriteFile fRawFile, fNavDataFile; // raw存储原始数据，navData存储解析后数据
    util::WriteFile f1sFile;
    util::WriteFile f10sFile;

    // 日志

    void LogShow(QString info, bool format=true); // 显示日志

    // 图形显示
    Plot *plot = nullptr;

private slots:
    // ---- 数据协议 ----

    // 打开和关闭串口
    void on_openPortBtn_clicked(bool checked);

    // 串口打开时的信号变化
    void slot_serialPortOpenState(bool checked);
    // 串口关闭时的信号变化
    void slot_serialPortCloseState(bool checked);

    // 任务调度：数据处理、绘图
    void slot_taskScheduler();

signals:
    // 打开串口信号
    void signal_openSerialPort(QString portName, int baudrate, qint8 dataBits, qint8 stopBits, qint8 parity);
    // 关闭串口信号
    void signal_closeSerialPort();
};

#endif // MAINWINDOW_H
