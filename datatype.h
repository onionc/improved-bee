#ifndef DATATYPE_H
#define DATATYPE_H

// 所需数据

#include <QStringList>
#include <QMap>
#include <QString>
#include <QMetaEnum>
#include <QObject>
#include <QtCore>
#include "util.h"

#define DEFAULT_HZ 200
#define INI_OTHER "Other"

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
    // 类型
    typedef union typeUnion{
        qint8 t_char;
        quint8 t_uchar;
        qint16 t_short;
        quint16 t_ushort;
        qint32 t_int;
        quint32 t_uint;
        float t_float;
        double t_double;
    }type;

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
    enum FieldColumn{colName=0, colType, colData, colAccumCheck, colCurve1, colCurve2, colCurve3};
    // 协议信息 每一项（属性）的结构体
    typedef struct SProperty_Struct{
        QString name;
        QString type;
        QString data;
        bool accumCheck=false; // 累加值
        bool curve1=false; // 绘图到图形1
        bool curve2=false;
        bool curve3=false;

    }SProperty;
    // 数据 每一项的结构体
    typedef struct NAV_Data_Struct{
        QString name; // 名称
        EnumClass::typeListEnum type; // 类型
        quint8 bytesLen; // 类型对应的数据长度
        QByteArray buf; // 数组
        // 数据
        typeUnion data;
        // 函数名，函数不为空则取值为扩展值（表示协议有其他计算过程并调用脚本计算，结果用t_double值），为空则表示不做特殊处理（使用对应原始type的值）
        QString extFuncName = "";

        // 是否为累加值，true：1s数据是求和），false：瞬时值
        bool accumFlag = false;

        // 绘图，共三个图形框，每个图形下最多有三种数据
        quint8 curve1Index = 0; // 从1开始（1~3），代表第几种数据
        quint8 curve2Index = 0;
        quint8 curve3Index = 0;

        // 获取数据（字符串格式），需要具体获取到对应类型数据，需要检查type，再调用data相应字段
        const QString getDataStr() const{
            QString s;
            if(!extFuncName.isEmpty()
               || accumFlag){
                // 如果有计算或者有累加，使用double值
                s = QString("%1").arg(data.t_double);
            }else{
                switch(type){
                    case EnumClass::t_char: s = QString("%1").arg(data.t_char); break;
                    case EnumClass::t_uchar: s = QString("%1").arg(data.t_uchar); break;
                    case EnumClass::t_short: s = QString("%1").arg(data.t_short); break;
                    case EnumClass::t_ushort: s = QString("%1").arg(data.t_ushort); break;
                    case EnumClass::t_int: s = QString("%1").arg(data.t_int); break;
                    case EnumClass::t_uint: s = QString("%1").arg(data.t_uint); break;
                    case EnumClass::t_float: s = QString::number(data.t_float, 'f', 8); break;
                    case EnumClass::t_double: s = QString::number(data.t_double, 'f', 16); break;
                    default:
                        s = "";
                        break;
                }
            }

            return s;
        }
    }NAV_Data;

}

#endif // DATATYPE_H
