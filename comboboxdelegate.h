#ifndef COMBOBOXDELEGATE_H
#define COMBOBOXDELEGATE_H

#include <QItemDelegate>
#include <QComboBox>


class ComboboxDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ComboboxDelegate(QObject *parent = Q_NULLPTR);

    // 创建并返回部件
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    // 操作部件的数据
    void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;

    // 确保部件在item中正确显示
    void updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

    // 更新数据
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;

};

#endif // COMBOBOXDELEGATE_H
