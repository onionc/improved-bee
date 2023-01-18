#ifndef XSTRING_H
#define XSTRING_H

#define QT 1
#ifdef QT
#include <QString>
#endif

#include <string>
using namespace std;

namespace XString
{

    #ifdef QT
    QString charToQS(const char *ch);
    const string QSToChar(QString qs);
    #endif
}

#endif // XSTRING_H
