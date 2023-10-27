

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
            /* 提供设置进程占用资源大小的接口 */
            /* mem_limit 是 kb 单位 */
            struct rlimit cpu_rlimit;
            cpu_rlimit.rlim_max = RLIM_INFINITY;
            cpu_rlimit.rlim_cur = cpu_limit;
            setrlimit(RLIMIT_CPU, &cpu_rlimit);

            struct rlimit mem_rlimit;
            mem_rlimit.rlim_max = RLIM_INFINITY;
            mem_rlimit.rlim_cur = mem_limit * 1024; // 转化成kb
            setrlimit(RLIMIT_AS, &mem_rlimit);
        }

    public:
        // 指名文件名即可，不需要带上路径，不需要带上路径
        static int Run(const std::string &file_name, int cpu_limit, int mem_limit)
        {
            /*
                cpu_limit: 该程序运行的时候，可以使用的最大cpu资源上限
                mem_limit: 改程序运行的时候，可以使用的最大内存资源上限
            */
            /*
                return value: status & 0x7F
                >0 表示程序发生了异常，子进程收到了信号
                返回值就是对应的信号编号！
                ==0 正常运行完毕，结果保存到了临时文件当中，是否跑过测试用例，不关心
                <0 内部错误（比如打开文件失败，创建子进程失败）
            */
            /*
                程序运行完成之后：
                1. 代码跑完，结果正确
                2. 代码跑完，结果不正确
                3. 代码没跑完，异常
                run需要考虑代码跑完，结果正确与否吗？不需要！
                判断结果对不对，是由测试用例决定的！
                所以这部分只考虑代码是否正确运行完毕

                另外，我们必须知道，可执行程序是谁？
                一个程序在默认启动的时候：
                标准输入：不处理，不考虑自测的情况
                标准输出：程序运行完成，输出结果是什么
                标准错误：运行时错误信息（区别编译错误信息）
                我们希望，这三个输出，都可以重定向到文件中去
            */
            std::string _execute_file_name = PathUtil::Exe(file_name);
            std::string _stdin_file_name = PathUtil::Stdin(file_name);
            std::string _stdout_file_name = PathUtil::Stdout(file_name);
            std::string _stderr_file_name = PathUtil::Stderr(file_name);

            // 打开这几个文件
            umask(0);
            int _stdin_fd = open(_stdin_file_name.c_str(), O_CREAT | O_RDONLY, 0777);
            int _stdout_fd = open(_stdout_file_name.c_str(), O_CREAT | O_WRONLY, 0777);
            int _stderr_fd = open(_stderr_file_name.c_str(), O_CREAT | O_WRONLY, 0777);

            // 一定要保证打开成功
            if (_stdin_fd < 0 || _stdout_fd < 0 || _stderr_fd < 0)
            {
                LOG(ERROR) << "运行时打开标准文件失败"
                           << "\n";
                return -1; // 代表打开文件失败
            }

            pid_t pid = fork();
            if (pid < 0)
            {
                LOG(ERROR) << "运行时创建子进程失败"
                           << "\n";
                // 创建子进程失败
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                return -2; // 代表创建子进程失败
            }
            else if (pid == 0)
            {
                // 子进程
                // 进行三个重定向
                dup2(_stdin_fd, 0);
                dup2(_stdout_fd, 1);
                dup2(_stderr_fd, 2);
                // 设置资源限制
                SetProcLimit(cpu_limit, mem_limit);
                execlp(_execute_file_name.c_str(), _execute_file_name.c_str(), nullptr);
                exit(1); // 如果出错了就会到这里来，设置成1
            }
            else
            {
                // 父进程
                // 父进程不关注这些文件描述符
                close(_stdin_fd);
                close(_stdout_fd);
                close(_stderr_fd);
                int status = 0;
                waitpid(pid, &status, 0);
                // 程序运行异常，一定是因为收到了信号
                LOG(INFO) << "运行完毕, info: " << (status & 0x7F) << "\n";
                return status & 0x7F;
            }
        }
    };
}

#endif