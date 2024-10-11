#include "compile_run.hpp"
#include "../comm/httplib.h"

// 引入编译并运行模块
using namespace ns_compile_run;
// 引入网络服务模块
using namespace httplib;

void Usage(const std::string proc)
{
    std::cerr << "Usage: " << "\n\t" << proc << " port" << std::endl;
}

// 运行：./compile_server [端口号]
int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        return 1;
    }
    // 1. 创建Server对象
    Server svr;

    // 2. 注册响应处理
    //    当用户请求"/compile_and_run"时，执行对应的响应（编译并运行）
    svr.Post("/compile_and_run", [](const Request &req, Response &resp) {
        // 获取用户提交的代码+测试用例+CPU和内存限制
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
    svr.listen("0.0.0.0", atoi(argv[1]));
    return 0;
}