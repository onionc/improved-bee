#include "XString.h"


/**
 *自定义的字符串库，包含一些字符串转换操作
 */
namespace XString
{
    // 字符相关处理
    const char* wcharToChar(const wchar_t *wch)
    {
        char *ch;
        int len = WideCharToMultiByte(CP_ACP, 0, wch, wcslen(wch), NULL, 0, NULL, NULL);
        ch = new char[len + 1];
        WideCharToMultiByte(CP_ACP, 0, wch, wcslen(wch), ch, len, NULL, NULL);
        ch[len] = '\0';
        return ch;
    }
    const wchar_t* charToWChar(const char *ch)
    {
        wchar_t *wch;
        int len = MultiByteToWideChar(CP_ACP, 0, ch, strlen(ch), NULL, 0);
        wch = new wchar_t[len + 1];
        MultiByteToWideChar(CP_ACP, 0, ch, strlen(ch), wch, len);
        wch[len] = '\0';
        return wch;
    }
    void deleteCh(const char *ch)
    {
        delete[] ch;
    }
    void deleteCh(const wchar_t *wch)
    {
        delete[] wch;
    }

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
