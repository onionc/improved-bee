#include "plot.h"
#include "ui_plot.h"
#include "plot/qcustomplot.h"

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plot)
{
    ui->setupUi(this);
    // 标题
    setWindowTitle("绘图");
    // 主窗口关闭时，关闭子窗口
    setAttribute(Qt::WA_QuitOnClose, false);

    // ------------- 图形1 ------------------
    // 增加线条
    ui->mPlot1->addGraph();
    ui->mPlot1->graph(0)->setPen(QPen(Qt::red));
    ui->mPlot1->addGraph();
    ui->mPlot1->graph(1)->setPen(QPen(Qt::blue));
    ui->mPlot1->addGraph();
    ui->mPlot1->graph(2)->setPen(QPen(Qt::gray));

    // 坐标轴显示刻度
    ui->mPlot1->yAxis->setTickLabels(true);
    ui->mPlot1->xAxis->ticker()->setTickCount(10); // 刻度个数
    // 设置显示曲线图例
    ui->mPlot1->legend->setVisible(true);
    ui->mPlot1->legend->setBrush(QColor(255,255,255,0)); // 图例背景设置透明
    ui->mPlot1->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignLeft); // 左上角
    // 四边都有轴
    ui->mPlot1->axisRect()->setupFullAxesBox();

    // 设置交互模式：iRangeDrag轴范围拖动 iRangeZoom滚动放大 iSelectLegend图例可被选中 iSelectPlottables曲线可被选中
    ui->mPlot1->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectAxes);


    // ------------- 图形2 ------------------
    // 增加线条
    ui->mPlot2->addGraph();
    ui->mPlot2->graph(0)->setPen(QPen(Qt::red));
    ui->mPlot2->addGraph();
    ui->mPlot2->graph(1)->setPen(QPen(Qt::blue));
    ui->mPlot2->addGraph();
    ui->mPlot2->graph(2)->setPen(QPen(Qt::gray));


    // ------------- 图形3 ------------------
    // 增加线条
    ui->mPlot3->addGraph();
    ui->mPlot3->graph(0)->setPen(QPen(Qt::red));
    ui->mPlot3->addGraph();
    ui->mPlot3->graph(1)->setPen(QPen(Qt::blue));
    ui->mPlot3->addGraph();
    ui->mPlot3->graph(2)->setPen(QPen(Qt::gray));

}

Plot::~Plot()
{
    delete ui;
}


void Plot::plotAddData(int index,
                  const QVector<double> &key,
                  const QVector<double> &value1,
                  const QVector<double> &value2,
                  const QVector<double> &value3){

    if(index<1 || index>3){
        return;
    }

    if(!value1.isEmpty()){
        ui->mPlot1->graph(0)->addData(key, value1);
    }

    if(!value2.isEmpty()){
        ui->mPlot1->graph(1)->addData(key, value2);
    }

    if(!value3.isEmpty()){
        ui->mPlot1->graph(2)->addData(key, value3);
    }
}

void Plot::plotAddData(int index, double key, double value1, double value2, double value3){

    if(index<1 || index>3){
        return;
    }
    QCustomPlot *qplot;
    if(index==1){
        qplot = ui->mPlot1;
    }else if(index==2){
        qplot = ui->mPlot2;
    }else if(index==3){
        qplot = ui->mPlot3;
    }
    qplot->graph(0)->addData(key, value1);

    qplot->graph(1)->addData(key, value2);

    qplot->graph(2)->addData(key, value3);

    // 自适应大小
    qplot->rescaleAxes();
    // 放大区间，使数据边界进入图形内
    double dCenter = qplot->yAxis->range().center();
    qplot->yAxis->scaleRange(1.2, dCenter);
    // 重绘
    qplot->replot(QCustomPlot::rpQueuedReplot);
}
