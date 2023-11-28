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
#include "plot/attitude.h"
#include "parsethread.h"

// 增加测试状态。定义了MODE_TEST则为开发板，否则为发行版（从文件读取协议）
#define MODE_TEST

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
    QTableWidget  *showTableWidget;

    // 表格
    ComboboxDelegate comboboxDelegate; // // 下拉框代理
    void addRow(int curRow, QString name="<name>", EnumClass::typeListEnum type=EnumClass::t_char, QString data="", Qt::CheckState accumCheck=Qt::Unchecked, Qt::CheckState curve1Checked=Qt::Unchecked, Qt::CheckState curve2Checked=Qt::Unchecked, Qt::CheckState curve3Checked=Qt::Unchecked); // 表格新增一行

    void switchTable(bool formatPage=true); // 切换表格的显示, framePage=true 显示协议编辑框，=false显示数据框

    // 数据协议
    void frameFormat(); // 协议格式化，从table获取数据转为vector
    bool frameChecked; // 数据协议确认
    bool frameLittleEndian; // =true 小端字节序, =false 大端
    quint32 frameHz; // 频率

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
    void parseOneFrame(QByteArray &recvBuf); // 解析一帧数据


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
    Attitude *attitude = nullptr;

    // 解析文件线程
    ParseThread *parseThread;

private slots:
    // ---- 数据协议 ----

    // 新增一行
    void on_addRowBtn_clicked();
    // 删除选定行
    void on_delRowBtn_clicked();
    // 添加帧头
    void on_addHeaderBtn_clicked();
    // 添加校验和
    void on_addCheckSumBtn_clicked();
    // 保存协议
    void on_saveFrameBtn_clicked();
    // 加载协议
    void on_loadFrameBtn_clicked();
    // 从文件中读取协议，确认数据帧
    void loadFrameByFile();
    // 清空表格
    void clearTable();
    // 确认数据协议
    void on_confirmFrameBtn_clicked(bool checked);
    // 使能/失能 协议按钮
    void enableFrameBtn(bool state);

    // 打开和关闭串口
    void on_openPortBtn_clicked(bool checked);

    // 串口打开时的信号变化
    void slot_serialPortOpenState(bool checked);
    // 串口关闭时的信号变化
    void slot_serialPortCloseState(bool checked);

    // 任务调度：数据处理、绘图
    void slot_taskScheduler();

    // 显示绘图界面按钮
    void on_showPlotBtn_clicked();

    void on_showAttitudeBtn_clicked();
    // 发送命令
    void on_sendCommandBtn_clicked();
    // 解析数据
    void on_parseDataBtn_clicked();

    // 解析线程进度条和通知
    void slot_parseProgress(int i);
    void slot_parseMsg(QString msg);

signals:
    // 打开串口信号
    void signal_openSerialPort(QString portName, int baudrate, qint8 dataBits, qint8 stopBits, qint8 parity);
    // 关闭串口信号
    void signal_closeSerialPort();
};

#endif // MAINWINDOW_H
