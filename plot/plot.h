#ifndef PLOT_H
#define PLOT_H

// 绘图组件，显示三组图形，每组三种数据

#include <QWidget>
#include <QPen>
#include <QVector>
#include <QDebug>

namespace Ui {
class Plot;
}

class Plot : public QWidget
{
    Q_OBJECT

public:
    explicit Plot(QWidget *parent = 0);
    ~Plot();

    //  index为图形索引
    /**
     * @brief plotAddData 图形增加数据
     * @param index 图形索引，1~3 分别代表三个图形
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
private:
    Ui::Plot *ui;
};

#endif // PLOT_H
