#include "LuaScript.h"
#include "util.h"

int LuaScript::funcIndex = 0;

LuaScript::LuaScript()
{
    scriptStr = "";
}

QString LuaScript::addFunc(QString funcStr, int paramCount){
    if(paramCount<1 || paramCount>255) return "";
    QString funcName=QString("func_%1").arg(funcIndex++);
    QString paramStr = "";
    while(paramCount--){
        paramStr += QString("p%1").arg(paramCount);
        paramStr += " ,"[paramCount>0]; // 学的一个奇技淫巧
    }
    QString t = QString(R"(
function %1(%2)
    %3
end
)").arg(funcName).arg(paramStr).arg(funcStr);

    PLOGD<<t;
    scriptStr+=t;
    return funcName;
}

bool LuaScript::runFunc(QString funcName, QByteArray param){
    return true;
}
