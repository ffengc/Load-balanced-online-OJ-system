
/* export LD\_LIBRARY\_PATH=$LD\_LIBRARY\_PATH:/usr/local/lib */

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
    // 用户请求的路由功能
    Server svr;
    Control ctrl;
    g_ctrl_ptr = &ctrl;
    // 获取所有题目的列表
    svr.Get("/all_questions", [&ctrl](const Request &req, Response &resp)
            {
        // 这里我想返回一张包含所以题目列表信息的html网页
        std::string html;
        ctrl.AllQuestions(&html);

        resp.set_content(html, "text/html; charset=utf-8"); });
    // 用户要根据题目编号获取题目的内容
    // /question/100  ->  正则匹配
    svr.Get(R"(/question/(\d+))", [&ctrl](const Request &req, Response &resp)
            {
        std::string number = req.matches[1];
        std::string html;
        ctrl.Question(number, &html);
        resp.set_content(html, "text/html; charset=utf-8"); });
    // 用户提交
    svr.Post(R"(/judge/(\d+))", [&ctrl](const Request &req, Response &resp)
             {
        std::string number = req.matches[1];
        std::string res_json;
        ctrl.Judge(number, req.body, &res_json);
        resp.set_content(res_json, "application/json; charset=utf-8"); });
    // 设置一下首页
    svr.set_base_dir("./wwwroot");

    svr.listen("0.0.0.0", 8080);
    return 0;
}

#if false
int main()
{
    // 用户请求的路由功能
    Server svr;
    // 获取所有题目的列表
    svr.Get("/all_questions", [](const Request &req, Response &resp) {
        resp.set_content("这是所有题目的列表", "text/plain; charset=utf-8");
    });
    // 用户要根据题目编号获取题目的内容
    // /question/100  ->  正则匹配
    svr.Get(R"(/question/(\d+))", [](const Request &req, Response &resp){
        std::string number = req.matches[1]; // 这里可以得到正则表达式获取到的东西
        // 感兴趣可以研究一下
        resp.set_content("这是指定的一道题: " + number, "text/plain; charset=utf-8");
    });
    // 用户提交
    svr.Get(R"(/judge/(\d+))", [](const Request &req, Response &resp){
        std::string number = req.matches[1];
        resp.set_content("这是指定题目的判题: " + number, "text/plain; charset=utf-8");
    });
    // 设置一下首页
    svr.set_base_dir("./wwwroot");

    svr.listen("0.0.0.0", 8080);
    return 0;
}
#endif