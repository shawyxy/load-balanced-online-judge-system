#pragma once

#include <iostream>
#include <string>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fstream>
#include <atomic>

#include <boost/algorithm/string.hpp>

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

        // 重定向：作为可执行程序的标准输入（本来由键盘输入）
        static std::string Stdin(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }

        // 重定向：将可执行程序的标准输出重定向到文件（本来输出到屏幕）
        static std::string Stdout(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdout");
        }

        // 重定向：用于接收可执行程序的标准错误信息（本来输出到标准错误）
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
                // 获取属性成功，文件已经存在
                return true;
            }
            return false;
        }

        // 构建唯一文件名（不包含路径和后缀，只由编号和时间戳组成）
        static std::string UniqFileName()
        {
            static std::atomic_uint id(0);
            id++; // 原子计数器
            // 毫秒级时间戳+原子性递增唯一值: 来保证唯一性
            std::string ms = TimeUtil::GetTimeMs();   // 毫秒级时间戳
            std::string uniq_id = std::to_string(id); // 唯一id
            return ms + "_" + uniq_id;
        }

        // 将字符串格式的代码写入文件
        // 参数：
        //     target：被写入的文件名
        //     content：字符串格式的代码
        // 返回值：是否写入成功
        static bool WriteFile(const std::string &target, const std::string &content)
        {
            std::ofstream out(target);
            if (!out.is_open()) // 打开文件失败
            {
                return false;
            }
            out.write(content.c_str(), content.size()); // 写入文件
            out.close();
            return true;
        }

        // 把文件中的内容读取到缓冲区
        // 参数：
        //     target：被读取的文件名
        //     content：输出型参数，用于保存读取的内容
        //     keep：是否保留行尾的换行符'\n'
        // 返回值：是否读取成功
        static bool ReadFile(const std::string &target, std::string *content, bool keep = false)
        {
            (*content).clear(); // 清空缓冲区

            std::ifstream in(target);
            if (!in.is_open()) // 打开文件失败
            {
                return false;
            }
            std::string line;
            // getline不会读取换行符'\n'
            while (std::getline(in, line))
            {
                (*content) += line;
                (*content) += (keep ? "\n" : ""); // 手动添加换行符'\n'
            }
            in.close();
            return true;
        }
    };

    class StringUtil
    {
    public:
        // 字符串切割
        // 参数：
        //     str：要被切割的目标字符串
        //     target：输出型参数，用于保存切割后的字串
        //     sep：指定的分隔符
        static void SplitString(const std::string &str, std::vector<std::string> *target, const std::string &sep)
        {
            boost::split((*target), str, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
}
