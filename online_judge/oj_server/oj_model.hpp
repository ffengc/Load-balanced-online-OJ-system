

#ifndef __YUFC_OJ_MODEL_HPP__
#define __YUFC_OJ_MODEL_HPP__

#include <iostream>
#include <string>
#include <unordered_map>
#include <cassert>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include "../comm/log.hpp"
#include "../comm/util.hpp"

// 根据 question.list 文件，加载所有的题目信息到内存中
// model: 主要用来和数据进行交互，对外提供访问数据的接口

namespace ns_model
{
    using namespace ns_log;
    using namespace ns_util;

    struct Question
    {
    public:
        std::string __number; // 题目编号（唯一的）
        std::string __title;  // 题目的标题
        std::string __star;   // 题目的难度 // 简单/中等/困难
        int __cpu_limit;      // 题目时间要求
        int __mem_limit;      // 题目的空间要求
        std::string __desc;   // 题目的描述
        std::string __header; // 预设的代码
        std::string __tail;   // 题目的测试用例，需要和header拼接，形成完整的代码
    };

    const std::string question_list_root = "./questions/question.list";
    const std::string question_path = "./questions/";
    class Model
    {
    private:
        std::unordered_map<std::string, Question> __questions;

    public:
        Model()
        {
            assert(LoadQuestionList(question_list_root));
        }
        ~Model() {}

    public:
        bool LoadQuestionList(const std::string &question_list)
        {
            /*
                加载配置文件: questions/question.list + 题目编号文件
            */

            std::ifstream in(question_list);
            if (!in.is_open())
            {
                LOG(FATAL) << "加载题库失败，请检查是否存在题库文件"
                           << "\n";
                return false;
            }
            // 按行读取
            std::string line;
            while (std::getline(in, line))
            {
                // 以空格为分割符
                // 切分字符串
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, " ");
                if (tokens.size() != 5) // 说明切分有问题
                {
                    LOG(WARNING) << "加载部分题目失败，请检查文件格式"
                                 << "\n";
                    continue;
                }
                Question q;
                q.__number = tokens[0];
                q.__title = tokens[1];
                q.__star = tokens[2];
                q.__cpu_limit = atoi(tokens[3].c_str());
                q.__mem_limit = atoi(tokens[4].c_str());

                std::string question_number_path = question_path;
                question_number_path += q.__number;
                question_number_path += "/";

                FileUtil::ReadFile(question_number_path + "desc.txt", &(q.__desc), true);
                FileUtil::ReadFile(question_number_path + "header.cpp", &(q.__header), true);
                FileUtil::ReadFile(question_number_path + "tail.cpp", &(q.__tail), true);

                __questions.insert({q.__number, q}); // 插入到哈希表中
            }
            LOG(INFO) << "加载题库 ... 成功！"
                      << "\n";
            in.close();
            return true;
        }
        bool GetAllQuestions(std::vector<Question> *out)
        {
            if (__questions.size() == 0)
            {
                LOG(ERROR) << "用户获取题库失败"
                           << "\n";
                return false;
            }
            for (const auto &q : __questions)
            {
                out->push_back(q.second); // 把哈希的丢到vector里面去
            }
            return true;
        }
        bool GetOneQuestions(const std::string &number, Question *q)
        {
            const auto &iter = __questions.find(number);
            if (iter == __questions.end())
            {
                LOG(ERROR) << "用户获取题目失败，题目编号: " << number << "\n";
                return false;
            }
            (*q) = iter->second;
            return true;
        }
    };
};

#endif