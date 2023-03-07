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
        QString keyName = QString("Data_%1").arg(i,4,10,QLatin1Char('0')); // 四位数字填充0，为了在读取的时候顺序一致
        // 是否选定
        write.setValue(QString("%1/accumCheck").arg(keyName), info->accumCheck);

        // 名称、类型、数据
        write.setValue(QString("%1/name").arg(keyName), info->name);
        write.setValue(QString("%1/type").arg(keyName), info->type);
        write.setValue(QString("%1/data").arg(keyName), info->data);

        // 图表绘图项是否选定
        write.setValue(QString("%1/curve1").arg(keyName), info->curve1);
        write.setValue(QString("%1/curve2").arg(keyName), info->curve2);
        write.setValue(QString("%1/curve3").arg(keyName), info->curve3);
    }
}


// 从ini文件读取
void Parse::loadFromIni(QString readFilename, QVector<SProperty> *data, bool &frameLittleEndian,quint32 &frameHz){
    // 打开ini文件
    QSettings read(readFilename, QSettings::IniFormat);
    read.setIniCodec(QTextCodec::codecForName("UTF-8"));
    QStringList allGroups = read.childGroups();
    data->clear();
    foreach(QString groupKey, allGroups){
        if(groupKey.startsWith("Data")){
            qDebug()<<groupKey;
            // Data开头的为协议
            SProperty info;
            info.name = read.value(QString("%1/name").arg(groupKey)).toString();
            info.type = read.value(QString("%1/type").arg(groupKey)).toString();
            info.data = read.value(QString("%1/data").arg(groupKey)).toString();

            info.accumCheck = read.value(QString("%1/accumCheck").arg(groupKey)).toBool();
            info.curve1 = read.value(QString("%1/curve1").arg(groupKey)).toBool();
            info.curve2 = read.value(QString("%1/curve2").arg(groupKey)).toBool();
            info.curve3 = read.value(QString("%1/curve3").arg(groupKey)).toBool();
            data->push_back(info);
        }else if(groupKey=="endian"){
            // util::readIni(loadFilename, QString("%1/endian").arg(INI_OTHER)).toBool();
            frameLittleEndian = read.value(QString("%1/endian").arg(INI_OTHER)).toBool();
        }else if(groupKey=="hz"){
            frameHz = read.value(QString("%1/hz").arg(INI_OTHER)).toUInt();
        }
    }
}




// 协议帧信息解析：查找帧头、校验、帧长
bool Parse::parseFrameInfo(const QVector<SProperty> *frameInfoData, QString &errorMsg, uint &chartNum){

    int itmp = 0;
    QString tmp;
    bool ok;
    errorMsg = "";

    if(frameInfoData->size()<=0){
        errorMsg = QString("帧协议为空");
        return false;
    }

    // 初始化帧信息
    frameLen = frameHeaderLen = frameDataLen = frameCheckLen = 0;
    frameCheckSumType = EnumClass::None;
    frameHeaderArr.clear();
    frameDataArr.clear();

    luaScirpt.clearFunc(); // lua 清空函数

    QString lastName = ""; // 最后一个名称，用来判断多帧头

    // 绘图项计数，每个图最多允许三条数据，其他忽略
    int curve1Count=0, curve2Count=0, curve3Count=0;

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
        }else if(info->name==FRAME_CHECK){ // 校验信息
            frameCheckSumType = str2CheckSum(info->data);
            switch(frameCheckSumType){
                case EnumClass::None:
                    // 不做具体校验，只是为了有默认值
                    break;
                case EnumClass::c_add8:
                    frameCheckLen = 1;
                    break;
                case EnumClass::c_xor8:
                    frameCheckLen = 1;
                    break;
                case EnumClass::c_crc16_xmodem:
                    frameCheckLen = 2;
                    break;
                default:
                    errorMsg = QString("校验类型设置错误");
                    return false;
            }

        }else{ // 数据
            NAV_Data nav;
            // 数据名称
            nav.name = info->name;

            // 数据类型
            EnumClass::typeListEnum type = str2Type(info->type);
            nav.type = type;

            switch(type){
                case EnumClass::t_char:
                    frameDataLen++;
                    nav.bytesLen = 1;
                    break;
                case EnumClass::t_uchar:
                    frameDataLen++;
                    nav.bytesLen = 1;
                    break;
                case EnumClass::t_short:
                    frameDataLen+=2;
                    nav.bytesLen = 2;
                    break;
                case EnumClass::t_ushort:
                    frameDataLen+=2;
                    nav.bytesLen = 2;
                    break;
                case EnumClass::t_int:
                    frameDataLen+=4;
                    nav.bytesLen = 4;
                    break;
                case EnumClass::t_uint:
                    frameDataLen+=4;
                    nav.bytesLen = 4;
                    break;
                case EnumClass::t_float:
                    frameDataLen+=4;
                    nav.bytesLen = 4;
                    break;
                case EnumClass::t_double:
                    frameDataLen+=8;
                    nav.bytesLen = 8;
                    break;
                default:
                    break;
            }

            // 是否有脚本
            if(!info->data.isEmpty()){
                nav.extFuncName = luaScirpt.addFunc(info->data);
                if(nav.extFuncName.isEmpty()){
                    errorMsg = QString("脚本错误");
                    return false;
                }
            }

            // 累加标志
            nav.accumFlag = info->accumCheck;

            // 绘图标志
            if(info->curve1 && curve1Count<3){
                curve1Count++;
                nav.curve1Index = curve1Count;
            }

            frameDataArr.push_back(nav);
        }

        lastName = info->name;
    }

    frameHeaderLen = frameHeaderArr.size();
    frameLen = frameHeaderLen + frameDataLen + frameCheckLen;

    if(frameHeaderLen<=0 || frameDataLen<=0){
        errorMsg = QString("必须有帧头和数据");
        return false;
    }

    // lua 加载脚本
    if(!luaScirpt.load()){
        errorMsg = QString("脚本加载错误");
        return false;
    }

    // 将三个图表的线条数量返回，十进制的一二三位分别表示
    chartNum = curve1Count;

    return true;
}


