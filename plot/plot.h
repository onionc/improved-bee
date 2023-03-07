#ifndef PLOT_H
#define PLOT_H

// 绘图组件，显示三组图形，每组三种数据

#include <QPen>
#include <QVector>
#include <QDebug>
#include "plot/qcustomplot.h"


class Plot
{

public:
    explicit Plot(QCustomPlot *mPlot);
    ~Plot();

    // 初始化图表
    void initGraph();
    //  index为图形索引
    /**
     * @brief plotAddData 图形增加数据
     * @param index 图形索引，1~3 分别代表三个图表
     * @param key x数据
     * @param value1 y轴数据1
     * @param value2 y轴数据2
     * @param value3 z轴数据3
     */
    void plotAddData(int index,
                      const QVector<double> &key,
                      const QVector<double> &value1,
                      const QVector<double> &value2,
                      const QVector<double> &value3);
    void plotAddData(int index, double key, double value1=0, double value2=0, double value3=0);

    // 设置图表中线条个数，并初始化
    void setChartNum(int chart1Num);
    // 清除绘图
    void on_clearBtn_clicked();

private:
    QCustomPlot *qplot;

    // 图表里面有几条线，数值为0则图表不绘制
    int chart1;

    // 三个图形的x计数
    long xCount1, xCount2, xCount3;


    // 初始化图表
    void init();
};

#endif // PLOT_H
