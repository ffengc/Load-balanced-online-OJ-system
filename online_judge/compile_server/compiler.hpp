/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

#ifndef __YUFC_COMPILER_HPP__
#define __YUFC_COMPILER_HPP__

#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../comm/util.hpp"
#include "../comm/log.hpp"

// Responsible only for code compilation

namespace ns_compiler
{
    // Introduce path concatenation functionality
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
                true: Compilation successful
                false: Compilation failed
            */
            /*
                We assume that the incoming file_name has no suffix.
                So: file_name: 1234
                1234 -> ./temp/1234.cpp
                1234 -> ./temp/1234.exe
                1234 -> ./temp/1234.stderr
            */
            pid_t child_pro_pid = fork();
            if (child_pro_pid < 0)
            {
                LOG(ERROR) << "Internal error, creating child process failed"
                           << "\n";
                return false; // Failed to create child process -> Compilation failed
            }
            else if (child_pro_pid == 0)
            {
                // Redirect error information
                umask(0);
                int fd_stderr = open(PathUtil::CompilerError(file_name).c_str(), O_CREAT | O_WRONLY, 0777);
                if (fd_stderr < 0)
                {
                    LOG(WARNING) << "Failed to create .stderr file"
                                 << "\n";
                    exit(1);
                }
                // Redirect standard error to fd_stderr
                dup2(fd_stderr, 2);

                // Child process: Call the compiler to complete the compilation of the code
                // g++ -o target src -std=c++11
                execlp("g++", "g++", "-o", PathUtil::Exe(file_name).c_str(), \
                       PathUtil::Src(file_name).c_str(), "-std=c++11", "-D", "COMPILER_ONLINE", nullptr); // Don't forget this nullptr
                LOG(ERROR) << "Compilation error occurred when running g++, possibly due to parameter issues"
                           << "\n";
                exit(2);
            }
            else
            {
                waitpid(child_pro_pid, nullptr, 0);
                // Was compilation successful?
                if (FileUtil::IsFileExists(PathUtil::Exe(file_name)))
                {
                    LOG(INFO) << PathUtil::Src(file_name) << " Compiled successfully"
                              << "\n";
                    return true;
                }
            }
            LOG(ERROR) << "Compilation error, no .exe file generated"
                       << "\n";
            return false;
        }
    };
}

#endif