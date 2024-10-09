#pragma once

#include <iostream>
#include <string>
#include "util.hpp"

namespace ns_log
{
    using namespace ns_util;
    // 日志等级
    enum
    {
        INFO,    // 常规信息
        DEBUG,   // 调试信息
        WARNING, // 告警信息
        ERROR,   // 错误信息
        FATAL,   // 严重错误信息
    };
    // 返回值：[日志等级][文件名][行号]
    inline std::ostream &Log(const std::string level, const std::string file_name, int line)
    {
        // 添加日志等级
        std::string msg = "[";
        msg += level;
        msg += "]";
        // 添加报错文件名称
        msg += "[";
        msg += file_name;
        msg += "]";
        // 添加报错行
        msg += "[";
        msg += std::to_string(line);
        msg += "]";
        // 日志时间戳
        msg += "[";
        msg += TimeUtil::GetTimeStamp();
        msg += "]";
        // 将msg加入到cout的缓冲区
        std::cout << msg;
        return std::cout;
    }
// 定义为宏
#define LOG(level) Log(#level, __FILE__, __LINE__)
}
