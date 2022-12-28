#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // 表格设置
    QStringList headerText;
    headerText<<"名称"<<"数据类型"<<"数值"<<"图1"<<"图2"<<"图3";
    ui->tableWidget->setColumnCount(headerText.count()); // 列数
    ui->tableWidget->setHorizontalHeaderLabels(headerText); // 设置水平表头数据
    ui->tableWidget->horizontalHeader()->setVisible(true); // 水平表头有效
    ui->tableWidget->verticalHeader()->setVisible(true); // 竖直表头有效
    ui->tableWidget->setColumnWidth(colName, 140); // 列宽
    ui->tableWidget->setColumnWidth(colType, 140); // 列宽
    ui->tableWidget->setColumnWidth(colData, 140); // 列宽
    ui->tableWidget->setColumnWidth(colCurve1, 40); // 列宽
    ui->tableWidget->setColumnWidth(colCurve2, 40); // 列宽
    ui->tableWidget->setColumnWidth(colCurve3, 40); // 列宽
    ui->tableWidget->setAlternatingRowColors(true); // 隔行换色
    ui->tableWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn); // 竖直滚动条
    ui->tableWidget->setStyleSheet("QTableWidget::item:selected{background:lightblue}"); // 选定为淡蓝色
    ui->tableWidget->setItemDelegateForColumn(colType,&comboboxDelegate);   //委托

    ui->tableWidget->insertRow(0); // 插入一行
    addRow(0, FRAME_HEADER, "char", "0xAA");



}

MainWindow::~MainWindow()
{
    delete ui;
}


// 表格新增一行
void MainWindow::addRow(int curRow, QString name, QString type, QString data){
    QTableWidgetItem *item;

    int typeValue = 1000; // type>=1000 自定义类型

    // 名称
    item = new QTableWidgetItem(name, typeValue++);
    ui->tableWidget->setItem(curRow, colName, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CEHCK){
        // 特殊项名称不可编辑
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    }else{
        // 可选定
        item->setCheckState(Qt::Checked);
    }

    // 数据类型
    item = new QTableWidgetItem(type, typeValue++);
    ui->tableWidget->setItem(curRow, colType, item);
    if(name == FRAME_HEADER || name == FRAME_END){
        // 特殊项类型不可编辑
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    }


    // 数据
    item = new QTableWidgetItem(data, typeValue++);
    ui->tableWidget->setItem(curRow, colData, item);

    // 绘图1
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colCurve1, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CEHCK){
        // 特殊项不可选择绘图
        item->setFlags(item->flags() & (Qt::NoItemFlags));
    }else{
        // 默认不选定
        item->setCheckState(Qt::Unchecked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

    // 绘图2
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colCurve2, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CEHCK){
        item->setFlags(item->flags() & (Qt::NoItemFlags));
    }else{
        item->setCheckState(Qt::Unchecked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

    // 绘图3
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colCurve3, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CEHCK){
        item->setFlags(item->flags() & (Qt::NoItemFlags));
    }else{
        item->setCheckState(Qt::Unchecked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

}

