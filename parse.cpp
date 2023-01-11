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




// 协议帧信息解析：查找帧头、校验、帧长
bool Parse::parseFrameInfo(const QVector<SProperty> *frameInfoData, QString &errorMsg){

    int itmp = 0;
    quint8 byte;
    QString tmp;
    bool ok;
    errorMsg = "";



    if(frameInfoData->size()<=0){
        errorMsg = QString("帧协议为空");
        return false;
    }

    QString lastName = "";
    for(int i=0; i<frameInfoData->size(); i++){
        const SProperty *info = &frameInfoData->at(i);

        /* 帧头校验的限制性说明
         * 1. 帧头必须有；帧头只能位于开头；可以有多个字节（因为帧头按字节一项项存，所以有多项）；
         * 2. 检验和可选，只能位于最后，只有一项（内部一或两个字节）（add8 xor8 crc16-ccitt）
         *
         */
        if(i==0 && info->name!=FRAME_HEADER){
            errorMsg = QString("必须有帧头信息，且必须在开头处");
            return false;
        }
        if(info->name==FRAME_HEADER && (lastName!="" && lastName!=FRAME_HEADER)){
            errorMsg = QString("多个帧头必须连续，且都放到开头处");
            return false;
        }
        if(info->name==FRAME_CHECK && i!=frameInfoData->size()-1){
            errorMsg = QString("检验和只能有一项，且必须位于结尾处");
            return false;
        }


        // 帧信息
        if(info->name==FRAME_HEADER){ // 帧头 byte
            itmp = info->data.toInt(&ok, 16) ;
            if(itmp<0 || itmp>0xff || !ok){
                errorMsg = QString("帧头设置错误，错误值：%1").arg(info->data);
                return false;
            }
            frameHeaderArr.push_back((quint8)itmp);
            frameLen++;
        }else if(info->name==FRAME_CHECK){ // 校验信息
            frameCheckSumType = str2CheckSum(info->data);
            switch(frameCheckSumType){
                case EnumClass::None:
                    // 不做具体校验，只是为了有默认值
                    break;
                case EnumClass::c_add8:
                    frameLen++;
                    break;
                case EnumClass::c_xor8:
                    frameLen++;
                    break;
                case EnumClass::c_crc16_xmodem:
                    frameLen+=2;
                    break;
                default:
                    errorMsg = QString("校验类型设置错误");
                    return false;
            }

        }else{ // 数据
            // 数据名称
            frameDataName.push_back(info->name);

            // 数据类型
            EnumClass::typeListEnum type = str2Type(info->type);
            frameDataType.append(type);
            switch(type){
                case EnumClass::t_char:
                    frameLen++;
                    break;
                case EnumClass::t_uchar:
                    frameLen++;
                    break;
                case EnumClass::t_short:
                    frameLen+=2;
                    break;
                case EnumClass::t_ushort:
                    frameLen+=2;
                    break;
                case EnumClass::t_int:
                    frameLen+=4;
                    break;
                case EnumClass::t_uint:
                    frameLen+=4;
                    break;
                case EnumClass::t_float:
                    frameLen+=4;
                    break;
                case EnumClass::t_double:
                    frameLen+=8;
                default:
                    break;
            }


        }

        lastName = info->name;
    }

    frameHeaderSize = frameHeaderArr.size();
    frameDataSize = frameDataType.size();

    if(frameHeaderSize<=0 || frameDataSize<=0){
        errorMsg = QString("必须有帧头和数据");
        return false;
    }

    return true;
}


// 解析一帧数据
bool Parse::parseFrameByte(QByteArray &allBytes){
    if(allBytes.size()<frameLen){
        return false;
    }

    bool frameHeaderOk = false;

    int startIndex=-1; // 帧头索引
    int dataIndex=0; // 数据头索引，默认为0，检查allBytes,特殊情况下，设置其他值
    //qDebug()<<allBytes;
    // 找帧头的第一个字节
    startIndex = allBytes.indexOf(frameHeaderArr);
    QByteArray t;
    if(startIndex<0){
        // 未找到完整帧头，继续查看末尾是否有部分帧头
        int j=0;
        while(++j<frameHeaderSize){
            // 继续找
            t = allBytes.mid(dataIndex);
            startIndex = t.indexOf(frameHeaderArr.mid(0, frameHeaderSize-j));

            if(startIndex>=0){
                // 找到子串

                if(startIndex+(frameHeaderSize-j)==t.size()){
                    // 并且在末尾
                    allBytes.remove(0, startIndex+dataIndex); // 清除 部分帧头 之前的数据
                    return false;
                }else{
                    /*
                     * 找到子串，但是不在末尾。其实还需要再在startIndex后面的子串中查找子串
                     * 比如在协议头 AA BB xx xx xx 下有数据 0x 11 22 33 AA AA ，应该保留数据的最后一个AA
                     *
                     */
                    dataIndex = startIndex+1;
                    j--; // 需要重新查同样的子串，撤回一个字节
                }
            }

        }
        allBytes.clear();
        return false;
    }



    // 剩余长度是否足够
    if((allBytes.size()-startIndex) < frameLen){
        qDebug()<<allBytes.size()<<"-"<<startIndex<<"<"<<frameLen;
        allBytes.remove(0, startIndex); // 清除帧头之前的数据
        return false;
    }

    // 检验数据，暂时默认从帧头后到校验数据之间的数据
    if(frameCheckSumType!=EnumClass::None){
        if(!checkData(allBytes.mid(startIndex, frameLen))){
            qDebug()<<"check failed";
            allBytes.remove(0, startIndex+frameHeaderSize); // 清除帧头数据
            return false;
        }
    }

    // todo 解析数据
    qDebug()<<"end"<<startIndex<<" "<<startIndex+frameLen;
    allBytes.remove(0, startIndex+frameLen);



    return true;
}


// 校验数据
bool Parse::checkData(const QByteArray &frameBytes){

    int size = frameBytes.size();
    if(size!=frameLen){
        return false;
    }

    quint8 result[4]={0x00}, r[4]={0x00};
    switch(frameCheckSumType){
        case EnumClass::c_add8:
            result[0] = frameBytes.at(frameLen-1);
            for(int i=frameHeaderSize; i<size-1; i++){
                r[0]+=(quint8)frameBytes[i];
            };
            if(r[0] == result[0]){
                return true;
            }
            break;
        case EnumClass::c_xor8:
            result[0] = frameBytes.at(frameLen-1);
            break;
        case EnumClass::c_crc16_xmodem:
            //frameBytes.mid(frameLen-2, 1);
            break;
        default:
            break;
    }
    return false;
}
