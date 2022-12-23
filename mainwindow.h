#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    enum FieldColumn{colName=0, colType, colData, colCurve1, colCurve2, colCurve3};
    void addRow(int curRow, QString name="title", QString type="char", QString data=""); // 添加一行

    // 协议
    const char* FRAME_HEADER = "Frame_Header";
    const char* FRAME_END = "Frame_Tail";
    const char* FRAME_CEHCK = "Check_Sum";

};

#endif // MAINWINDOW_H
