

#ifndef __YUFC_UTIL_HPP__
#define __YUFC_UTIL_HPP__

#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <atomic>
#include <iostream>
#include <fstream>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace ns_util
{
    class TimeUtil
    {
    public:
        static std::string GetTimeStamp()
        {
            struct timeval __time;
            gettimeofday(&__time, nullptr);
            return std::to_string(__time.tv_sec);
        }
        static std::string GetTimeMs()
        {
            struct timeval _time;
            gettimeofday(&_time, nullptr);
            return std::to_string(_time.tv_sec * 1000 + _time.tv_usec / 1000);
        }
    };

    const std::string temp_path_root = "./temp/"; // 全剧路径
    class PathUtil
    {
    public:
        static std::string AddSuffix(const std::string &file_name, const std::string &suffix)
        {
            std::string path_name = temp_path_root;
            path_name += file_name;
            path_name += suffix;
            return path_name;
        }

    public:
        // 构建源文件路径+后缀的完整文件名
        static std::string Src(const std::string &file_name)
        {
            /*
                我们认为传进来的这个file_name是不带任何后缀的。
            */
            return AddSuffix(file_name, ".cpp");
        }
        // 构建可执行程序路径+后缀的完整文件名
        static std::string Exe(const std::string &file_name)
        {
            return AddSuffix(file_name, ".exe");
        }
        // 构建标准错误路径+后缀的完整文件名
        static std::string CompilerError(const std::string &file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }
        // 上面三个是编译时所需要的文件
        // 下面三个是运行时所需要的文件
        static std::string Stdin(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdin");
        }
        static std::string Stdout(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stdout");
        }
        static std::string Stderr(const std::string &file_name)
        {
            return AddSuffix(file_name, ".stderr");
        }
    };
    class FileUtil
    {
    public:
        static bool IsFileExists(const std::string &path_name)
        {
            struct stat st;
            if (stat(path_name.c_str(), &st) == 0)
            {
                // 获取文件属性成功了
                return true;
            }
            return false;
        }
        static std::string UniqFileName()
        {
            // 定义一个C++11库 #include <atomic> 里面提供的一个原子性的计数器
            static std::atomic_uint id(0); // 这里要static，避免每次调用它都重新定义这个id
            id++;
            // 毫秒级时间戳+原子性递增唯一值：来保证唯一性
            std::string ms = TimeUtil::GetTimeMs(); // 得到毫秒级时间戳
            std::string uniq_id = std::to_string(id);
            return ms + "." + uniq_id;
        }
        static bool WriteFile(const std::string &target, const std::string &content)
        {
            std::ofstream out(target);
            if (!out.is_open())
            {
                return false;
            }
            out.write(content.c_str(), content.size());
            out.close();
            return true;
        }
        static bool ReadFile(const std::string &target, std::string *content, bool keep = false)
        {
            /*
                keep == false 表示不保留每一行的 "\n"
            */
            (*content).clear();
            std::ifstream in(target);
            if (!in.is_open())
            {
                return false;
            }
            std::string line;
            // getline不保存行分隔符 -- 这里有坑
            // getline 有些时候是需要保留\n的
            while (std::getline(in, line))
            {
                (*content) += line;
                (*content) += (keep) ? "\n" : "";
            }
            in.close();
            return true;
        }
    };
    class StringUtil
    {
    public:
        static void SplitString(const std::string &str, std::vector<std::string> *target, const std::string &sep)
        {
            /*
                str: 要切分的字符串
                target: 输出结果
                sep: 指定的分割符
            */
            // boost 库
            boost::split((*target), str, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
} // namespace ns_util

#endif