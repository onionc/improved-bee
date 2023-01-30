#ifndef LUASCRIPT_H
#define LUASCRIPT_H


// lua 脚本处理：新增函数、组合字符串（或输出文件）、调用lua函数、测试

#include <QString>
#include <QDebug>
#include <lua/lua.hpp>

class LuaScript
{
    static int funcIndex; // 函数计数
    QString scriptStr; // 脚本字符串
    // lua
    lua_State* luaState;

public:
    LuaScript();

    // 清除函数
    void clearFunc(){scriptStr="";}

    /**
     * @brief addFunc 新增函数
     * @param funcStr 函数体字符串
     * @return  返回函数名
     */
    QString addFunc(QString funcStr);

    /**
     * @brief runFunc 调用函数
     * @param funcName 函数名
     * @param param Byte数组参数
     * @return 返回执行与否，todo: 返回值待定
     */
    bool runFunc(QString funcName, QByteArray param);
};

#endif // LUASCRIPT_H
