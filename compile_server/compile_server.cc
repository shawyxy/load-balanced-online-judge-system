#include "compile_run.hpp"
#include "../comm/httplib.h"

// 引入编译并运行模块
using namespace ns_compile_run;
// 引入网络服务模块
using namespace httplib;

int main()
{
    // 1. 创建Server对象
    Server svr;

    // 2. 注册响应处理
    //    当用户请求"/compile_and_run"时，执行对应的响应（编译并运行）
    svr.Post("/compile_and_run", [](const Request &req, Response &resp) {
        // 获取用户提交的代码
        std::string in_json = req.body; 
        // 输出型参数：代码编译运行后的结果（状态码，描述，[标准输出，标准错误]）
        std::string out_json; 
        if (!in_json.empty()) { // 请求体不为空
            // 调用编译并运行代码的函数
            CompileAndRun::Start(in_json, &out_json);
            // 返回编译运行结果
            resp.set_content(out_json, "application/json;charset=utf-8");
        }});

    // 3. 启动http服务
    svr.listen("0.0.0.0", 8080);
    return 0;
}