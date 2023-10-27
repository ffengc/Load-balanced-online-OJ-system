
#ifndef __YUFC_OJ_VIEW_HPP__
#define __YUFC_OJ_VIEW_HPP__

#include <iostream>
#include <string>
#include <ctemplate/template.h>

#include "./oj_model.hpp"

namespace ns_view
{
    using namespace ns_model;

    /*
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
    */
    const std::string template_path = "./template_html/";
    class View
    {
    public:
        View() {}
        ~View() {}

    public:
        void AllExpandHtml(const std::vector<struct Question> questions, std::string *html)
        {
            // 1. 形成路径
            std::string src_html = template_path + "all_questions.html";
            // 2. 形成数据字典
            ctemplate::TemplateDictionary root("all_questions");
            for (const auto &q : questions)
            {
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("question_list");
                sub->SetValue("number", q.__number);
                sub->SetValue("title", q.__title);
                sub->SetValue("star", q.__star);
            }
            // 3. 获取被渲染的html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);

            // 4. 执行渲染
            tpl->Expand(html, &root);
        }
        void OneExpandHtml(const struct Question &q, std::string *html)
        {
            std::string src_html = template_path + "one_question.html";
            ctemplate::TemplateDictionary root("one_question");
            root.SetValue("number", q.__number);
            root.SetValue("title", q.__title);
            root.SetValue("star", q.__star);
            root.SetValue("desc", q.__desc);
            root.SetValue("pre_code", q.__header);
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);
            tpl->Expand(html, &root);
        }
    };
}

#endif