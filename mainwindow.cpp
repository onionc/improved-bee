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

    // toolBox控制界面设置
    ui->panelBox->setCurrentIndex(0); // 默认第一页

    // 协议
    on_addHeaderBtn_clicked(); // 插入一行帧头
    ui->confirmFrameBtn->setCheckable(true);
    ui->confirmFrameBtn->setChecked(false);

    // 日志
    util::logInit();

    // 串口设置
    QStringList baudrate, dataBit, stopBit, parity;
    baudrate<<"4800"<<"9600"<<"19200"<<"115200"<<"230400"<<"921600";
    ui->baudrate_comboBox->addItems(baudrate);
    ui->baudrate_comboBox->setCurrentText("115200");
    dataBit<<"5"<<"6"<<"7"<<"8";
    ui->dataBits_comboBox->addItems(dataBit);
    ui->dataBits_comboBox->setCurrentText("8");
    stopBit<<"1"<<"1.5"<<"2";
    ui->stopBit_comboBox->addItems(stopBit);
    ui->stopBit_comboBox->setCurrentText("1");
    parity<<"无"<<"奇校验"<<"偶校验"<<"Space"<<"Mask";
    ui->parity_comboBox->addItems(parity);
    ui->parity_comboBox->setCurrentText(0);


    // 状态设置
    isOpenSerialPort = false; // 串口状态 关

    // 串口线程创建
    serialPortThread = new QThread();
    serialPort = new SerialPort();
    serialPort->moveToThread(serialPortThread); // 串口和线程关联
    serialPortThread->start(); // 开启线程

    connect(serialPortThread, &QThread::started, serialPort, &SerialPort::init); // 线程准备好后，初始化串口对象
    connect(ui->comPort_comboBox, &SerialComBox::detectComPorts, serialPort, &SerialPort::updatePortList); // 下拉的监测端口信号和更新界面绑定
    connect(serialPort,&SerialPort::signal_getPortList,ui->comPort_comboBox,&SerialComBox::updateItem);  //检测后更新串口号

    connect(this, &MainWindow::signal_openSerialPort, serialPort, &SerialPort::slot_openSerialPort); // 打开串口
    connect(this, &MainWindow::signal_closeSerialPort, serialPort, &SerialPort::slot_closeSerialPort); // 关闭串口

    connect(serialPort, &SerialPort::signal_serialPortOpenState, this, &MainWindow::slot_serialPortOpenState); // 串口打开时状态
    connect(serialPort, &SerialPort::signal_serialPortCloseState, this, &MainWindow::slot_serialPortCloseState); // 串口关闭时状态

    // 主线程定时器, 任务调度 10ms
    runTimer = new QTimer(this);
    runTimer->setInterval(1000);  //10ms
    connect(runTimer,&QTimer::timeout,this,&MainWindow::slot_taskScheduler);



    // 图像相关
    ui->led->setSize(15);
    ui->led->setColor(0); // 设定LED为灰色

}

MainWindow::~MainWindow()
{
    // 结束子线程
    serialPortThread->quit();
    if(serialPortThread->wait(5) == false){
        serialPortThread->terminate();
    }
    delete serialPortThread;
    delete serialPort;

    delete ui;
}


