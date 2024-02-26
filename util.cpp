#include "util.h"
#include "XString.h"

bool util::smallEndian = true;

// 获取日期或日期时间
QString util::getDatetime(bool onlyDate, bool name){
    QDateTime time = QDateTime::currentDateTime();

    QString tmp;
    if(onlyDate){
        if(name){
            tmp = time.toString("yyyy-MM-dd");
        }else{
            tmp = time.toString("yyyy_MM_dd");
        }

    }else{
        if(name){
            // 可作为文件名使用的字符串
            tmp = time.toString("yyyy_MM_dd_hh_mm_ss");
        }else{
            tmp = time.toString("yyyy-MM-dd hh:mm:ss");
        }
    }

    return tmp;
}


// 创建目录
QString util::createDir(QString path){
    QDir dir(path);

    if(dir.exists(path)){
        return path;
    }

    int i = path.lastIndexOf('/');
    QString parentDir="", dirName="";

    if(i>0){
        // 有多级目录
        parentDir = createDir(path.mid(0, i));
    }

    // 创建目录
    dirName = path.mid(i+1);
    QDir  parentPath(parentDir);
    if(!dirName.isEmpty()){
        parentPath.mkpath(dirName);
    }


    return i>0 ? parentDir+"/"+dirName : dirName;
}

// 日志初始化
void util::logInit(long maxSize){
    // 多线程不安全
    static bool f = false;
    if(!f){
        // 创建目录
        QString dir = "./log/";
        dir = createDir(dir);

        dir+=getDatetime(true)+".log";

        plog::init(plog::debug, XString::QSToChar(dir).c_str(), maxSize, 1);
        f = true;
    }

}


// 创建文件
util::WriteFile::WriteFile():ostrm(NULL), filePath(""){}
util::WriteFile::~WriteFile(){close();}

void util::WriteFile::close(){
    if(ostrm && ostrm->is_open()){
        ostrm->close();
    }
}

bool util::WriteFile::open(QString path){
    // 拆分目录和文件名
    int i = path.lastIndexOf('/');
    QString dirName="",filename="";

    filename = path.mid(i+1);
    if(filename.isEmpty()){
        return false;
    }

    if(i>0){
        // 有目录
        dirName = createDir(path.mid(0, i));
        filePath = dirName+"/"+filename;
    }else{
        filePath = filename;
    }

    // 创建文件
    ostrm = new ofstream(XString::QSToChar(filePath), ios::app|ios::binary);
    if(ostrm->is_open()){
        return true;
    }

    return false;
}

void util::WriteFile::write(QString s){
    if(ostrm->is_open()){
        *ostrm << XString::QSToChar(s);
    }
}

void util::WriteFile::write(QByteArray arr){
    if(ostrm->is_open()){
        ostrm->write(arr.constData(), arr.size());
        ostrm->flush();
    }
}

void util::WriteFile::write(const char *data, quint32 len){
    if(ostrm->is_open()){
        ostrm->write(data, len);
        ostrm->flush();
    }
}


QString util::WriteFile::getFilePath(){
    return filePath;
}



/*
小端 [0x12,0x34, 0x56,0x78] => 0x12345678;
大端 [0x12,0x34, 0x56,0x78] => 0x78563412;
*/

// convert 4 bytes to float
float util::bytes2float(quint8 arr[])
{
    static bytes4float b;
    for(int i=0;i<4;i++){
        if(smallEndian){
            b.byte_arr[i] = arr[i];
        }else{
            b.byte_arr[3-i] = arr[i];
        }
    }
    return b.f;
}

double util::bytes2double(quint8 arr[])
{
    static bytes8double b;
    for(int i=0;i<8;i++){
        if(smallEndian){
            b.byte_arr[i] = arr[i];
        }else{
            b.byte_arr[7-i] = arr[i];
        }
    }
    return b.d;
}
qint16 util::bytes2short(quint8 arr[])
{
    static bytesShort b;
    for(int i=0;i<2;i++){
        if(smallEndian){
            b.byte_arr[i] = arr[i];
        }else{
            b.byte_arr[1-i] = arr[i];
        }
    }
    return b.d;
}
quint16 util::bytes2ushort(quint8 arr[])
{
    static bytesUshort b;
    for(int i=0;i<2;i++){
        if(smallEndian){
            b.byte_arr[i] = arr[i];
        }else{
            b.byte_arr[1-i] = arr[i];
        }
    }
    return b.d;
}

qint32 util::bytes2int(quint8 arr[])
{
    static bytesInt b;
    for(int i=0;i<4;i++){
        if(smallEndian){
            b.byte_arr[i] = arr[i];
        }else{
            b.byte_arr[3-i] = arr[i];
        }
    }
    return b.d;
}

// 3字节转int，考虑最高位表示负数的情况
qint32 util::bytes3_2int(quint8 arr[])
{
    static bytesInt b;
    for(int i=0;i<3;i++){
        if(smallEndian){
            b.byte_arr[i] = arr[i];
        }else{
            b.byte_arr[2-i] = arr[i];
        }
    }

    b.byte_arr[3] = 0;
    if((b.byte_arr[2]&0x80) == 0x80){
        b.byte_arr[3] = 0xff;
    }

    return b.d;
}

quint32 util::bytes2uint(quint8 arr[])
{
    bytesUint b;
    for(int i=0;i<4;i++){
        if(smallEndian){
            b.byte_arr[i] = arr[i];
        }else{
            b.byte_arr[3-i] = arr[i];
        }
    }
    return b.d;
}



// 写入ini文件
void util::writeIni(QString saveFilename, QString key, QVariant value){

    QSettings write(saveFilename, QSettings::IniFormat);
    write.setIniCodec(QTextCodec::codecForName("UTF-8"));
    //write.clear();

    write.setValue(key, value);
}


// 读取ini
QVariant util::readIni(QString readFilename, QString key){
    // 打开ini文件
    QSettings read(readFilename, QSettings::IniFormat);
    read.setIniCodec(QTextCodec::codecForName("UTF-8"));
    return read.value(key);
}
