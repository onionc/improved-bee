#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextCodec>
#include <QThread>
#include <QLabel>
#include "ui/comboboxdelegate.h"
#include "ui/tablewidgetdrag.h"
#include "serialPort/serialport.h"
#include "serialPort/serialcombox.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:


private:
    Ui::MainWindow *ui;

    // 表格
    ComboboxDelegate comboboxDelegate; // // 下拉框代理
    enum FieldColumn{colName=0, colType, colData, colCurve1, colCurve2, colCurve3};
    void addRow(int curRow, QString name="<name>", QString type="char", QString data="", Qt::CheckState checked=Qt::Checked, Qt::CheckState curve1Checked=Qt::Unchecked, Qt::CheckState curve2Checked=Qt::Unchecked, Qt::CheckState curve3Checked=Qt::Unchecked); // 表格新增一行

    // 数据协议
    const char* FRAME_HEADER = "Frame_Header";
    const char* FRAME_END = "Frame_Tail"; // 帧尾暂时没用到
    const char* FRAME_CHECK = "Check_Sum";

    // 串口
    bool isOpenSerialPort; // 串口是否打开
    QThread *serialPortThread = nullptr; // 串口线程
    SerialPort* serialPort = nullptr; // 串口对象



private slots:
    // 数据协议

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
    // 清空表格
    void clearTable();
    // 确认数据协议
    void on_confirmFrameBtn_clicked(bool checked);
    // 使能/失能 协议按钮
    void enableFrameBtn(bool state);
};

#endif // MAINWINDOW_H
