/*
 * Write by Yufc
 * See https://github.com/ffengc/Load-balanced-online-OJ-system
 * please cite my project link: https://github.com/ffengc/Load-balanced-online-OJ-system when you use this code
 */

#include "compile_run.hpp"
#include "../comm/httplib.h"

using namespace ns_compile_run;
using namespace httplib;

void Usage(std::string proc)
{
    std::cerr << "Usage: "
              << "\n\t" << proc << " port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        return 1;
    }
    Server svr;
#if false
    svr.Get("/hello", [](const Request &req, Response &resp)
            { resp.set_content("hello httplib, hello", "text/plain;charset=utf-8;"); });
#endif
    svr.Post("/compile_and_run", [](const Request &req, Response &resp)
             {
        // The body of the user's request is the json string we want
        std::string in_json = req.body;
        std::string out_json;
        if (!in_json.empty())
        {
            compile_run::Start(in_json, &out_json);
            resp.set_content(out_json, "application/json;charset=utf-8");
        } });

    svr.listen("0.0.0.0", atoi(argv[1])); // Start the http service

    return 0;
}