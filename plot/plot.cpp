#include "plot.h"


Plot::Plot(QCustomPlot *mPlot)
{
    this->qplot = mPlot;
    chart1=0;
}

Plot::~Plot()
{
}

// 设置图表中线条个数
void Plot::setChartNum(int chart1Num){
    if(chart1Num<1){
        chart1Num = 0;
    }else if(chart1Num>3){
        chart1Num = 3;
    }

    chart1 = chart1Num;

    initGraph();
}


void Plot::initGraph(){

    // 清除
    qplot->clearGraphs();



    if(chart1>0){
        // 增加线条
        qplot->addGraph();
        qplot->graph(0)->setPen(QPen(Qt::red));
        // 显示数据点
        qplot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssPlus, 5));
    }
    if(chart1>1){
        qplot->addGraph();
        qplot->graph(1)->setPen(QPen(Qt::blue));

        qplot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssPlus, 5));
    }
    if(chart1>2){
        qplot->addGraph();
        qplot->graph(2)->setPen(QPen(Qt::darkGreen));

        qplot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssPlus, 5));
    }

    // 自动设置界面范围
    qplot->rescaleAxes();

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


    // 四边都有轴
    // qplot->axisRect()->setupFullAxesBox();

    // 设置交互模式：iRangeDrag轴范围拖动 iRangeZoom滚动放大 iSelectLegend图例可被选中 iSelectPlottables曲线可被选中
    qplot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectLegend | QCP::iSelectPlottables | QCP::iSelectAxes);


    // 鼠标放入绘图区，变手形状
    qplot->setCursor(QCursor(Qt::PointingHandCursor));

    // 计数清空
    xCount1 = xCount2 = xCount3 = 0;

    // 图例默认不显示
    qplot->legend->setVisible(false);

    // 重绘
    qplot->replot();
}


void Plot::plotAddData(int index, double key, double value1, double value2, double value3){

    if(index!=1){  // 只允许有一个图表
        return;
    }

    long count = 0; // x轴计数

    count = ++xCount1;


    if(chart1>0){
        qplot->graph(0)->addData(count, value1);
        if(chart1>1){
            qplot->graph(1)->addData(count, value2);
        }
        if(chart1>2){
            qplot->graph(2)->addData(count, value3);
        }

        // 自适应大小
        qplot->rescaleAxes(true);
        // 放大区间，使数据边界进入图形内
        double dCenter = qplot->yAxis->range().center();
        if(fabs(dCenter)>1e-5){ // 接近0的不缩放，0会触发问题一直缩放刻度的bug
            qplot->yAxis->scaleRange(1.1, dCenter);
        }

        // 重绘
        qplot->replot(QCustomPlot::rpQueuedReplot);
    }
}

