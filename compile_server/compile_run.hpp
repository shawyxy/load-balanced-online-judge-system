#pragma once

#include <jsoncpp/json/json.h>
#include <vector>

#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include "compiler.hpp"
#include "runner.hpp"

namespace ns_compile_run
{
    // 引入路径拼接和日志功能
    using namespace ns_util;
    using namespace ns_log;
    // 引入编译和运行模块
    using namespace ns_compiler;
    using namespace ns_runner;

    class CompileAndRun
    {
    public:
        CompileAndRun() {}
        ~CompileAndRun() {}

    public:
        // 清理临时文件
        static void RemoveTempFile(const std::string &file_name)
        {
            std::vector<std::string> files = {
                PathUtil::Src(file_name),
                PathUtil::CompilerError(file_name),
                PathUtil::Exe(file_name),
                PathUtil::Stdin(file_name),
                PathUtil::Stdout(file_name),
                PathUtil::Stderr(file_name)};

            for (const auto &file : files)
            {
                if (FileUtil::IsFileExists(file))
                {
                    unlink(file.c_str());
                }
            }
        }

        // 返回状态码对应的描述
        // code
        //     大于0：程序被信号中断
        //     等于0：程序运行成功
        //     小于0：编译失败或代码为空等
        static std::string CodeToDesc(int code, const std::string &file_name)
        {
            std::string desc;
            switch (code)
            {
            case 0:
                desc = "编译并运行成功";
                break;
            case -1:
                desc = "用户提交的代码为空";
                break;
            case -2:
                desc = "内部未知错误";
                break;
            case -3:
                // desc = "编译错误";
                FileUtil::ReadFile(PathUtil::CompilerError(file_name), &desc, true);
                break;
            case SIGABRT:
                desc = "内存超限";
                break;
            case SIGXCPU:
                desc = "CPU超时";
                break;
            case SIGFPE:
                desc = "算术错误（除零错误/浮点错误/溢出错误）";
                break;
            default:
                desc = "未知错误，code：" + std::to_string(code);
                break;
            }
            return desc;
        }
        // 解析 compile_server.cc 分发的 JSON 字符串，编译并运行代码
        // 输入参数：
        //     in_json：{
        //         "code": "用户提交的代码",
        //         "input": "题目对应的测试用例",
        //         "cpu_limit": CPU 时间限制,
        //         "mem_limit": 内存空间限制}
        // 输出参数（返回值）：
        //     out_json：{
        //  [必选] "status":"状态码",
        //  [必选] "reason":"状态码对应的描述",
        //  [可选] "stdout":"程序运行的标准输出",
        //  [可选] "stderr":"程序运行的标准错误",}
        static void Start(const std::string &in_json, std::string *out_json)
        {
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json, in_value); // 将JSON字符串转化为JSON对象，以便用key值

            std::string code = in_value["code"].asCString();
            std::string input = in_value["input"].asString();
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            int status_code = 0;   // 返回的状态码默认为0（一切正常）
            int run_status = 0;    // 运行后的状态码
            std::string file_name; // 唯一文件名
            Json::Value out_value; // 构建输出JSON对象

            // goto...END 之间定义变量可能会被忽略

            if (code.size() == 0)
            {
                status_code = -1; // 代码为空
                goto END;
            }
            // 为用户的所有临时文件构建一个唯一的名称（不带路径前缀和类型后缀）
            file_name = FileUtil::UniqFileName();

            // 将代码写入 .cpp 临时文件
            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code))
            {
                status_code = -2; // 未知错误
                goto END;
            }

            // 编译代码
            if (!Compiler::Compile(file_name))
            {
                status_code = -3; // 编译失败
                goto END;
            }

            // 运行代码，设置时空限制，返回值是运行时/运行后的状态码
            run_status = Runner::Run(file_name, cpu_limit, mem_limit);
            if (run_status < 0)
            {
                status_code = -2; // 未知内部错误（由Run自定义，可以打LOG调试）
            }
            else if (run_status > 0) // 系统信号中断
            {
                status_code = run_status;
            }
            else // 正常运行
            {
                status_code = 0;
            }

        END:
            out_value["status"] = status_code;                        // 返回状态码
            out_value["reason"] = CodeToDesc(status_code, file_name); // 返回状态码的描述
            // 程序完整地运行完毕
            if (status_code == 0)
            {
                // 运行程序的标准输出和标准错误的重定向文件都已经被 Runner::Run 创建
                // 路径：./temp/
                std::string _stdout;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout, true);
                out_value["stdout"] = _stdout;

                std::string _stderr;
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr, true);
                out_value["stderr"] = _stderr;
            }
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);
            // 清理临时文件
            RemoveTempFile(file_name);
        }
    };
}
