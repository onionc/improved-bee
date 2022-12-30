#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QDateTime>
#include <QDir>
#include "plog/Log.h"
#include "plog/Initializers/RollingFileInitializer.h"

namespace util{

    // 获取日期或日期时间  name=true 可作为文件名使用的字符串
    QString getDatetime(bool onlyDate=false, bool name=false);

    // 创建目录，多级以/分隔
    QString createDir(QString path);

    // 日志初始化
    void logInit(long maxSize=1024*1024*10);

    // 打印数组到日志中
    // void log(BYTE* arr, size_t len);

    // 得到文件路径的目录
    // string GetPathDir(string filePath);


    /*




    // 写文件
    class WriteFile{
        private:
            string filePath;
        public:
            ofstream *ostrm;
            WriteFile();
            ~WriteFile();



            // 打开文件，指定目录和文件名, 如果是本目录下，可填（.\\ 或 .）
            bool open(string dir, string fileName);
            void close();

            // 写数据
            void write(string s);
            void write(int t);
            void write(double t);
            // 返回文件名
            string util::WriteFile::getFilePath();
    };

    // 分割字符串，分隔符可指定子字符串
    vector<CString> splitCString(CString s, CString split);
    vector<string> splitCString(string s, string split);

    // 获取目录下的所有文件
    void GetFiles(string path, vector<string>& files);

    typedef struct{
        char drove[20]; // 盘符
        char dir[200]; // 目录
        char name[200]; // 文件名
        char ext[20]; // 后缀

        // 拆分文件路径
        void split(string file){
            memset(this, 0, sizeof(FILE_FULL_PATH));
            _splitpath(file.c_str(), drove, dir, name, ext);
        }

        // 合并（获取）文件路径
        string getPath(){
            CString temp;
            temp.Format("%s%s%s%s", drove, dir, name, ext);
            return temp.GetString();
        }

        // 分别获取路径和文件名
        string getDir(){
            CString temp;
            temp.Format("%s%s", drove, dir);
            return temp.GetString();
        }
        string getFilename(){
            CString temp;
            temp.Format("%s%s", name, ext);
            return temp.GetString();
        }

    }FILE_FULL_PATH; // 文件路径
    void SplitPath(CString file, FILE_FULL_PATH *fileFullPathP);

    */
}


#endif // UTIL_H
