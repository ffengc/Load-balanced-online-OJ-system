/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

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

// According to the question.list file, load all the question information into memory
// Model: Mainly used to interact with data and provide an interface for accessing data

namespace ns_model
{
    using namespace ns_log;
    using namespace ns_util;

    struct Question
    {
    public:
        std::string __number; // Question number (unique)
        std::string __title;  // Question title
        std::string __star;   // Difficulty level of the question // Easy/Medium/Hard
        int __cpu_limit;      // CPU time limit for the question
        int __mem_limit;      // Memory space requirement for the question
        std::string __desc;   // Description of the question
        std::string __header; // Preset code for the question
        std::string __tail;   // Test cases for the question, need to be concatenated with header to form complete code
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
                Load configuration file: questions/question.list + question number file
            */

            std::ifstream in(question_list);
            if (!in.is_open())
            {
                LOG(FATAL) << "Failed to load question bank, please check if the question bank file exists"
                           << "\n";
                return false;
            }
            // Read line by line
            std::string line;
            while (std::getline(in, line))
            {
                // Use space as the delimiter
                // Split string
                std::vector<std::string> tokens;
                StringUtil::SplitString(line, &tokens, " ");
                if (tokens.size() != 5) // Indicates a split error
                {
                    LOG(WARNING) << "Failed to load some questions, please check file format"
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

                __questions.insert({q.__number, q}); // Insert into hash table
            }
            LOG(INFO) << "Question bank loaded ... Successfully!"
                      << "\n";
            in.close();
            return true;
        }
        bool GetAllQuestions(std::vector<Question> *out)
        {
            if (__questions.size() == 0)
            {
                LOG(ERROR) << "Failed to retrieve question bank"
                           << "\n";
                return false;
            }
            for (const auto &q : __questions)
            {
                out->push_back(q.second); // Put hash into vector
            }
            return true;
        }
        bool GetOneQuestions(const std::string &number, Question *q)
        {
            const auto &iter = __questions.find(number);
            if (iter == __questions.end())
            {
                LOG(ERROR) << "Failed to retrieve question, question number: " << number << "\n";
                return false;
            }
            (*q) = iter->second;
            return true;
        }
    };
};

#endif