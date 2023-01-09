#ifndef DATATYPE_H
#define DATATYPE_H

#include <QStringList>
#include <QMap>
#include <QString>

// 数据类型字符串
extern const QStringList typeList;


// 协议字段
enum FieldColumn{colName=0, colType, colData, colCurve1, colCurve2, colCurve3};
// 每一项（属性）的结构体
typedef struct SProperty_Struct{
    QString name;
    QString type;
    QString data;
    bool checked;
    bool curve1;
    bool curve2;
    bool curve3;
}SProperty;


class DataType
{
public:
    DataType();
};

#endif // DATATYPE_H
