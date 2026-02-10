/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

#ifndef __YUFC_RUNNER__
#define __YUFC_RUNNER__

#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "../comm/log.hpp"
#include "../comm/util.hpp"

namespace ns_runner
{
    using namespace ns_log;
    using namespace ns_util;
    class Runner
    {
    public:
        Runner() {}
        ~Runner() {}

    public:
        static void SetProcLimit(int cpu_limit, int mem_limit)
        {
            /* Provides an interface to set the process resource usage size */
            /* mem_limit is in kb units */
            struct rlimit cpu_rlimit;
            cpu_rlimit.rlim_max = RLIM_INFINITY;
            cpu_rlimit.rlim_cur = cpu_limit;
            setrlimit(RLIMIT_CPU, &cpu_rlimit);

            struct rlimit mem_rlimit;
            mem_rlimit.rlim_max = RLIM_INFINITY;
            mem_rlimit.rlim_cur = mem_limit * 1024; // Convert to kb
            setrlimit(RLIMIT_AS, &mem_rlimit);
        }

    public:
        // Specify the filename only, no need to include the path
        static int Run(const std::string &file_name, int cpu_limit, int mem_limit)
        {
            /*
                cpu_limit: Maximum CPU resources available when the program is running
                mem_limit: Maximum memory resources available when the program is running
            */
            /*
                return value: status & 0x7F
                >0 means the program encountered an exception, the child process received a signal
                The return value is the corresponding signal number!
                ==0 Program ran to completion, result saved in a temporary file, whether it passed test cases is not our concern
                <0 Internal error (e.g., file opening failure, child process creation failure)
            */
            /*
                After the program runs:
                1. Code ran to completion, result correct
                2. Code ran to completion, result incorrect
                3. Code did not complete, exception occurred
                Does run need to consider whether the code ran correctly or not? No!
                Determining if the result is correct is decided by the test cases!
                So this part only considers whether the code ran to completion correctly

                Additionally, we must know, who is the executable?
                A program at default startup:
                Standard input: not handled, self-testing not considered
                Standard output: What the result is after program completion
                Standard error: Runtime error information (different from compile-time error information)
                We hope that these three outputs can be redirected to files
            */
            std::string _execute_file_name = PathUtil::Exe(file_name);
            std::string _stdin_file_name = PathUtil::Stdin(file_name);
            std::string _stdout_file_name = PathUtil::Stdout(file_name);
            std::string _stderr_file_name = PathUtil::Stderr(file_name);

            // Open these files
            umask(0);
            int _stdin_fd = open(_stdin_file_name.c_str(), O_CREAT | O_RDONLY, 0777);
            int _stdout_fd = open(_stdout_file_name.c_str(), O_CREAT | O_WRONLY, 0777);
            int _stderr_fd = open(_stderr_file_name.c_str(), O_CREAT | O_WRONLY, 0777);

            // Ensure successful opening
            if (_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0)
            {
                LOG(ERROR) << "Failed to open standard files at runtime"
                           << "\n";
                return -1; // Indicates file opening failure
            }

            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "Failed to create child process at runtime"
                           << "\n";
                // Failed to create child process
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                return -2; // Indicates child process creation failure
            }
            else if (pid == 0)
            {
                // Child process
                // Perform three redirections
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);
                // Set resource limits
                SetProcLimit(cpu_limit, mem_limit);
                execlp(_execute_file_name.c_str(), _execute_file_name.c_str(), nullptr);
                exit(1); // If there is an error, it will come here, set to 1
            }
            else
            {
                // Parent process
                // Parent does not care about these file descriptors
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                int status = 0;
                waitpid(pid, &status, 0);
                // If the program ran abnormally, it must have received a signal
                LOG(INFO) << "Run completed, info: " << (status & 0x7F) << "\n";
                return status & 0x7F;
            }
        }
    };
}

#endif