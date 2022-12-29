#include "comboboxdelegate.h"
#include "datatype.h"

ComboboxDelegate::ComboboxDelegate(QObject *parent) : QItemDelegate(parent){

}

QWidget *ComboboxDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const {

    QComboBox* classBox = new QComboBox(parent);
    classBox->setFrame(false);
    classBox->setEditable(false);

    classBox->addItems(typeList);
    classBox->setCurrentIndex(0);
    classBox->setStyleSheet("font-size:16px;");
    return classBox;
}

void ComboboxDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const {

    QItemDelegate::setEditorData(editor, index);

}

void ComboboxDelegate::updateEditorGeometry(QWidget *editor, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    editor->setGeometry(option.rect);
}

void ComboboxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const {
    QComboBox *box = static_cast<QComboBox*>(editor);
    QString str = box->currentText();
    model->setData(index, str, Qt::EditRole);
    model->setProperty("font", 20);
}
