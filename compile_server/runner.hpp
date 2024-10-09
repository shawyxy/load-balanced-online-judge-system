#pragma once

#include <iostream>
#include <sys/resource.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>

#include "../comm/util.hpp"
#include "../comm/log.hpp"

namespace ns_runner
{
    // 引入路径拼接和日志功能
    using namespace ns_util;
    using namespace ns_log;
    class Runner
    {
    public:
        Runner() {}
        ~Runner() {}

    public:
        // 提供设置进程占用资源大小的接口
        static void SetProcLimit(int _cpu_limit, int _mem_limit)
        {
            // 设置 CPU 时间限制（秒）
            struct rlimit cpu_rlimit;
            cpu_rlimit.rlim_max = RLIM_INFINITY; // 不设置硬限制
            cpu_rlimit.rlim_cur = _cpu_limit;
            setrlimit(RLIMIT_CPU, &cpu_rlimit);

            // 设置内存限制（KB）
            struct rlimit mem_rlimit;
            mem_rlimit.rlim_max = RLIM_INFINITY;
            mem_rlimit.rlim_cur = _mem_limit * 1024; // 转化成为 KB
            setrlimit(RLIMIT_AS, &mem_rlimit);
        }

        // 运行编译后的可执行程序，并通过设置资源限制和重定向标准输入、输出、错误流来控制进程。
        // 参数：
        //     file_name：需要运行的文件名（同源文件），不带路径前缀和类型后缀。
        //     cpu_limit：设置 CPU 时间限制（秒）。
        //     mem_limit：设置内存限制（KB）。
        // 返回值：
        //     大于 0：表示程序异常终止，值为信号编号。
        //     等于 0：表示程序正常运行结束，输出结果被写入到指定的标准输出文件中。
        //     小于 0：表示内部错误，比如无法创建子进程或打开文件失败。
        static int Run(const std::string &file_name, int cpu_limit, int mem_limit)
        {
            
            std::string _execute = PathUtil::Exe(file_name);
            std::string _stdin = PathUtil::Stdin(file_name);
            std::string _stdout = PathUtil::Stdout(file_name);
            std::string _stderr = PathUtil::Stderr(file_name);

            umask(0);
            // 打开输入/输出/错误文件，这些文件描述符用于重定向进程的标准输入、输出和错误流。
            int _stdin_fd = open(_stdin.c_str(), O_CREAT | O_RDONLY, 0644);
            int _stdout_fd = open(_stdout.c_str(), O_CREAT | O_WRONLY, 0644);
            int _stderr_fd = open(_stderr.c_str(), O_CREAT | O_WRONLY, 0644);

            if (_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0)
            {
                LOG(ERROR) << "运行时打开标准文件失败\n";
                return -1; // 代表打开文件失败
            }
            // 创建子进程
            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "运行时创建子进程失败\n";
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                return -2; // 代表创建子进程失败
            }
            else if (pid == 0)
            {
                // 重定向标准输入、输出、错误流
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);
                // 设置资源限制
                SetProcLimit(cpu_limit, mem_limit);
                // 执行可执行文件
                execl(_execute.c_str(), _execute.c_str(), nullptr);
                // 如果execl正常调用，不会执行下面的内容
                exit(1);
            }
            else
            {
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                int status = 0;
                // 父进程等待子进程结束
                waitpid(pid, &status, 0);
                // 程序正常运行到结束，status 为 0，否则不为 0
                LOG(INFO) << "运行完毕，status：" << (status & 0x7F) << "\n";
                return status & 0x7F;
            }
        }
    };
}