#ifndef DATATYPE_H
#define DATATYPE_H

// 所需数据

#include <QStringList>
#include <QMap>
#include <QString>
#include <QMetaEnum>
#include <QObject>
#include <QtCore>

// 通过枚举类型获取字符串
#define typeS(a) DATA::typeListStr(a)
#define checkS(a) DATA::checkSumListStr(a)

namespace DATA{


    class EnumClass{
        Q_GADGET
    public:
        // 数据类型列表
        enum typeListEnum{t_char=0, t_uchar, t_short, t_ushort, t_int, t_uint, t_float, t_double};
        Q_ENUM(typeListEnum)

        // 校验类型列表
        enum checkSumListEnum{None=0, c_add8, c_xor8, c_crc16_xmodem};
        Q_ENUM(checkSumListEnum)

    };
    const extern QStringList typeList;
    const extern QStringList checkSumList;
    // 枚举转字符串
    QString type2Str(EnumClass::typeListEnum type);
    QString checkSum2Str(EnumClass::checkSumListEnum checkType);
    // 字符串转枚举
    EnumClass::typeListEnum str2Type(QString typeStr);
    EnumClass::checkSumListEnum str2CheckSum(QString checkTypeStr);



    // 特殊数据名称
    extern const char* FRAME_HEADER;
    extern const char* FRAME_END; // 帧尾暂时没用到
    extern const char* FRAME_CHECK;

    // 协议字段
    enum FieldColumn{colName=0, colType, colData, colCurve1, colCurve2, colCurve3};
    // 协议信息 每一项（属性）的结构体
    typedef struct SProperty_Struct{
        QString name;
        QString type;
        QString data;
        bool checked;
        bool curve1;
        bool curve2;
        bool curve3;
    }SProperty;
    // 数据 每一项的结构体
    typedef struct NAV_Data_Struct{
        QString name; // 名称
        EnumClass::typeListEnum type; // 类型
        quint8 bytesLen; // 类型对应的数据长度
        QByteArray buf; // 数组
    }NAV_Data;

}

#endif // DATATYPE_H
