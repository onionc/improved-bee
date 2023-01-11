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
    EnumClass::checkSumListEnum frameCheckSumType=EnumClass::None; // 校验类型
    QByteArray frameHeaderArr; // 帧头
    QVector<EnumClass::typeListEnum> frameDataType; // 数据类型
    QVector<QString> frameDataName; // 数据名称
    quint16 frameHeaderSize=0; // 帧头长度
    quint16 frameDataSize=0; // 数据长度


    // 协议帧信息解析：查找帧头、校验、帧长
    bool parseFrameInfo(const QVector<SProperty> *frameInfoData, QString &errorMsg);

    // 从字节数据中解析一帧数据：判断帧头、检验和，每次找出一帧数据
    bool parseFrameByte(QByteArray &allBytes);



    // 协议
    // CRC16 协议解析 (CCITT-Xmodem)
    unsigned short Crc16Xmode(char *q, int len);

private:
    // 校验数据，frameBytes 为一帧数据
    bool checkData(const QByteArray &frameBytes);

    // 解析帧信息：拿帧字节和协议解析数据，frameBytes 为一帧数据
    bool parseFrameData(const QByteArray &frameBytes);



};

#endif // PARSE_H
