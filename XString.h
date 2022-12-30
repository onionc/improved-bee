#ifndef XSTRING_H
#define XSTRING_H

#define QT 1
#ifdef QT
#include <QString>
#endif

#include <windows.h>
#include <string>
using namespace std;

namespace XString
{
    const char* wcharToChar(const wchar_t *wch);
    const wchar_t* charToWChar(const char *ch);
    void deleteCh(const char *ch);
    void deleteCh(const wchar_t *wch);

    #ifdef QT
    QString charToQS(const char *ch);
    const string QSToChar(QString qs);
    #endif
}

#endif // XSTRING_H
