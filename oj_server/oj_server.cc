#include <iostream>

#include "../comm/httplib.h"
#include "oj_control.hpp"

using namespace httplib;
using namespace ns_control;

int main()
{
    // 用户请求的路由功能
    Server svr;
    // 前端和后端交互功能
    Control ctrl;

    // 获取题目列表
    svr.Get("/all_problems", [&ctrl](const Request &req, Response &resp) {
        std::string html;
        ctrl.AllProblems(&html);
        resp.set_content(html, "text/html; charset=utf-8");
    });
    
    // 根据用户提交的题目编号返回题目内容给用户
    // /problems/${题目编号}$
    svr.Get(R"(/problems/(\d+))", [&ctrl](const Request &req, Response &resp) {
        std::string id = req.matches[1]; // 正则表达式匹配题目编号
        std::string html;
        ctrl.Problem(id, &html);
        resp.set_content(html, "text/html; charset=utf-8");
    });
    
    // 用户提交判题请求
    svr.Post(R"(/judge/(\d+))", [&ctrl](const Request &req, Response &resp) {
        std::string id = req.matches[1];
        std::string result_json;
        ctrl.Judge(id, req.body, &result_json);
        resp.set_content(result_json, "application/json;charset=utf-8");
    });

    // 将 wwwroot 目录设为服务器的基础目录
    svr.set_base_dir("./wwwroot");
    
    svr.listen("0.0.0.0", 8080);

    return 0;
}