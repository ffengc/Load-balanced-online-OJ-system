/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

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
            // Status code -> corresponding description
            std::string desc;
            switch (code)
            {
            case 0:
                desc = "Compilation and execution successful";
                break;
            case -1:
                desc = "The submitted code is empty";
                break;
            case -2:
                desc = "Unknown error";
                break;
            case -3:
                // desc = "An error occurred during compilation";
                FileUtil::ReadFile(PathUtil::CompilerError(file_name), &desc, true);
                break;
            case SIGABRT:
                desc = "Memory out of bounds";
                break;
            case SIGXCPU:
                desc = "CPU usage timeout";
                break;
            case SIGFPE:
                desc = "Floating-point overflow";
                break;
            default:
                desc = "Unknown error (code: " + std::to_string(code) + ")";
                break;
            }
            return desc;
        }
        static void RemoveTempFile(const std::string &file_name)
        {
            // The number of files to clean is uncertain, but we know which ones are there?
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
            Input parameters:
                input: The input corresponding to the code submitted by the user
                code: The input corresponding to the code submitted by the user, not processed
                cpu_limit: Time requirement
                mem_limit: Space requirement
            Output parameters:
                status: Status code
                reason: Request result
                stdout: The result after my program has run
                stderr: Error result after running
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
            reader.parse(in_json, in_value); // Handle error checking last
            // Code and input
            std::string code = in_value["code"].asString();
            std::string input = in_value["input"].asString(); // Not processed
            // Time limit and space limit
            int cpu_limit = in_value["cpu_limit"].asInt();
            int mem_limit = in_value["mem_limit"].asInt();

            // Variables cannot be defined within the scope of a goto jump
            // Status code to return to the upper layer
            int status_code = 0;
            // Status code returned by Runner
            int runner_rt_code = 0;
            // Unique file name
            std::string file_name;

            // Build a final JSON to return to the user
            Json::Value out_value;

            if (code.size() == 0)
            {
                status_code = -1; // The file is empty
                goto END;
            }
            // Generate a unique file name and write the code to a temporary file
            // Use millisecond timestamp + atomic increment for uniqueness
            file_name = FileUtil::UniqFileName();
            if (!FileUtil::WriteFile(PathUtil::Src(file_name), code)) // Generate temporary src source file
            {
                status_code = -2; // Unknown error
                goto END;
            }
            if (!Compiler::Compile(file_name))
            {
                status_code = -3; // Compilation error
                goto END;
            }
            runner_rt_code = Runner::Run(file_name, cpu_limit, mem_limit);
            if (runner_rt_code < 0)
            {
                status_code = -2; // Unknown error
                goto END;
            }
            else if (runner_rt_code > 0)
            {
                status_code = runner_rt_code; // Program crashed during execution
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
                // The entire process was successful
                std::string _stdout_content;
                FileUtil::ReadFile(PathUtil::Stdout(file_name), &_stdout_content, true);
                // stdout needs true here, otherwise the stdout presented to the user would be all on one line
                out_value["stdout"] = _stdout_content;

                std::string _stderr_content;
                FileUtil::ReadFile(PathUtil::Stderr(file_name), &_stderr_content, true);
                out_value["stderr"] = _stderr_content;
            }
            Json::StyledWriter writer;
            *out_json = writer.write(out_value);

            RemoveTempFile(file_name); // Remove all temporary files
        }
    };
} // namespace ns_compile_run

#endif