// 表格新增一行
void MainWindow::addRow(int curRow, QString name, QString type, QString data, Qt::CheckState checked, Qt::CheckState curve1Checked, Qt::CheckState curve2Checked, Qt::CheckState curve3Checked){
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
        item->setCheckState(checked);
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
        item->setCheckState(curve1Checked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

    // 绘图2
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colCurve2, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        item->setFlags(Qt::NoItemFlags);
    }else{
        item->setCheckState(curve1Checked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

    // 绘图3
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colCurve3, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        item->setFlags(Qt::NoItemFlags);
    }else{
        item->setCheckState(curve1Checked);
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
        // 是否选定
        write.setValue(QString("%1/checked").arg(i), ui->tableWidget->item(i, colName)->checkState()==Qt::Checked?1:0);

        // 名称、类型、数据
        write.setValue(QString("%1/name").arg(i), ui->tableWidget->item(i, colName)->text());
        write.setValue(QString("%1/type").arg(i), ui->tableWidget->item(i, colType)->text());
        write.setValue(QString("%1/data").arg(i), ui->tableWidget->item(i, colData)->text());

        // 图表绘图项是否选定
        write.setValue(QString("%1/curve1").arg(i), ui->tableWidget->item(i, colCurve1)->checkState()==Qt::Checked);
        write.setValue(QString("%1/curve2").arg(i), ui->tableWidget->item(i, colCurve2)->checkState()==Qt::Checked);
        write.setValue(QString("%1/curve3").arg(i), ui->tableWidget->item(i, colCurve3)->checkState()==Qt::Checked);
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
    QStringList allGroups = read.childGroups();
    QString name, type, data;
    int curRow = ui->tableWidget->rowCount();
    // 选定状态：行选定（在第一个item下），图1，图2，图3
    Qt::CheckState line, curve1, curve2, curve3;
    foreach(QString groupKey, allGroups){
        ui->tableWidget->insertRow(curRow);

        name = read.value(QString("%1/name").arg(groupKey)).toString();
        type = read.value(QString("%1/type").arg(groupKey)).toString();
        data = read.value(QString("%1/data").arg(groupKey)).toString();

        line = read.value(QString("%1/checked").arg(groupKey)).toBool() ? Qt::Checked : Qt::Unchecked;
        curve1 = read.value(QString("%1/curve1").arg(groupKey)).toBool() ? Qt::Checked : Qt::Unchecked;
        curve2 = read.value(QString("%1/curve2").arg(groupKey)).toBool() ? Qt::Checked : Qt::Unchecked;
        curve3 = read.value(QString("%1/curve3").arg(groupKey)).toBool() ? Qt::Checked : Qt::Unchecked;

        addRow(curRow, name, type, data, line, curve1, curve2, curve3);

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

// 使能/失能 协议编辑按钮
void MainWindow::enableFrameBtn(bool state){
    QList<QPushButton *> pushButtons = ui->page->findChildren<QPushButton *>();
    foreach(QPushButton *a, pushButtons){
        // 仅保存和确认数据按钮可用
        if(a==ui->confirmFrameBtn || a==ui->saveFrameBtn){
            continue;
        }
        a->setEnabled(state);
    }
}


// ----------------------------- 串口 ----------------------------
// 打开和关闭串口
void MainWindow::on_openPortBtn_clicked(bool checked)
{

    if(checked){
        // 打开串口

        // 串口名称，波特率，数据位，停止位，校验
        QString name     = ui->comPort_comboBox->currentText();
        qint32  baudrate = ui->baudrate_comboBox->currentText().toInt();
        quint8   dataBits = ui->dataBits_comboBox->currentText().toInt();
        quint8   stopBits = ui->stopBit_comboBox->currentIndex();
        quint8   parity   = ui->parity_comboBox->currentIndex();

        switch(stopBits)
        {
            case 1: stopBits = 3; break;  // 1.5个停止位
            case 2: stopBits = 2; break;  // 2个停止位
            case 0:
            default:
                stopBits = 1; // 1个停止位
            break;
        }

        switch(parity)
        {
            case 1: parity = 2; break;   //偶校验
            case 2: parity = 3; break;   //奇校验
            case 3: parity = 4; break; // space
            case 4: parity = 5; break; // mark
            case 0:
            default:
                parity = 0; //无校验
                break;
        }

        emit signal_openSerialPort(name, baudrate, dataBits, stopBits, parity);

    }
    else{
        //关闭串口

        emit signal_closeSerialPort();
    }

}



// 串口打开时的信号变化
void MainWindow::slot_serialPortOpenState(bool checked){
    if(checked){
        enableFrameBtn(false); // 失能协议编辑按钮
        ui->led->setColor(2); // 点亮led（绿色）

        ui->openPortBtn->setText("断开串口");

        runTimer->start(); // 任务开始执行

    }else{
        enableFrameBtn(true);
        ui->led->setColor(1); // 红色

        ui->openPortBtn->setChecked(false);
        QMessageBox::critical(this, "失败", "串口打开失败");
    }
}

// 串口关闭时的信号变化
void MainWindow::slot_serialPortCloseState(bool checked){
    if(checked){
        enableFrameBtn(true);
        ui->led->setColor(0); // 灰色
        ui->openPortBtn->setText("连接串口");

        runTimer->stop(); // 任务停止执行
    }else{
        enableFrameBtn(false);
        ui->led->setColor(2); // 红色

        ui->openPortBtn->setChecked(true);
        QMessageBox::critical(this, "失败", "串口关闭失败");
    }

}


// 任务调度：数据处理、绘图
void MainWindow::slot_taskScheduler(){
    // 处理数据
    if(rwLock.tryLockForWrite(3)){
        qDebug()<<"buf2:"<<serialPort->recvBuf.size();
        serialPort->recvBuf.remove(1,3);

        rwLock.unlock();
    }

}
