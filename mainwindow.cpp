#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QComboBox>
#include "util.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);



    // 显示数据的表格设置
    ui->tableWidget->setColumnCount(1); // 列数
    ui->tableWidget->horizontalHeader()->setVisible(false); // 水平表头有效
    ui->tableWidget->verticalHeader()->setVisible(true); // 竖直表头有效
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //ui->tableWidget->show();



    // 日志
    util::logInit();

    // todo del
    frameLittleEndian = true; // 默认字节序：小端
    frameHz = DEFAULT_HZ; // 默认频率：200Hz

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
    ui->parity_comboBox->setCurrentIndex(0);




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

    // 主线程定时器, 任务调度 100ms
    runTimer = new QTimer(this);
    runTimer->setInterval(100);  //100ms
    connect(runTimer,&QTimer::timeout,this,&MainWindow::slot_taskScheduler);



    // 图形显示界面
    qplotArr.push_back(ui->mPlot1);
    plot = new Plot(&qplotArr);

    // 协议未确认
    frameChecked = false;

    // 文件
    bSaveRawFlag = bSave1sFlag = true;
    bSave10sFlag = false;

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
void MainWindow::addRow(int curRow, QString name, EnumClass::typeListEnum type, QString data, Qt::CheckState accumCheck, Qt::CheckState curve1Checked, Qt::CheckState curve2Checked, Qt::CheckState curve3Checked){
    QTableWidgetItem *item;

    int typeValue = 1000; // type>=1000 自定义类型

    // 名称
    item = new QTableWidgetItem(name, typeValue++);
    ui->tableWidget->setItem(curRow, colName, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        // 特殊项名称不可编辑
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    }

    // 数据类型

    QString typeStr = typeList[type];
    if(name == FRAME_CHECK){
        typeStr = "--"; // 校验无数据类型，长度通过具体校验类型确认
    }
    item = new QTableWidgetItem(typeStr, typeValue++);
    ui->tableWidget->setItem(curRow, colType, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        // 特殊项类型不可编辑
        item->setFlags(item->flags() & (~Qt::ItemIsEditable));
    }


    // 数据
    if(name == FRAME_HEADER){
        // 帧头，默认lineEdit
        item = new QTableWidgetItem(data, typeValue++);
        ui->tableWidget->setItem(curRow, colData, item);
    }else if(name == FRAME_CHECK){
        // 校验，下拉选择
        QComboBox *classBox = new QComboBox;
        classBox->addItems(checkSumList);
        classBox->setCurrentText(data);
        ui->tableWidget->setCellWidget(curRow, colData, classBox);
    }else{
        // 多行输入框，用来输入数据或LUA脚本
        QTextEdit *edit = new QTextEdit;
        edit->setSizeIncrement(100, 50);
        edit->setText(data);
        ui->tableWidget->setCellWidget(curRow, colData, edit);
    }

    // 累加值选择框
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colAccumCheck, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        // 特殊项不可选择绘图
        item->setFlags(Qt::NoItemFlags);
    }else{
        // 默认不选定
        item->setCheckState(accumCheck);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }


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
        item->setCheckState(curve2Checked);
        item->setFlags(item->flags() & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

    // 绘图3
    item = new QTableWidgetItem("", typeValue++);
    ui->tableWidget->setItem(curRow, colCurve3, item);
    if(name == FRAME_HEADER || name == FRAME_END || name == FRAME_CHECK){
        item->setFlags(Qt::NoItemFlags);
    }else{
        item->setCheckState(curve3Checked);
        item->setFlags(item->flags()  & ~Qt::ItemIsEditable & ~Qt::ItemIsSelectable);
    }

}


