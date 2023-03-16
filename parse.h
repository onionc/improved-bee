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
#include "util.h"
#include "luaScript/LuaScript.h"

using namespace DATA;

class Parse : public QObject
{
    Q_OBJECT
public:
    explicit Parse(QObject *parent=nullptr);
    ~Parse();

    // 协议写入ini文件
    void writeToIni(const QVector<SProperty> *data, QString saveFilename);
    //
    /**
     * @brief loadFromIni 从ini文件读取协议数据
     * @param readFilename 文件名
     * @param data 数据部分
     * @param frameLittleEndian 是否为小端
     * @param frameHz 频率
     */
    void loadFromIni(QString readFilename, QVector<SProperty> *data, bool &frameLittleEndian,quint32 &frameHz);

    /**
     * @brief parseFrameInfo 协议帧信息解析：查找帧头、校验、帧长
     * @param frameInfoData 协议结构体
     * @param errorMsg
     * @param chartInfoArr 将多个图表的多线条数量（和名称）返回
     * @return
     */
    bool parseFrameInfo(const QVector<SProperty> *frameInfoData, QString &errorMsg, std::vector<std::vector<QString>> &chartInfoArr);

    // 从字节数据中找到一帧数据并调用解析：判断帧头、检验和，每次找出一帧数据去解析
    bool findFrameAndParse(QByteArray &allBytes);



    // 协议
    // CRC16 协议解析 (CCITT-Xmodem)
    unsigned short Crc16Xmode(const char *q, int len);
    // 获取帧长
    quint16 getFrameLen() {return frameLen;}
    // 获取原始数据
    const QVector<NAV_Data>* getNavData(){
        return &frameDataArr;
    }

    // 帧数据写入文件
    void writeFile(std::ofstream *fp, const QVector<NAV_Data>* navData, bool addHeader=false);

private:
    // 校验数据，checkDataBytes 为要校验的数据，checkBytes 为校验字节
    bool checkData(const QByteArray &checkDataBytes, const QByteArray &checkBytes);

    // 解析一帧数据：拿帧字节匹配协议解析数据，frameBytesData 仅为数据（无帧头和校验）
    bool parseFrameData(const QByteArray &frameBytesData);


private:
    // 协议帧信息
    quint16 frameLen=0; // 帧字节长度 = 帧头+数据+校验
    QByteArray frameHeaderArr; // 帧头
    QVector<NAV_Data> frameDataArr; // 帧的数据部分
    EnumClass::checkSumListEnum frameCheckSumType=EnumClass::None; // 校验类型
    quint16 frameHeaderLen=0; // 帧头字节长度
    quint16 frameDataLen=0; // 数据字节长度
    quint16 frameCheckLen=0; // 校验字节长度



    // lua脚本
    LuaScript luaScirpt;

};

#endif // PARSE_H
