#include "parsethread.h"
#include <QDebug>
#include <QFile>
#include "util.h"


ParseThread::ParseThread(Parse *parse, QString filename, quint32 hz)
    :dataCount(0), flag1sUpdate(false)
{
    this->pParse  = parse;
    this->filename = filename;
    this->frameLen = this->pParse->getFrameLen();
    this->frameHz = hz;
}

void ParseThread::run(){
    QFile file(filename);
    QString ts;
    char buf[200];
    int count=0;
    qint64 fileSize = 0, fileReadedSize=0;
    int index=0;
    quint8 percent, prevPercent=0;
    if(file.open(QIODevice::ReadOnly)){
        /************** 创建文件 *************/
        // 新建目录
        QString path = QString("data/%1").arg(util::getDatetime(false, true));

        updateMsg("文件保存目录："+path);
        // 原始文件
        if(!fRawFile.open(path+"/data.raw")){
             updateMsg("error: 文件创建失败");
            return;
        }
        if(!fNavDataFile.open(path+"/navFile.csv")){
            updateMsg("error: 文件创建失败");
            return;
        }
        // 1s 文件
        if(!f1sFile.open(path+"/oneSecFile.csv")){
            updateMsg("error: 文件创建失败");
            return;
        }


        // 1s数据重置
        QVector<NAV_Data> oneSecData; // 1s的数据
        Parse parse;
        QByteArray byteArr;

        // 获取文件大小
        fileSize = file.size();
        updateMsg("开始解析");

        // 一直读取到末尾
        int len = 0;
        while(file.atEnd() == false){
            if(byteArr.size()<frameLen){
                len = file.read(buf, frameLen);

                // 保存原始数据
                fRawFile.write(buf, len);

                byteArr.append(buf, len);
            }else{
                len = 0;
            }


            // 解析一帧数
            parseOneFrame(byteArr);

            // 进度条
            fileReadedSize += len;
            percent = (fileReadedSize*100/fileSize);
            updateProgress(percent);

            if(percent-prevPercent>=10){
                prevPercent = percent;
                ts.sprintf("%d%%", percent);
                updateMsg(ts);
            }
        }

        updateMsg("解析完成");
        fRawFile.close();
        fNavDataFile.close();
        f1sFile.close();
    }else{
        updateMsg("文件打开失败");
    }
}

void ParseThread::parseOneFrame(QByteArray &recvBuf){
    // 解析数据

    // 找并解析一帧数
    if(pParse->findFrameAndParse(recvBuf)){
        navData = pParse->getNavData();

        // 保存一帧数据
        pParse->writeFile(fNavDataFile.ostrm, navData, false, true);

        if(frameHz<=0) return;

        // 1s 10s 数据填充
        for(unsigned int j=0; j<navData->size(); j++){
            const NAV_Data *info = &(navData->at(j));

            // 1s 计数判断
            if(dataCount%frameHz==0){
                // 清空数据
                oneSecData.push_back(*info);
                flag1sUpdate = false;
            }else{
                flag1sUpdate = true;
            }


            // 数据处理
            typeTmp = info->extFuncName.isEmpty() ? info->type : EnumClass::t_double; // 扩展数据使用double类型数据
            switch(typeTmp){
                case EnumClass::t_char:
                    if(info->accumFlag){
                        // 累加值

                        // 1s 求和
                        if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_char;
                        else oneSecData[j].data.t_double = info->data.t_char; // 第一次

                    }else{
                        // 瞬时值
                        oneSecData[j].data.t_char = info->data.t_char;
                    }

                    break;
                case EnumClass::t_uchar:
                    if(info->accumFlag){
                        if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_uchar;
                        else oneSecData[j].data.t_double = info->data.t_uchar;
                    }else{
                        oneSecData[j].data.t_uchar = info->data.t_uchar;
                    }

                    break;
                case EnumClass::t_short:
                    if(info->accumFlag){
                        if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_short;
                        else oneSecData[j].data.t_double = info->data.t_short;

                    }else{
                        oneSecData[j].data.t_short = info->data.t_short;
                    }
                    break;
                case EnumClass::t_ushort:
                    if(info->accumFlag){
                        if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_ushort;
                        else oneSecData[j].data.t_double = info->data.t_ushort;

                    }else{
                        oneSecData[j].data.t_ushort = info->data.t_ushort;
                    }
                    break;
                case EnumClass::t_int:
                    if(info->accumFlag){
                        if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_int;
                        else oneSecData[j].data.t_double = info->data.t_int;

                    }else{
                        oneSecData[j].data.t_int = info->data.t_int;
                    }
                    break;
                case EnumClass::t_uint:
                    if(info->accumFlag){
                        if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_uint;
                        else oneSecData[j].data.t_double = info->data.t_uint;

                    }else{
                        oneSecData[j].data.t_uint = info->data.t_uint;
                    }
                    break;
                case EnumClass::t_float:
                    if(info->accumFlag){
                        if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_float;
                        else oneSecData[j].data.t_double = info->data.t_float;

                    }else{
                        oneSecData[j].data.t_float = info->data.t_float;
                    }
                    break;
                case EnumClass::t_double:
                    if(info->accumFlag){
                        if(flag1sUpdate)  oneSecData[j].data.t_double += info->data.t_double;
                        else oneSecData[j].data.t_double = info->data.t_double;
                    }else{
                        oneSecData[j].data.t_double = info->data.t_double;
                    }
                    break;
                default:
                    break;
            }




        }

        // 写入 1s 数据，并清除使之重新计算
        if((dataCount+1)%frameHz==0){
            pParse->writeFile(f1sFile.ostrm, &oneSecData);
            oneSecData.clear();
        }

        // 接收数据+1
        dataCount++;
    }
}
