#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include "util.h"

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
    ui->tableWidget->setItemDelegateForColumn(colType,&comboboxDelegate);   //委托

    ui->tableWidget->setDragEnabled(true); // 开启拖拽
    ui->tableWidget->setDragDropMode(QAbstractItemView::InternalMove );  // 拖拽，内部移动
    ui->tableWidget->setDropIndicatorShown(true); // 拖拽时提示
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // 选定一行

    // 协议
    on_addHeaderBtn_clicked(); // 插入一行帧头
    ui->confirmFrameBtn->setCheckable(true);
    ui->confirmFrameBtn->setChecked(false);

    // 日志
    util::logInit();



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
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
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
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        // 特殊项不可选择绘图
        item->setFlags(Qt::NoItemFlags);
    }else{
        // 默认不选定
        item->setCheckState(Qt::Unchecked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

    // 绘图2
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colCurve2, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        item->setFlags(Qt::NoItemFlags);
    }else{
        item->setCheckState(Qt::Unchecked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

    // 绘图3
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colCurve3, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        item->setFlags(Qt::NoItemFlags);
    }else{
        item->setCheckState(Qt::Unchecked);
        item->setFlags(item->flags()  & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

}


// ----------------------- 数据协议 -----------------------
// 增加一行
void MainWindow::on_addRowBtn_clicked()
{
    int curRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(curRow);
    QString name  = QString("name_%1").arg(curRow+1);
    addRow(curRow, name);

    // 选定新行
    ui->tableWidget->selectRow(curRow);
}

// 删除选定行
void MainWindow::on_delRowBtn_clicked()
{
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());

    ui->tableWidget->selectRow(ui->tableWidget->currentRow());
}

// 添加帧头
void MainWindow::on_addHeaderBtn_clicked()
{
    int curRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(curRow);

    addRow(curRow, FRAME_HEADER, "uchar", "0xAA");

    // 选定新行
    ui->tableWidget->selectRow(curRow);
}

// 添加校验和
void MainWindow::on_addCheckSumBtn_clicked()
{
    int curRow = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(curRow);

    addRow(curRow, FRAME_CHECK, "uchar", "");

    // 选定新行
    ui->tableWidget->selectRow(curRow);
}

// 保存协议
void MainWindow::on_saveFrameBtn_clicked()
{
    // 创建目录
    QString dir = util::createDir("config");

    QString filter = "协议文件(*.ini);;所有文件(*.*)";
    QString saveFilename = QFileDialog::getSaveFileName(this, " 另存为", dir, filter);

    if(saveFilename.isEmpty()){
        QMessageBox::warning(this, "保存失败", "文件名为空");
        return;
    }

    // 保存数据到ini文件
    QSettings write(saveFilename, QSettings::IniFormat);
    write.setIniCodec(QTextCodec::codecForName("UTF-8"));
    write.clear();
    for(int i=0; i<ui->tableWidget->rowCount(); i++) {
        write.setValue(QString("%1/name").arg(i), ui->tableWidget->item(i, colName)->text());
        write.setValue(QString("%1/type").arg(i), ui->tableWidget->item(i, colType)->text());
        write.setValue(QString("%1/data").arg(i), ui->tableWidget->item(i, colData)->text());
    }
}

// 清除表格数据
void MainWindow::clearTable(){
    int i = ui->tableWidget->rowCount()-1;
    for(; i>=0; i--)
    {
        ui->tableWidget->removeRow(i);
    }
}

// 加载协议
void MainWindow::on_loadFrameBtn_clicked()
{
    // 创建目录
    QString dir = util::createDir("config");

    QString filter = "协议文件(*.ini);;所有文件(*.*)";
    QString loadFilename = QFileDialog::getOpenFileName(this, " 加载文件", dir, filter);
    if(loadFilename.isEmpty()){
        QMessageBox::warning(this, "打开失败", "文件名为空");
        return;
    }

    // 清除
    clearTable();

    // 打开ini文件
    QSettings read(loadFilename, QSettings::IniFormat);
    QStringList allKeys = read.childGroups();
    QString name, type, data;
    int curRow = ui->tableWidget->rowCount();
    foreach(QString key, allKeys){
        ui->tableWidget->insertRow(curRow);

        name = read.value(QString("%1/name").arg(key)).toString();
        type = read.value(QString("%1/type").arg(key)).toString();
        data = read.value(QString("%1/data").arg(key)).toString();
        addRow(curRow, name, type, data);

        curRow++;
    }

}

// 确认数据帧
void MainWindow::on_confirmFrameBtn_clicked(bool checked)
{
    if(checked){
        ui->confirmFrameBtn->setText("编辑数据协议");
        // 失能表格；失能按键
        ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
        enableFrameBtn(false);

    }else{
        ui->confirmFrameBtn->setText("确认数据协议");
        // 使能表格；使能按键
        enableFrameBtn(true);
        ui->tableWidget->setEditTriggers(QAbstractItemView::SelectedClicked | QAbstractItemView::DoubleClicked);

    }
}

// 使能/失能 协议按钮
void MainWindow::enableFrameBtn(bool state){
    QList<QPushButton *> pushButtons = ui->page->findChildren<QPushButton *>();
    foreach(QPushButton *a, pushButtons){
        if(a==ui->confirmFrameBtn){
            continue;
        }
        a->setEnabled(state);
    }
}
