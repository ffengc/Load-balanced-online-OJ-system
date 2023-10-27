

#ifndef __YUFC_COMPILE_RUN_HPP__
#define __YUFC_COMPILE_RUN_HPP__

#include "compiler.hpp"
#include "runner.hpp"
#include "../comm/log.hpp"
#include "../comm/util.hpp"
#include <jsoncpp/json/json.h>
#include <signal.h>
#include <unistd.h>

namespace ns_compile_run
{
    using namespace ns_log;
    using namespace ns_util;
    using namespace ns_compiler;
    using namespace ns_runner;
    class compile_run
    {
    public:
        static std::string CodeToDesc(int code, const std::string &file_name)
        {
            // 状态码 -> 对应的描述
            std::string desc;
            switch (code)
            {
            case 0:
                desc = "编译运行成功";
                break;
            case -1:
                desc = "用户提交的代码是空";
                break;
            case -2:
                desc = "未知错误";
                break;
            case -3:
                // desc = "编译时发生了错误";
                FileUtil::ReadFile(PathUtil::CompilerError(file_name), &desc, true);
                break;
            case SIGABRT:
                desc = "内存超过范围";
                break;
            case SIGXCPU:
                desc = "cpu使用超时";
                break;
            case SIGFPE:
                desc = "浮点数溢出";
                break;
            default:
                desc = "未知错误(code: " + std::to_string(code) + ")";
                break;
            }
            return desc;
        }
        static void RemoveTempFile(const std::string &file_name)
        {
            // 清理文件的个数是不确定，但是有哪些我们是知道的？
            std::string src_file_name = PathUtil::Src(file_name);
            if (FileUtil::IsFileExists(src_file_name))
                unlink(src_file_name.c_str());
            std::string compiler_error_file_name = PathUtil::CompilerError(file_name);
            if (FileUtil::IsFileExists(compiler_error_file_name))
                unlink(compiler_error_file_name.c_str());

            std::string execute_file_name = PathUtil::Exe(file_name);
            if (FileUtil::IsFileExists(execute_file_name))
                unlink(execute_file_name.c_str());

            std::string stdin_file_name = PathUtil::Stdin(file_name);
            if (FileUtil::IsFileExists(stdin_file_name))
                unlink(stdin_file_name.c_str());

            std::string stdout_file_name = PathUtil::Stdout(file_name);
            if (FileUtil::IsFileExists(stdout_file_name))
                unlink(stdout_file_name.c_str());

            std::string stderr_file_name = PathUtil::Stderr(file_name);
            if (FileUtil::IsFileExists(stderr_file_name))
                unlink(stderr_file_name.c_str());
        }

    public:
        /*
            输入参数:
                input: 用户给自己提交的代码对应的输入
                code: 用户提交的代码对应的输入，不做处理
                cpu_limit: 时间要求
                mem_limit: 空间要求
            输出参数:
                status: 状态码
                reason: 请求结果
                stdout: 我的程序运行完的结果
                stderr: 运行完的错误结果
        */
        /*
            in_json:
            {"code":"#include ...", "input":"", "cpu_limit":1, "mem_limit":10240}
            out_json:
            {"status":0, "reason":"", "stdout":"", "stderr":"", ...}
        */
        static void Start(const std::string &in_json, std::string *out_json)
        {
            Json::Value in_value;
            Json::Reader reader;
            reader.parse(in_json, in_value); // 最后再处理差错问题
            // 代码和输入
            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString(); // 不做处理
            // 时间限制和空间限制
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            // 因为goto跳转的区间不能定义变量
            // 返回给上层的状态码
            int status_code = 0;
            // Run返回的状态码
            int runner_rt_code = 0;
            // 唯一的文件名
            std::string file_name;

            // 构建一个最终给用户返回的json
            Json::Value out_value;

            if (code.size() == 0)
            {
                status_code = -1; // 文件为空
                goto END;
            }
            // 形成一个唯一文件名，然后把code写到临时文件里面去
            // 这里到时候采用毫秒级时间戳+原子性递增唯一值：来保证唯一性
            file_name = FileUtil::UniqFileName();
            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code)) // 形成临时src源文件
            {
                status_code = -2; // 未知错误
                goto END;
            }
            if (!Compiler::Compile(file_name))
            {
                status_code = -3; // 编译错误
                goto END;
            }
            runner_rt_code = Runner::Run(file_name, cpu_limit, mem_limit);
            if (runner_rt_code < 0)
            {
                status_code = -2; // 未知错误
                goto END;
            }
            else if (runner_rt_code > 0)
            {
                status_code = runner_rt_code; // 程序运行崩溃
                goto END;
            }
            else
            {
                status_code = 0;
            }
        END:
            out_value["status"] = status_code;
            out_value["reason"] = CodeToDesc(status_code, file_name);
            if (status_code == 0)
            {
                // 整个过程全部成功
                std::string _stdout_content;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout_content, true);
                // stdout这里是要true的，因为不然我们呈现给用户的stdout就是堆在一行了
                out_value["stdout"] = _stdout_content;

                std::string _stderr_content;
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr_content, true);
                out_value["stderr"] = _stderr_content;
            }
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);

            RemoveTempFile(file_name); // 删除所有的临时文件
        }
    };
} // namespace ns_compile_run

#endif