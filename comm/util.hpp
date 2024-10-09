#pragma once

#include <iostream>
#include <string>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <atomic>


namespace ns_util
{
    // 时间工具类
    class TimeUtil
    {
    public:
        // 获得秒级时间戳
        static std::string GetTimeStamp()
        {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            return std::to_string(tv.tv_sec);
        }
        // 获得毫秒级时间戳
        static std::string GetTimeMs()
        {
            struct timeval tv;
            gettimeofday(&tv, nullptr);
            return std::to_string(tv.tv_sec * 1000 + tv.tv_usec / 1000);
        }
    };

    // 路径前缀，用于临时存放编译生成的文件
    static const std::string temp_path = "./temp/";

    // 建名工具类
    class PathUtil
    {
    public:
        // 路径前缀+文件名+后缀
        // 文件名：test -> 构建后：./temp/test.cpp
        static std::string AddSuffix(const std::string &file_name, const std::string &suffix)
        {
            std::string path_name = temp_path;
            path_name += file_name;
            path_name += suffix;
            return path_name;
        }

        // C++源文件
        static std::string Src(const std::string &file_name)
        {
            return AddSuffix(file_name, ".cpp");
        }

        // 可执行程序文件
        static std::string Exe(const std::string &file_name)
        {
            return AddSuffix(file_name, ".exe");
        }

        // 编译错误文件
        static std::string CompilerError(const std::string &file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }

        // 重定向：程序运行时需要的临时输入文件（本来由键盘输入）
        static std::string Stdin(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }

        // 重定向：程序运行时需要的临时输出数据（本来输出到屏幕）
        static std::string Stdout(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdout");
        }

        // 重定向：程序运行时输出的错误（本来输出到标准错误）
        static std::string Stderr(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stderr");
        }
    };

    // 文件工具类
    class FileUtil
    {
    public:
        // 判断文件是否存在
        static bool IsFileExists(const std::string &path_name)
        {
            struct stat st;
            if (stat(path_name.c_str(), &st) == 0)
            {
                //获取属性成功，文件已经存在
                return true;
            }

            return false;
        }
    };

}
