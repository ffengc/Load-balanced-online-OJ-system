/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

/* export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib */

#include <iostream>
#include <signal.h>
#include "../comm/httplib.h"
#include "./oj_control.hpp"

using namespace httplib;
using namespace ns_control;

static Control *g_ctrl_ptr = nullptr;

void Recovery(int signo)
{
    g_ctrl_ptr->RecoveryMachine();
}

int main()
{
    signal(SIGQUIT, Recovery);
    // Route functions for user requests
    Server svr;
    Control ctrl;
    g_ctrl_ptr = &ctrl;
    // Retrieve the list of all questions
    svr.Get("/all_questions", [&ctrl](const Request &req, Response &resp)
            {
        // Here I want to return a webpage containing the list of all questions
        std::string html;
        ctrl.AllQuestions(&html);

        resp.set_content(html, "text/html; charset=utf-8"); });
    // Users want to get the content of a question based on its number
    // /question/100 -> Regular expression matching
    svr.Get(R"(/question/(\d+))", [&ctrl](const Request &req, Response &resp)
            {
        std::string number = req.matches[1];
        std::string html;
        ctrl.Question(number, &html);
        resp.set_content(html, "text/html; charset=utf-8"); });
    // User submissions
    svr.Post(R"(/judge/(\d+))", [&ctrl](const Request &req, Response &resp)
             {
        std::string number = req.matches[1];
        std::string res_json;
        ctrl.Judge(number, req.body, &res_json);
        resp.set_content(res_json, "application/json; charset=utf-8"); });
    // Set the homepage
    svr.set_base_dir("./wwwroot");

    svr.listen("0.0.0.0", 8080);
    return 0;
}

#if false
int main()
{
    // Route functions for user requests
    Server svr;
    // Retrieve the list of all questions
    svr.Get("/all_questions", [](const Request &req, Response &resp) {
        resp.set_content("This is the list of all questions", "text/plain; charset=utf-8");
    });
    // Users want to get the content of a question based on its number
    // /question/100 -> Regular expression matching
    svr.Get(R"(/question/(\d+))", [](const Request &req, Response &resp){
        std::string number = req.matches[1]; // Here you can get what the regular expression captured
        // Interested can study further
        resp.set_content("This is a specific question: " + number, "text/plain; charset=utf-8");
    });
    // User submissions
    svr.Get(R"(/judge/(\d+))", [](const Request &req, Response &resp){
        std::string number = req.matches[1];
        resp.set_content("This is the judging for the specific question: " + number, "text/plain; charset=utf-8");
    });
    // Set the homepage
    svr.set_base_dir("./wwwroot");

    svr.listen("0.0.0.0", 8080);
    return 0;
}
#endif