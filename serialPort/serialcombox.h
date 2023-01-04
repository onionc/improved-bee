#ifndef SERIALCOMBOX_H
#define SERIALCOMBOX_H

// 串口的下拉框自定义

#include <QComboBox>
#include <QMouseEvent>

class SerialComBox : public QComboBox
{
    Q_OBJECT
public:
    explicit SerialComBox(QWidget *parent=nullptr);
    ~SerialComBox();
protected:
    // 鼠标按下
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

signals:
    // 监测端口
    void detectComPorts();
public slots:
    // 更新项目
    void updateItem(QStringList comPorts);
};

#endif // SERIALCOMBOX_H
