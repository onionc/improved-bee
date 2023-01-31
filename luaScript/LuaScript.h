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
    // 加载脚本
    bool load();

    /**
     * @brief addFunc 新增函数
     * @param funcStr 函数体字符串
     * @return  返回函数名
     */
    QString addFunc(QString funcStr);


    /**
     * @brief runFunc 调用函数
     * @param funcName 函数名
     * @param data 数据值 int/double
     * @param arr 数组
     * @param size 长度
     * @param result 结果值
     * @return 执行成功与否
     */
    bool runFunc(QString funcName, int data, const unsigned char* arr, size_t size, double &result);
    bool runFunc(QString funcName, double data, const unsigned char* arr, size_t size, double &result);
};

#endif // LUASCRIPT_H
