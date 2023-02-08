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

    // 初始化图形1~3
    initGraph(1);
    initGraph(2);
    initGraph(3);

}

Plot::~Plot()
{
    delete ui;
}

void Plot::initGraph(int index){
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

    // 增加线条
    qplot->addGraph();
    qplot->graph(0)->setPen(QPen(Qt::red));
    qplot->addGraph();
    qplot->graph(1)->setPen(QPen(Qt::blue));
    qplot->addGraph();
    qplot->graph(2)->setPen(QPen(Qt::darkGreen));

    // 坐标轴显示刻度
    qplot->yAxis->setTickLabels(true);
    qplot->xAxis->ticker()->setTickCount(10); // 刻度个数


    // 图例设置
    qplot->legend->setVisible(true);
    qplot->legend->setBrush(QColor(255,255,255,180)); // 图例背景设置透明
    qplot->axisRect()->insetLayout()->setInsetAlignment(0,Qt::AlignTop|Qt::AlignLeft); // 左上角
    // 设置图例字体及大小
    QFont legendFont = QFont();
    legendFont.setPointSize(10);
    legendFont.setFamily("Times New Roman");
    qplot->legend->setFont(legendFont);
    // 设置被选中项的字体及颜色
    qplot->legend->setSelectedFont(legendFont);
    qplot->legend->setSelectedTextColor(QColor(Qt::magenta));
    // 设置被选中项的图标的边框及颜色
    QPen legendPen = QPen();
    legendPen.setStyle(Qt::DashLine);
    legendPen.setColor(QColor(Qt::magenta));
    qplot->legend->setSelectedIconBorderPen(legendPen);
    // 设置图例中可以被选择的部分：仅选择其中的每个图例项，不会选择边框
    qplot->legend->setSelectableParts(QCPLegend::spItems);
    // 选中图例时，对应线条也选中
    connect(qplot, &QCustomPlot::selectionChangedByUser, this, [=](){
        for (int i=0; i<qplot->graphCount(); ++i){
            QCPGraph *graph = qplot->graph(i);
            QCPPlottableLegendItem *item = qplot->legend->itemWithPlottable(graph);
            if (item->selected() || graph->selected())
            {
            item->setSelected(true);
            graph->setSelection(QCPDataSelection(graph->data()->dataRange()));
            graph->selectionDecorator()->setPen(QPen(QColor(Qt::magenta)));
            }
        }
    });

    // 四边都有轴
    // qplot->axisRect()->setupFullAxesBox();

    // 设置交互模式：iRangeDrag轴范围拖动 iRangeZoom滚动放大 iSelectLegend图例可被选中 iSelectPlottables曲线可被选中
    qplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectAxes);


    // 鼠标放入绘图区，变手形状
    qplot->setCursor(QCursor(Qt::PointingHandCursor));

}

void Plot::plotAddData(int index,
                  const QVector<double> &key,
                  const QVector<double> &value1,
                  const QVector<double> &value2,
                  const QVector<double> &value3){

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

    if(!value1.isEmpty()){
        qplot->graph(0)->addData(key, value1);
    }

    if(!value2.isEmpty()){
        qplot->graph(1)->addData(key, value2);
    }

    if(!value3.isEmpty()){
        qplot->graph(2)->addData(key, value3);
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
    if(dCenter>1e-5){ // 接近0的不缩放，0会触发问题一直缩放刻度的bug
        qplot->yAxis->scaleRange(1.2, dCenter);
    }

    // 重绘
    qplot->replot(QCustomPlot::rpQueuedReplot);
}

void Plot::on_pushButton_clicked(bool checked)
{
    ui->mPlot1->legend->setVisible(checked);
    ui->mPlot2->legend->setVisible(checked);
    ui->mPlot3->legend->setVisible(checked);
    ui->mPlot1->replot(QCustomPlot::rpQueuedReplot);
    ui->mPlot2->replot(QCustomPlot::rpQueuedReplot);
    ui->mPlot3->replot(QCustomPlot::rpQueuedReplot);
}
