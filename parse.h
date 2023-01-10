#ifndef PARSE_H
#define PARSE_H

// 数据解析处理

#include <QObject>
#include <QString>
#include <QSettings>
#include <QTableWidget>
#include <QTextCodec>
#include "datatype.h"
#include <vector>
#include <QVector>
#include <QMessageBox>
#include <QDebug>

using namespace DATA;

class Parse : public QObject
{
    Q_OBJECT
public:
    explicit Parse(QObject *parent=nullptr);
    ~Parse();

    // 协议写入ini文件
    void writeToIni(const QVector<SProperty> *data, QString saveFilename);
    // 从ini文件读取
    void loadFromIni(QString readFilename, QVector<SProperty> *data);


    // 协议帧信息
    quint16 frameLen=0; // 帧长度
    EnumClass::checkSumListEnum frameCheckSumType; // 校验类型
    QVector<quint8> frameHeaderArr; // 帧头
    QVector<EnumClass::typeListEnum> frameDataType; // 数据类型
    QVector<QString> frameDataName; // 数据名称

    // 协议帧信息解析：查找帧头、校验、帧长
    bool parseFrame(const QVector<SProperty> *data, QString &errorMsg);

};

#endif // PARSE_H
