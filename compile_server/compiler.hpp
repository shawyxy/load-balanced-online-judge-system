#pragma once

#include <iostream>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>
#include <fcntl.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_compiler
{
    // 引入路径拼接和日志功能
    using namespace ns_util;
    using namespace ns_log;

    class Compiler
    {
    public:
        Compiler() {}
        ~Compiler() {}
    public:
        // 编译源文件
        // 参数：不带路径前缀和类型后缀的文件名
        // 返回值：编译成功与否
        static bool Compile(const std::string &file_name)
        {
            pid_t pid = fork();
            if (pid < 0) // 创建子进程失败
            {
                LOG(ERROR) << "内部错误：创建子进程失败\n";
                return false;
            }
            else if (pid == 0) // 子进程
            {
                umask(0); // 确保系统不会屏蔽文件权限
                // 打开/新建文件，用于保存输出到标准错误的内容，返回值是文件描述符
                // 构造完整的带路径和类型后缀的文件名
                int cperr_fd = open(PathUtil::CompilerError(file_name).c_str(), O_CREAT | O_WRONLY, 0644); // <fcntl.h>
                if (cperr_fd < 0) // 打开或创建文件失败
                {
                    LOG(WARNING) << "打开或创建 .compile_error 文件失败\n";
                    exit(1);
                }

                // 将标准错误（2）重定向到 .compile_error文件（cperr_fd）
                dup2(cperr_fd, 2);

                // 进程程序替换：调用g++编译器
                // 相当于：g++ -o file_name.exe file_name.cpp -D COMPILER_ONLINE -std=c++11
                execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str() /*.exe*/,
                       PathUtil::Src(file_name).c_str() /*.cpp*/, "-D" /*预处理宏*/,
                       "COMPILER_ONLINE", "-std=c++11", nullptr /*选项终止处*/);
                
                // 如果 execlp 执行成功，是不会走到这里的，而是执行完g++后终止
                LOG(ERROR) << "启动编译器g++失败，可能是参数错误\n";
                exit(2);
            }
            else // 父进程回收子进程资源
            {
                waitpid(pid, nullptr, 0);
                // 如果g++编译成功，则会生成.exe文件，如果没有生成，则说明编译失败
                if (FileUtil::IsFileExists(PathUtil::Exe(file_name)))
                {
                    LOG(INFO) << PathUtil::Src(file_name) << " 编译成功\n";
                    return true;
                }
                LOG(ERROR) << "编译失败，没有生成可执行程序\n";
                return false;
            }
        }
    };
}