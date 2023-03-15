#include "plot.h"
#include "ui_plot.h"
#include "plot/qcustomplot.h"
#include "datatype.h"

Plot::Plot(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Plot)
{
    ui->setupUi(this);
    // 标题
    setWindowTitle("绘图");
    // 主窗口关闭时，关闭子窗口
    setAttribute(Qt::WA_QuitOnClose, false);


    chart1=chart2=chart3=0;
}

Plot::~Plot()
{
    delete ui;
}

// 设置图表中线条个数
void Plot::setChartNum(std::vector<std::vector<QString>> &chartInfoArr){
    init();


    QCustomPlot *qplot;
    chart1 = chart2 = chart3 = 0;
    // 第i个图表，第j条线
    for(int i=0; i<chartInfoArr.size(); i++){
        for(int j=0; j<chartInfoArr[i].size(); j++){
            if(i==0){
                qplot = ui->mPlot1;
                chart1++;
            }else if(i==1){
                qplot = ui->mPlot2;
                chart2++;
            }else if(i==2){
                qplot = ui->mPlot3;
                chart3++;
            }else{
                break;
            }


            // 增加线条
            qplot->addGraph();
            qplot->graph(j)->setPen(QPen(QColor(CLEAN_Colors[j])));
            // 显示数据点
            qplot->graph(j)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssPlus, 5));
            // 名称
            qplot->graph(j)->setName(chartInfoArr[i][j]);
        }

    }
}
// 初始化图表
void Plot::init(){
    initGraph(1);
    initGraph(2);
    initGraph(3);
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

    // 清除
    qplot->clearGraphs();


    /*

    if(chartNum>0){
        // 增加线条
        qplot->addGraph();
        qplot->graph(0)->setPen(QPen(Qt::red));
        // 显示数据点
        qplot->graph(0)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssPlus, 5));
    }
    if(chartNum>1){
        qplot->addGraph();
        qplot->graph(1)->setPen(QPen(Qt::blue));

        qplot->graph(1)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssPlus, 5));
    }
    if(chartNum>2){
        qplot->addGraph();
        qplot->graph(2)->setPen(QPen(Qt::darkGreen));

        qplot->graph(2)->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ScatterShape::ssPlus, 5));
    }
    */

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

    // 计数清空
    xCount1 = xCount2 = xCount3 = 0;

    // 图例默认显示
    qplot->legend->setVisible(true);


    // 重绘
    qplot->replot();
}


void Plot::plotAddData(int index, double key, std::vector<double> &data){

    if(index<1 || index>3){
        return;
    }
    QCustomPlot *qplot;
    long count = 0; // x轴计数
    int chartNum = 0; // 线条数量
    if(index==1){
        qplot = ui->mPlot1;
        count = ++xCount1;
        chartNum = chart1;
    }else if(index==2){
        qplot = ui->mPlot2;
        count = ++xCount2;
        chartNum = chart2;
    }else if(index==3){
        qplot = ui->mPlot3;
        count = ++xCount3;
        chartNum = chart3;
    }

    for(int i=0; i<chartNum && i<data.size(); i++){
        qplot->graph(i)->addData(count, data[i]);
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

void Plot::on_pushButton_clicked(bool checked)
{
    ui->mPlot1->legend->setVisible(checked);
    ui->mPlot2->legend->setVisible(checked);
    ui->mPlot3->legend->setVisible(checked);
    ui->mPlot1->replot(QCustomPlot::rpQueuedReplot);
    ui->mPlot2->replot(QCustomPlot::rpQueuedReplot);
    ui->mPlot3->replot(QCustomPlot::rpQueuedReplot);
}


void Plot::on_clearBtn_clicked()
{
    init();
}