// 找到一帧数据并解析
bool Parse::findFrameAndParse(QByteArray &allBytes){
    if(allBytes.size()<frameLen){
        return false;
    }

    int startIndex=-1; // 帧头索引
    int dataIndex=0; // 数据头索引，默认为0，检查allBytes,特殊情况下，设置其他值
    //qDebug()<<allBytes;
    // 找帧头的第一个字节
    startIndex = allBytes.indexOf(frameHeaderArr);
    QByteArray t;
    if(startIndex<0){
        // 未找到完整帧头，继续查看末尾是否有部分帧头
        int j=0;
        while(++j<frameHeaderLen){
            // 继续找
            t = allBytes.mid(dataIndex);
            startIndex = t.indexOf(frameHeaderArr.mid(0, frameHeaderLen-j));

            if(startIndex>=0){
                // 找到子串

                if(startIndex+(frameHeaderLen-j)==t.size()){
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


    // 拆分为 数据 和 校验字节
    QByteArray frameBytesData = allBytes.mid(startIndex+frameHeaderLen, frameDataLen); // 数据
    QByteArray frameBytesCehck = allBytes.mid(startIndex+frameHeaderLen+frameDataLen, frameCheckLen); // 数据

    // 检验数据，暂时默认校验的位从帧头后到校验数据之间的数据
    if(frameCheckSumType!=EnumClass::None){
        if(!checkData(frameBytesData, frameBytesCehck)){
            qDebug()<<"check failed";
            allBytes.remove(0, startIndex+frameHeaderLen); // 清除帧头数据
            return false;
        }else{

        }
    }

    // 解析数据，（只需要数据部分，不要帧头和校验）
    bool r = parseFrameData(frameBytesData);
    allBytes.remove(0, startIndex+frameLen);

    return r;
}


// 校验数据
bool Parse::checkData(const QByteArray &checkDataBytes, const QByteArray &checkBytes){

    int size = checkDataBytes.size();
    int cSize = checkBytes.size();
    if(cSize!=frameCheckLen){
        return false;
    }

    quint8 result[4]={0x00}, r[4]={0x00};
    unsigned short crcValue;

    switch(frameCheckSumType){
        case EnumClass::c_add8:
            if(cSize==1){
                result[0] = checkBytes.at(0);
                for(int i=0; i<size; i++){
                    r[0]+=(quint8)checkDataBytes[i];
                };
                if(r[0] == result[0]){
                    return true;
                }
            }

            break;
        case EnumClass::c_xor8:
            if(cSize==1){
                result[0] = checkBytes.at(0);
                for(int i=0; i<size; i++){
                    r[0]^=(quint8)checkDataBytes[i];
                };
                if(r[0] == result[0]){
                    return true;
                }
            }
            break;
        case EnumClass::c_crc16_xmodem:
            if(cSize==2){
                if(util::smallEndian){
                    // 小端，先收到的字节在前
                    result[0] = checkBytes.at(0);
                    result[1] = checkBytes.at(1);
                }else{
                    // 大端
                    result[0] = checkBytes.at(1);
                    result[1] = checkBytes.at(0);
                }

                crcValue = Crc16Xmode(checkDataBytes.data(), frameDataLen);
                r[0]=crcValue & 0x00FF;
                r[1]=(crcValue & 0xFF00)>>8;

                if(r[0]==result[0] && r[1]==result[1]){
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return false;
}

// 解析一帧数据
bool Parse::parseFrameData(const QByteArray &frameBytesData){
    int len = frameBytesData.size();
    int size = frameDataArr.size();
    // len是字节数，所以这里的size==frameDataLen；size 是字段个数，每个字段可以有多个字节。不能用 frameDataArr.size() 来比对。
    if(len!=frameDataLen){
        return false;
    }

    quint8 tByte;
    NAV_Data *nav;


    // 分配字节
    for(int i=0, jLen=0; i<size; i++){
        nav = &frameDataArr[i];
        if((jLen+nav->bytesLen) <= frameDataLen){ // jLen用来当数据的索引
            nav->buf = frameBytesData.mid(jLen, nav->bytesLen);
            jLen += nav->bytesLen;
        }else{
            return false;
        }

    }


    quint8 * dp;
    double result;
    // 遍历数据项
    for(int i=0; i<frameDataArr.size(); i++){
        nav = &frameDataArr[i];
        // qDebug()<<nav->name<<"-0x"<<nav->buf.toHex();


        dp = (quint8*)nav->buf.data();
        switch(nav->type){
            case EnumClass::t_char:
                nav->data.t_char = qint8(*dp);
                if(!nav->extFuncName.isEmpty()){
                    if(luaScirpt.runFunc(nav->extFuncName, nav->data.t_char, dp, 1, result)){
                        nav->data.t_double = result;
                    }else{
                        nav->data.t_double = nav->data.t_char;
                    }
                }
                break;
            case EnumClass::t_uchar:
                nav->data.t_uchar = quint8(*dp);
                if(!nav->extFuncName.isEmpty()){
                    if(luaScirpt.runFunc(nav->extFuncName, nav->data.t_uchar, dp, 1, result)){
                        nav->data.t_double = result;
                    }
                }
                break;
            case EnumClass::t_short:
                nav->data.t_short = util::bytes2short(dp);
                if(!nav->extFuncName.isEmpty()){
                    if(luaScirpt.runFunc(nav->extFuncName, nav->data.t_short, dp, 2, result)){
                        nav->data.t_double = result;
                    }
                }
                break;
            case EnumClass::t_ushort:
                nav->data.t_ushort = util::bytes2ushort(dp);
                if(!nav->extFuncName.isEmpty()){
                    if(luaScirpt.runFunc(nav->extFuncName, nav->data.t_ushort, dp, 2, result)){
                        nav->data.t_double = result;
                    }
                }
                break;
            case EnumClass::t_int:
                nav->data.t_int = util::bytes2int(dp);
                if(!nav->extFuncName.isEmpty()){
                    if(luaScirpt.runFunc(nav->extFuncName, nav->data.t_int, dp, 4, result)){
                        nav->data.t_double = result;
                    }
                }
                break;
            case EnumClass::t_uint:
                nav->data.t_uint = util::bytes2uint(dp);
                if(!nav->extFuncName.isEmpty()){
                    if(luaScirpt.runFunc(nav->extFuncName, (double)nav->data.t_uint, dp, 4, result)){ // uint 处理较少，不专门处理，直接传给double
                        nav->data.t_double = result;
                    }
                }
                break;
            case EnumClass::t_float:
                nav->data.t_float = util::bytes2float(dp);
                if(!nav->extFuncName.isEmpty()){
                     if(luaScirpt.runFunc(nav->extFuncName, nav->data.t_float, dp, 4, result)){
                         nav->data.t_double = result;
                     }
                }
                break;
            case EnumClass::t_double:
                nav->data.t_double = util::bytes2double(dp);
                if(!nav->extFuncName.isEmpty()){
                    if(luaScirpt.runFunc(nav->extFuncName, nav->data.t_double, dp, 8, result)){
                        nav->data.t_double = result;
                    }
                }
                break;
            default:
                PLOGD<<"类型错误";
                return false;
                break;
        }

    }

    return true;
}


// CRC16 协议解析 (CCITT-Xmodem)
unsigned short Parse::Crc16Xmode(const char *q, int len)
{
    static  unsigned short ccitt_table[256] = {
        0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
        0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
        0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
        0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
        0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
        0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
        0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
        0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
        0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
        0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
        0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
        0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
        0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
        0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
        0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
        0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
        0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
        0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
        0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
        0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
        0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
        0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
        0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
        0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
        0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
        0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
        0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
        0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
        0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
        0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
        0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
        0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
    };
    unsigned short crc = 0;
    while (len-- > 0)
        crc = ccitt_table[(crc >> 8 ^ *q++) & 0xff] ^ (crc << 8);
    return crc;
}


// 帧数据写入文件
void Parse::writeFile(std::ofstream *fp, const QVector<NAV_Data>* navData, bool addHeader){
    QString header="", line="";
    int size = navData->size();
    for(int i=0; i<size; i++){
        // 标题栏
        if(addHeader){
            header += navData->value(i).name;

            if(i<size-1){
                header.append(", ");
            }
        }

        line += navData->value(i).getDataStr();

        // 最后一列数据无逗号
        if(i<size-1){
            line.append(", ");
        }
    }

    if(addHeader){
        *fp<<qPrintable(header)<<"\r\n";
    }

    *fp<<line.toStdString()<<"\r\n"<<std::flush;
}
