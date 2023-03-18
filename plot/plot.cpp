#include "plot.h"


Plot::Plot(std::vector<QCustomPlot*> *mPlotArr)
{
    this->qplotArr = mPlotArr;
}

Plot::~Plot()
{
}

// 设置图表中线条个数
void Plot::setChartNum(std::vector<std::vector<QString>> &chartInfoArr){
    this->chartInfoArr = chartInfoArr;
    init();
}
// 初始化图表
void Plot::init(){
    xCountArr.clear();

    // 第i个图表，第j条线
    for(int i=0; i<qplotArr->size() && i<chartInfoArr.size(); i++){
        QCustomPlot *qplot = qplotArr->at(i);
        initGraph(qplot);
        xCountArr.push_back(0); // x计数
        for(int j=0; j<chartInfoArr[i].size(); j++){
            // 增加线条
            qplot->addGraph();
            qplot->graph(j)->setPen(QPen(QColor(CLEAN_Colors[j])));
            // 显示数据点
            qplot->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssCircle, 3));
            // 名称
            qplot->graph(j)->setName(chartInfoArr[i][j]);

        }
    }
}


void Plot::initGraph(QCustomPlot *qplot){

    // 清除
    qplot->clearGraphs();

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
    legendFont.setPointSize(9);
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


    // 图例默认显示
    qplot->legend->setVisible(true);

    // 重绘
    qplot->replot();
}


void Plot::plotAddData(int index, double key, std::vector<double> &data){

    if(index<0 || index>=qplotArr->size() || index>=chartInfoArr.size()){
        return;
    }

    QCustomPlot *qplot = qplotArr->at(index);
    int chartNum = chartInfoArr[index].size(); // 线条数量

    xCountArr[index]++;

    for(int i=0; i<chartNum && i<data.size(); i++){
        qplot->graph(i)->addData(xCountArr[index], data[i]);
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

