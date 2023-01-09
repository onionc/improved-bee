#include "parse.h"

Parse::Parse(QObject *parent) : QObject(parent){

}

Parse::~Parse(){

}

// 写入ini文件
void Parse::writeToIni(const QVector<SProperty> *data, QString saveFilename){

    QSettings write(saveFilename, QSettings::IniFormat);
    write.setIniCodec(QTextCodec::codecForName("UTF-8"));
    write.clear();

    for(int i=0; i<data->size(); i++){
        const SProperty *info = &data->at(i);
        // 是否选定
        write.setValue(QString("%1/checked").arg(i), info->checked);

        // 名称、类型、数据
        write.setValue(QString("%1/name").arg(i), info->name);
        write.setValue(QString("%1/type").arg(i), info->type);
        write.setValue(QString("%1/data").arg(i), info->data);

        // 图表绘图项是否选定
        write.setValue(QString("%1/curve1").arg(i), info->curve1);
        write.setValue(QString("%1/curve2").arg(i), info->curve2);
        write.setValue(QString("%1/curve3").arg(i), info->curve3);
    }
}


// 从ini文件读取
void Parse::loadFromIni(QString readFilename, QVector<SProperty> *data){
    // 打开ini文件
    QSettings read(readFilename, QSettings::IniFormat);
    QStringList allGroups = read.childGroups();
    data->clear();
    foreach(QString groupKey, allGroups){
        SProperty info;
        info.name = read.value(QString("%1/name").arg(groupKey)).toString();
        info.type = read.value(QString("%1/type").arg(groupKey)).toString();
        info.data = read.value(QString("%1/data").arg(groupKey)).toString();

        info.checked = read.value(QString("%1/checked").arg(groupKey)).toBool();
        info.curve1 = read.value(QString("%1/curve1").arg(groupKey)).toBool();
        info.curve2 = read.value(QString("%1/curve2").arg(groupKey)).toBool();
        info.curve3 = read.value(QString("%1/curve3").arg(groupKey)).toBool();
        data->push_back(info);
    }
}




// 检查协议：查找帧头、校验、帧长
bool Parse::checkFrame(const QVector<SProperty> *data, QString &errorMsg){

    int itmp = 0;
    QString tmp;
    bool ok;
    errorMsg = "";


    for(int i=0; i<data->size(); i++){
        const SProperty *info = &data->at(i);

        // 帧头
        if(info->name==FRAME_HEADER){
            itmp = info->data.toInt(&ok, 16);
            if(itmp>0xff || !ok){
                errorMsg = QString("帧头设置错误，错误值：%1").arg(info->data);
                return false;
            }
        }
    }

    return true;
}
