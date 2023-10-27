

#ifndef __YUFC_COMPILER_HPP__
#define __YUFC_COMPILER_HPP__

#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../comm/util.hpp"
#include "../comm/log.hpp"

// 只负责代码的编译

namespace ns_compiler
{
    // 引入路径拼接功能
    using namespace ns_util;
    using namespace ns_log;
    class Compiler
    {
    public:
        Compiler() {}
        ~Compiler() {}
        static bool Compile(const std::string &file_name)
        {
            /*
                return value:
                true: 编译成功
                false: 编译失败
            */
            /*
                我们认为，传进来的file_name是没有后缀的
                所以: file_name: 1234
                1234 -> ./temp/1234.cpp
                1234 -> ./temp/1234.exe
                1234 -> ./temp/1234.stderr
            */
            pid_t child_pro_pid = fork();
            if (child_pro_pid < 0)
            {
                LOG(ERROR) << "Internal error, creating child process failed"
                           << "\n";
                return false; // 创建子进程失败 -> 编译失败
            }
            else if (child_pro_pid == 0)
            {
                // 重定向错误信息
                umask(0);
                int fd_stderr = open(PathUtil::CompilerError(file_name).c_str(), O_CREAT | O_WRONLY, 0777);
                if (fd_stderr < 0)
                {
                    LOG(WARNING) << "create .stderr file error"
                                 << "\n";
                    exit(1);
                }
                // 重定向标准错误到 fd_stderr
                dup2(fd_stderr, 2);

                // 子进程: 调用编译器，完成对代码的编译工作
                // g++ -o target src -std=c++11
                execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str(), \
                       PathUtil::Src(file_name).c_str(), "-std=c++11", "-D", "COMPILER_ONLINE", nullptr); // 不要忘记这个nullptr
                LOG(ERROR) << "运行g++的时候出现编译错误, 可能是参数问题"
                           << "\n";
                exit(2);
            }
            else
            {
                waitpid(child_pro_pid, nullptr, 0);
                // 编译是否成功？
                if (FileUtil::IsFileExists(PathUtil::Exe(file_name)))
                {
                    LOG(INFO) << PathUtil::Src(file_name) << " 编译成功"
                              << "\n";
                    return true;
                }
            }
            LOG(ERROR) << "编译错误, 没有生成exe文件"
                       << "\n";
            return false;
        }
    };
}

#endif