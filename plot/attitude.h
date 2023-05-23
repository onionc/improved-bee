#ifndef ATTITUDE_H
#define ATTITUDE_H

#include <QWidget>

namespace Ui {
class Attitude;
}

class Attitude : public QWidget
{
    Q_OBJECT

public:
    explicit Attitude(QWidget *parent = 0);
    ~Attitude();

private:
    Ui::Attitude *ui;
};

#endif // ATTITUDE_H
