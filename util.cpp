#include "util.h"
#include "XString.h"

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
