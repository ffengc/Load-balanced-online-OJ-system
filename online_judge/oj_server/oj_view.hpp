/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

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
            std::string __number; // Question number (unique)
            std::string __title; // Question title
            std::string __star; // Question difficulty // Easy/Medium/Difficult
            int __cpu_limit; // Question time requirement
            int __mem_limit; // Question space requirement
            std::string __desc; // Question description
            std::string __header; // Preset code
            std::string __tail; // Question test case, needs to be spliced ​​with header to form a complete code
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
            // 1. Forming a path
            std::string src_html = template_path + "all_questions.html";
            // 2. Forming a data dictionary
            ctemplate::TemplateDictionary root("all_questions");
            for (const auto &q : questions)
            {
                ctemplate::TemplateDictionary *sub = root.AddSectionDictionary("question_list");
                sub->SetValue("number", q.__number);
                sub->SetValue("title", q.__title);
                sub->SetValue("star", q.__star);
            }
            // 3. Get the rendered html
            ctemplate::Template *tpl = ctemplate::Template::GetTemplate(src_html, ctemplate::DO_NOT_STRIP);

            // 4. Perform rendering
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