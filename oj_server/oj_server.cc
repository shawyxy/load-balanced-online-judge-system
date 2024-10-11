#include <iostream>

#include "../comm/httplib.h"

using namespace httplib;

int main()
{
    // 用户请求的路由功能
    Server svr;

    // 获取题目列表
    svr.Get("/problems_list", [](const Request &req, Response &resp) {
        resp.set_content("题目列表", "text/plain; charset=utf-8");
    });
    
    // 根据用户提交的题目编号返回题目内容给用户
    // /problems/${题目编号}$
    svr.Get(R"(/problems/(\d+))", [](const Request &req, Response &resp) {
        std::string number = req.matches[1]; // 正则表达式匹配题目编号
        resp.set_content("题目编号：" + number, "text/plain; charset=utf-8");
    });

    // 用户提交判题请求
    svr.Get(R"(/judge/(\d+))", [](const Request &req, Response &resp) {
        std::string number = req.matches[1];
        resp.set_content("判题编号：" + number, "text/plain; charset=utf-8");
    });

    svr.listen("0.0.0.0", 8080);

    return 0;
}