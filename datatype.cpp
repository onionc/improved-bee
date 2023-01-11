#include "datatype.h"

const char* DATA::FRAME_HEADER="Frame_Header";
const char* DATA::FRAME_END="Frame_Tail";
const char* DATA::FRAME_CHECK="Check_Sum";


// const QStringList DATA::typeList = QStringList()<<"char"<<"uchar"<<"short"<<"ushort"<<"int"<<"uint"<<"float"<<"double";
const QStringList DATA::typeList = [](){
    // 从枚举key中生成 QStringList
    QMetaEnum meta = QMetaEnum::fromType<DATA::EnumClass::typeListEnum>();
    QStringList t;

    for(int i=0; i<meta.keyCount(); i++){
        t<<QString(meta.key(i));
    }

    return t;
}();

// const QStringList DATA::checkSumList = QStringList()<<"add8"<<"xor8"<<"ccitt-xmodem";
const QStringList DATA::checkSumList = [](){
    // 从枚举key中生成 QStringList
    QMetaEnum meta = QMetaEnum::fromType<DATA::EnumClass::checkSumListEnum>();
    QStringList t;

    for(int i=0; i<meta.keyCount(); i++){
        if(QString(meta.key(i))=="None") continue; // None为特殊值，在显示时不需要选择
        t<<QString(meta.key(i));
    }

    return t;
}();

// 枚举转字符串
QString DATA::type2Str(EnumClass::typeListEnum type){
    int t = 0;
    if(type>=0 && type<typeList.size()){
        t = type;
    }
    return typeList.value(t, "");
}

QString DATA::checkSum2Str(EnumClass::checkSumListEnum checkType){
    int t = 0;
    if(checkType>=0 && checkType<checkSumList.size()){
        t = checkType;
    }
    return checkSumList.value(t, "");
}

// 字符串转枚举
DATA::EnumClass::typeListEnum DATA::str2Type(QString typeStr){
    int t = typeList.indexOf(typeStr);
    return EnumClass::typeListEnum(t<0 ? 0 : t);
}

DATA::EnumClass::checkSumListEnum DATA::str2CheckSum(QString checkTypeStr){
    int t = checkSumList.indexOf(checkTypeStr);
    return EnumClass::checkSumListEnum(t<0 ? 0 : t);
}