// ----------------------- 数据协议 -----------------------
// 从文件中读取协议，确认数据帧
void MainWindow::loadFrameByFile()
{

    QString errorMsg;

    // 从ini文件读取协议
    QString loadFilename = "data.obj";
    /* 从目录中选择文件，这里不需要，直接从固定文件读取
    // 创建目录
    QString dir = util::createDir("config");

    QString filter = "协议文件(*.ini);;所有文件(*.*)";
    QString loadFilename = QFileDialog::getOpenFileName(this, " 加载文件", dir, filter);
    if(loadFilename.isEmpty()){
        QMessageBox::warning(this, "打开失败", "文件名为空");
        return;
    }
    */
    parse.loadFromIni(loadFilename, &frameData, frameLittleEndian, frameHz);
    // 协议确认
    std::vector<std::vector<QString>> chartInfoArr;
    if(!(parse.parseFrameInfo(&frameData, errorMsg, chartInfoArr))){
        QMessageBox::critical(this, "error", errorMsg);
        return;
    }


    if(frameHz<1 || frameHz>2000){
        QMessageBox::critical(this, "error", "频率获取失败");
        return;
    }

    util::smallEndian = frameLittleEndian; // 字节序赋值

    // 设置显示数据用的表格
    QStringList headerText;
    for(int i=0; i<parse.getNavData()->size(); i++){
        headerText << parse.getNavData()->value(i).name;
    }
    ui->tableWidget->setRowCount(headerText.count());
    ui->tableWidget->setVerticalHeaderLabels(headerText); // 设置竖直表头数据
    for(int i=0; i<parse.getNavData()->size(); i++){
        ui->tableWidget->setItem(i, 0,new QTableWidgetItem());
    }

    // 初始化图表
    plot->setChartNum(chartInfoArr);

    frameChecked = true;
}

// ----------------------------- 串口 ----------------------------
// 打开和关闭串口
void MainWindow::on_openPortBtn_clicked(bool checked)
{

    if(checked){
        // 打开串口

        // 判断是否确认数据帧
        loadFrameByFile();
        if(!frameChecked){
            QMessageBox::critical(this, "error", "请先确认数据协议");
            ui->openPortBtn->setChecked(false);
            return;
        }

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
        ui->openPortBtn->setText("断开串口");

        recvBuf.clear(); // 清除临时存储的数据
        runTimer->start(); // 任务开始执行


        // 新建目录
        QString path = QString("data/%1").arg(util::getDatetime(false, true));

        // 保存文件
        fRawFile.close();
        fNavDataFile.close();

        if(!fRawFile.open(path+"/data.raw")){
            QMessageBox::warning(this, "error", "文件创建失败");
            return;
        }
        if(!fNavDataFile.open(path+"/navFile.csv")){
            QMessageBox::warning(this, "error", "文件创建失败");
            return;
        }

        // 1s数据
        f1sFile.close();
        if(!f1sFile.open(path+"/oneSecFile.csv")){
            QMessageBox::warning(this, "error", "文件创建失败");
            return;
        }



        LogShow("文件保存目录："+path);

        // 数据重置
        oneSecData.clear();
        tenSecData.clear();
        dataCount = 0;

    }else{

        ui->openPortBtn->setChecked(false);

        QMessageBox::critical(this, "失败", "串口打开失败");
    }
}

// 串口关闭时的信号变化
void MainWindow::slot_serialPortCloseState(bool checked){
    if(checked){

        ui->openPortBtn->setText("连接串口");

        runTimer->stop(); // 任务停止执行

        // 关闭文件保存
        fRawFile.close();
        fNavDataFile.close();
        f1sFile.close();
        f10sFile.close();

    }else{
        ui->openPortBtn->setChecked(true);

        QMessageBox::critical(this, "失败", "串口关闭失败");
    }

}



