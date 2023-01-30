#include "LuaScript.h"
#include "util.h"

int LuaScript::funcIndex = 0;

LuaScript::LuaScript()
{
    scriptStr = "";

    // lua 初始化
    luaState = luaL_newstate(); // open
    luaL_openlibs(luaState);
}

QString LuaScript::addFunc(QString funcStr){
    QString funcName=QString("func_%1").arg(funcIndex++);

    QString t = QString(R"(
function %1(...)
    local arr={...}
    if #arr<1 then
        return nil
    end

    data = arr[1] --计算好的数据
    -- arr[2]~. 原始数据，需要 0xff 再运算
    %3
end
)").arg(funcName).arg(funcStr);


qDebug()<<t;
PLOGD<<t;

    if(luaL_dostring(luaState, t.toStdString().c_str())){
        qDebug()<<"script check failed";
        return "";
    }

    scriptStr+=t;
    return funcName;
}

bool LuaScript::runFunc(QString funcName, QByteArray param){
    return true;
}
