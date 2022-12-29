#ifndef TABLEWIDGETDRAG_H
#define TABLEWIDGETDRAG_H
#include <QTableWidget>
#include <QDropEvent>

// 表格组件，+拖拽

class TableWidgetDrag : public QTableWidget
{
public:
    explicit TableWidgetDrag(QWidget *parent=nullptr);

protected:
    void dropEvent(QDropEvent *event) Q_DECL_OVERRIDE;
};

#endif // TABLEWIDGETDRAG_H
