#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <fstream>
#include <QDateTime>
#include <QDir>
#include "plog/Log.h"
#include "plog/Initializers/RollingFileInitializer.h"
#include <QVariant>
#include <QSettings>
#include <QTextCodec>

namespace util{

    // 获取日期或日期时间  name=true 可作为文件名使用的字符串
    QString getDatetime(bool onlyDate=false, bool name=false);

    // 创建目录，多级以/分隔
    QString createDir(QString path);

    // 日志初始化
    void logInit(long maxSize=1024*1024*10);

    // 写文件
    class WriteFile{
        private:
            QString filePath;
        public:
            std::ofstream *ostrm;
            WriteFile();
            ~WriteFile();

            // 打开文件，如果是本目录，可用"./"表示或只写文件名称
            bool open(QString path);
            void close();

            // 写数据
            void write(QString s);
            // 返回文件名
            QString getFilePath();
    };


    // 类型转换
    extern bool smallEndian;
    union bytes4float
    {
        float f;
        quint8 byte_arr[4];
    };

    union bytes8double
    {
        double d;
        quint8 byte_arr[8];
    };
    union bytesShort
    {
        qint16 d;
        quint8 byte_arr[2];
    };

    union bytesUshort{
        quint16 d;
        quint8 byte_arr[2];
    };
    union bytesInt {
        qint32 d;
        quint8 byte_arr[4];
    };
    union bytesUint {
        quint32 d;
        quint8 byte_arr[4];
    };
    float bytes2float(quint8 arr[]);
    double bytes2double(quint8 arr[]);
    qint16 bytes2short(quint8 arr[]);
    quint16 bytes2ushort(quint8 arr[]);

    qint32 bytes2int(quint8 arr[]);
    quint32 bytes2uint(quint8 arr[]);


    // 写入ini文件
    void writeIni(QString saveFilename, QString key, QVariant value);

    // 读取ini
    QVariant readIni(QString readFilename, QString key);
}


#endif // UTIL_H
