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

// 加载脚本
bool LuaScript::load(){
    PLOGD<<scriptStr;
    return luaL_dostring(luaState, scriptStr.toStdString().c_str())==0;
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


//PLOGD<<t;

    if(luaL_dostring(luaState, t.toStdString().c_str())){
        qDebug()<<"script check failed";
        return "";
    }

    scriptStr+=t;
    return funcName;
}

bool LuaScript::runFuncCommon(QString funcName, char type, int iData, double dData, const unsigned char* arr, size_t size, double &result){

    // 函数入栈
    if(lua_getglobal(luaState, funcName.toStdString().c_str())==0){
        goto end;
    }

    // 参数1入栈，数据值，需要判断为int/double
    if(type==1){
        lua_pushinteger(luaState, iData);
    }else{
        lua_pushnumber(luaState, dData);
    }

    // todo: byte数组入栈

    // 调用函数
    if(lua_pcall(luaState, 1, 1, 0)){ // state, 参数个数，返回值个数, errFunc
        goto end;
    }

    // 获取返回值
    if(lua_isnumber(luaState, -1)){
        result = lua_tonumber(luaState, -1);
    }else{
        result = 0;
    }
    lua_pop(luaState, -1); // 清除栈数据
    return true;

end:
    lua_pop(luaState, -1); // 清除栈数据
    return false;

}
bool LuaScript::runFunc(QString funcName, int data, const unsigned char* arr, size_t size, double &result){
    return runFuncCommon(funcName, 1, data, 0.0, arr, size, result);

}
bool LuaScript::runFunc(QString funcName, double data, const unsigned char* arr, size_t size, double &result){
    return runFuncCommon(funcName, 2, 0, data, arr, size, result);
}
