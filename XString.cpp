#include "XString.h"


/**
 *自定义的字符串库，包含一些字符串转换操作
 */
namespace XString
{
    #ifdef QT
    QString charToQS(char *ch)
    {
        return QString(QLatin1String(ch));
    }

    const string QSToChar(QString qs){
        return qs.toStdString();
    }
    #endif

}
