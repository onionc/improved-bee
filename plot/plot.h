#ifndef PLOT_H
#define PLOT_H

// 绘图组件，显示三组图形，每组三种数据

#include <QPen>
#include <QVector>
#include <QDebug>
#include <QString>
#include "plot/qcustomplot.h"
#include "datatype.h"

class Plot
{

public:
    explicit Plot(std::vector<QCustomPlot*> *mPlotArr);
    ~Plot();

    // 初始化图表
    void initGraph(QCustomPlot *plot);
    //  index为图形索引
    /**
     * @brief plotAddData 图形增加数据
     * @param index 图形索引，1~3 分别代表三个图表
     * @param key x数据
     * @param value1 y轴数据
     */
    void plotAddData(int index, double key, std::vector<double> &data);

    // 设置图表中线条个数，并初始化
    void setChartNum(std::vector<std::vector<QString>> &chartInfoArr);

    // 清除绘图
    void on_clearBtn_clicked();

private:
    std::vector<QCustomPlot*> *qplotArr;

    // 图表里面有几条线，数值为0则图表不绘制
    int chart1;




    // 图表数据
    std::vector<std::vector<QString>> chartInfoArr;
    // 图表的计数
    std::vector<long> xCountArr;

    // 初始化图表
    void init();
};

#endif // PLOT_H
