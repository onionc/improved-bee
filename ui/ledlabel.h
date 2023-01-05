#ifndef LEDLABEL_H
#define LEDLABEL_H

// led label： label当指示灯用

#include <QWidget>
#include <QLabel>

class LedLabel : public QLabel
{
public:
    explicit LedLabel(QWidget* parent=nullptr);

    void setSize(unsigned short size);
    void setColor(int color);
protected:
    //void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;

private:
    unsigned short size;
    int color;
    void setStyle();
};

#endif // LEDLABEL_H
