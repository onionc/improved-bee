#ifndef PARSETHREAD_H
#define PARSETHREAD_H

#include <QThread>
#include <QString>
#include <QByteArray>
#include "parse.h"
#include "util.h"

class ParseThread : public QThread
{
    Q_OBJECT
public:
    ParseThread(Parse *parse, QString filename, quint32 hz);
protected:
    void run();
private:
    QString filename; // 文件名称
    quint32 frameLen; // 帧长度
    quint32 frameHz; // 频率
    Parse *pParse;

    Parse parse; // 解析
    util::WriteFile fRawFile, fNavDataFile, f1sFile; // 文件

    // 解析使用

    quint32 dataCount;
    const QVector<NAV_Data> *navData;
    QVector<NAV_Data> oneSecData; // 1s的数据
    bool flag1sUpdate;
    EnumClass::typeListEnum typeTmp;



    void parseOneFrame(QByteArray &recvBuf); // 解析一帧数据
signals:
    void updateProgress(int i);
    void updateMsg(QString msg);

};

#endif // PARSETHREAD_H