// 任务调度：数据处理（保存，解析）、绘图
void MainWindow::slot_taskScheduler(){
    // 处理数据

    // 获取锁
    if(rwLock.tryLockForWrite(3)){
        recvBuf += serialPort->recvBuf;
        // 保存原始数据
        if(bSaveRawFlag)
            fRawFile.write(serialPort->recvBuf);
        serialPort->recvBuf.clear();

        rwLock.unlock();
    }else{
        return;
    }

    // 解析数据
    const QVector<NAV_Data> *navData;
    QString ts;
    const NAV_Data *info;
    bool flag1sUpdate=false, flag10sUpdate=false;
    EnumClass::typeListEnum typeTmp;

    // 绘图用变量
   double key[3]={0};
   std::vector<double> data0, data1, data2;


   while(recvBuf.size()>=parse.getFrameLen()){
       data0.clear();
       data1.clear();
       data2.clear();
        // 找并解析一帧数
        if(parse.findFrameAndParse(recvBuf)){
            navData = parse.getNavData();

            // 保存一帧数据
            if(bSaveRawFlag){
                parse.writeFile(fNavDataFile.ostrm, navData);
            }

            if(frameHz<=0) continue;

            // 1s 10s 数据填充
            for(unsigned int j=0; j<navData->size(); j++){
                info = &(navData->at(j));

                // 1s 计数判断
                if(dataCount%frameHz==0){
                    // 每1s第一帧写入基础数据
                    oneSecData.push_back(*info);
                    flag1sUpdate = false;
                }else{
                    flag1sUpdate = true;
                }
                // 10s 计数判断
                if(dataCount%(frameHz*10)==0){
                    tenSecData.push_back(*info);
                    flag1sUpdate = false;
                }else{
                    flag10sUpdate = true;
                }

                // 数据处理
                typeTmp = info->extFuncName.isEmpty() ? info->type : EnumClass::t_double; // 扩展数据使用double类型数据
                switch(typeTmp){
                    case EnumClass::t_char:
                        if(info->accumFlag){
                            // 累加值

                            // 1s 10s 求和
                            if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_char;
                            if(flag10sUpdate) tenSecData[j].data.t_double += info->data.t_char;

                            // 10s有个特殊过程，求之前数据累加和的平均
                            if((dataCount+1)%(frameHz*10)==0) tenSecData[j].data.t_double/=10;
                        }else{
                            // 瞬时值
                            if(flag1sUpdate)  oneSecData[j].data.t_char = info->data.t_char;
                            if(flag10sUpdate) tenSecData[j].data.t_char = info->data.t_char;
                        }

                        break;
                    case EnumClass::t_uchar:
                        if(info->accumFlag){
                            if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_uchar;
                            if(flag10sUpdate) tenSecData[j].data.t_double += info->data.t_uchar;

                            if((dataCount+1)%(frameHz*10)==0) tenSecData[j].data.t_double/=10;
                        }else{
                            if(flag1sUpdate)  oneSecData[j].data.t_uchar = info->data.t_uchar;
                            if(flag10sUpdate) tenSecData[j].data.t_uchar = info->data.t_uchar;
                        }

                        break;
                    case EnumClass::t_short:
                        if(info->accumFlag){
                            if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_short;
                            if(flag10sUpdate) tenSecData[j].data.t_double += info->data.t_short;

                            if((dataCount+1)%(frameHz*10)==0) tenSecData[j].data.t_double/=10;
                        }else{
                            if(flag1sUpdate)  oneSecData[j].data.t_short = info->data.t_short;
                            if(flag10sUpdate) tenSecData[j].data.t_short = info->data.t_short;
                        }
                        break;
                    case EnumClass::t_ushort:
                        if(info->accumFlag){
                            if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_ushort;
                            if(flag10sUpdate) tenSecData[j].data.t_double += info->data.t_ushort;

                            if((dataCount+1)%(frameHz*10)==0) tenSecData[j].data.t_double/=10;
                        }else{
                            if(flag1sUpdate)  oneSecData[j].data.t_ushort = info->data.t_ushort;
                            if(flag10sUpdate) tenSecData[j].data.t_ushort = info->data.t_ushort;
                        }
                        break;
                    case EnumClass::t_int:
                        if(info->accumFlag){
                            if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_int;
                            if(flag10sUpdate) tenSecData[j].data.t_double += info->data.t_int;

                            if((dataCount+1)%(frameHz*10)==0) tenSecData[j].data.t_double/=10;
                        }else{
                            if(flag1sUpdate)  oneSecData[j].data.t_int = info->data.t_int;
                            if(flag10sUpdate) tenSecData[j].data.t_int = info->data.t_int;
                        }
                        break;
                    case EnumClass::t_uint:
                        if(info->accumFlag){
                            if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_uint;
                            if(flag10sUpdate) tenSecData[j].data.t_double += info->data.t_uint;

                            if((dataCount+1)%(frameHz*10)==0) tenSecData[j].data.t_double/=10;
                        }else{
                            if(flag1sUpdate)  oneSecData[j].data.t_uint = info->data.t_uint;
                            if(flag10sUpdate) tenSecData[j].data.t_uint = info->data.t_uint;
                        }
                        break;
                    case EnumClass::t_float:
                        if(info->accumFlag){
                            if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_float;
                            if(flag10sUpdate) tenSecData[j].data.t_double += info->data.t_float;

                            if((dataCount+1)%(frameHz*10)==0) tenSecData[j].data.t_double/=10;
                        }else{
                            if(flag1sUpdate)  oneSecData[j].data.t_float = info->data.t_float;
                            if(flag10sUpdate) tenSecData[j].data.t_float = info->data.t_float;
                        }
                        break;
                    case EnumClass::t_double:
                        if(info->accumFlag){
                            if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_double;
                            if(flag10sUpdate) tenSecData[j].data.t_double += info->data.t_double;

                            if((dataCount+1)%(frameHz*10)==0) tenSecData[j].data.t_double/=10;
                        }else{
                            if(flag1sUpdate)  oneSecData[j].data.t_double = info->data.t_double;
                            if(flag10sUpdate) tenSecData[j].data.t_double = info->data.t_double;
                        }
                        break;
                    default:
                        break;
                }


                // 1s 动态更新表格数据
                if((dataCount+1)%frameHz==0){
                    // 需要用到j, 所以放到for内
                    ui->tableWidget->item(j, 0)->setText(oneSecData[j].getDataStr());


                    // 绘图数据准备
                    if(info->curve0Index>=1 && info->curve0Index<=PLOT_MAX_LINE){
                        data0.push_back(oneSecData[j].getDataStr().toDouble());
                    }

                    if(info->curve1Index>=1 && info->curve1Index<=PLOT_MAX_LINE){
                        data1.push_back(oneSecData[j].getDataStr().toDouble());
                    }

                    if(info->curve2Index>=1 && info->curve2Index<=PLOT_MAX_LINE){
                        data2.push_back(oneSecData[j].getDataStr().toDouble());
                    }
                }

            }


            // 1s 绘图添加数据
            if((dataCount+1)%frameHz==0){
                plot->plotAddData(0, dataCount, data0);
                plot->plotAddData(1, dataCount, data1);
                plot->plotAddData(2, dataCount, data2);
            }

            // 写入 1s 数据，并清除使之重新计算
            if(bSave1sFlag && (dataCount+1)%frameHz==0){
                parse.writeFile(f1sFile.ostrm, &oneSecData);
                oneSecData.clear();
            }

            // 写入 10s 数据
            if( bSave10sFlag && (dataCount+1)%(frameHz*10)==0){
                parse.writeFile(f10sFile.ostrm, &tenSecData);
                tenSecData.clear();
            }

            // 接收数据+1
            dataCount++;
        }
    }

}

// 显示日志
void MainWindow::LogShow(QString info, bool format){
    static int i=1;

    if(i>100){
        ui->logBrowser->setText("");
        i=1;
    }

    PLOGD.printf("LogShow: %s", qPrintable(info));


    // 给消息添加序号和 换行
    if(format){
        info = QString("%1: %2").arg(i++).arg(info);
        ui->logBrowser->append(info);
    }else{
        ui->logBrowser->insertPlainText(info);
    }

}

