#include "compile_run.hpp"
#include "../comm/httplib.h"

// 引入编译并运行模块
using namespace ns_compile_run;
// 引入网络服务模块
using namespace httplib;

int main()
{
    // 1. 创建server对象
    Server svr;
    // 2. 注册响应内容
    //    当用户请求"/hello"时，执行对应的响应（lambda函数）
    svr.Get("/hello", [](const Request &req, Response &resp)
            { resp.set_content("hello httplib, 你好", "text/plain;charset=utf-8;"); });
    // 3. 启动http服务
    svr.listen("0.0.0.0", 8080);
    return 0;
}