#ifndef PLOT_H
#define PLOT_H

// 绘图组件，显示三组图形，每组三种数据

#include <QWidget>
#include <QPen>
#include <QVector>
#include <QDebug>
#include <QString>


namespace Ui {
class Plot;
}

class Plot : public QWidget
{
    Q_OBJECT

public:
    explicit Plot(QWidget *parent = 0);
    ~Plot();

    // 初始化图形框 1~3
    void initGraph(int index);
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

private slots:
    void on_pushButton_clicked(bool checked);
    // 清除绘图
    void on_clearBtn_clicked();

private:
    Ui::Plot *ui;

    // 三个图表每个里面有几条线，数值为0则图表不绘制
    int chart1, chart2, chart3;

    // 三个图形的x计数
    long xCount1, xCount2, xCount3;


    // 初始化图表
    void init();
};

#endif // PLOT_H
