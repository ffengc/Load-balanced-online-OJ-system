/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

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

    const std::string temp_path_root = "./temp/"; // Global path
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
        // Build full filename with source file path + suffix
        static std::string Src(const std::string &file_name)
        {
            /*
                We assume that the incoming file_name has no suffix.
            */
            return AddSuffix(file_name, ".cpp");
        }
        // Build full filename with executable file path + suffix
        static std::string Exe(const std::string &file_name)
        {
            return AddSuffix(file_name, ".exe");
        }
        // Build full filename with compiler error path + suffix
        static std::string CompilerError(const std::string &file_name)
        {
            return AddSuffix(file_name, ".compile_error");
        }
        // The above three are files needed for compilation
        // The following three are files needed for runtime
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
                // File attributes retrieved successfully
                return true;
            }
            return false;
        }
        static std::string UniqFileName()
        {
            // Define an atomic counter provided by the C++11 <atomic> library
            static std::atomic_uint id(0); // This should be static, to avoid redefining this id every time it's called
            id++;
            // Millisecond timestamp + atomic increment unique value: to ensure uniqueness
            std::string ms = TimeUtil::GetTimeMs(); // Get millisecond timestamp
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
                keep == false means not preserving each line's "\n"
            */
            (*content).clear();
            std::ifstream in(target);
            if (!in.is_open())
            {
                return false;
            }
            std::string line;
            // getline does not save the line separator -- beware here
            // Sometimes getline needs to preserve \n
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
                str: The string to split
                target: Output result
                sep: Specified delimiter
            */
            // boost library
            boost::split((*target), str, boost::is_any_of(sep), boost::algorithm::token_compress_on);
        }
    };
} // namespace ns_util

#endif