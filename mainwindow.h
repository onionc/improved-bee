#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "comboboxdelegate.h"
#include "tablewidgetdrag.h"
#include <QDebug>

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
    void addRow(int curRow, QString name="<name>", QString type="char", QString data=""); // 表格新增一行

    // 数据协议
    const char* FRAME_HEADER = "Frame_Header";
    const char* FRAME_END = "Frame_Tail";
    const char* FRAME_CEHCK = "Check_Sum";


private slots:
    // 数据协议

    // 新增一行
    void on_addRowBtn_clicked();
    // 删除选定行
    void on_delRowBtn_clicked();
};

#endif // MAINWINDOW_H
