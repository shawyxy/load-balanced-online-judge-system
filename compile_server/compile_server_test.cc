#include "compile_run.hpp"
using namespace ns_compile_run;

int main()
{
    std::string in_json;
    Json::Value in_value;
    in_value["code"] = R"(
    #include<iostream>
    using namespace std;
    int main(){
        int a;
        cin >> a;
        cout << a << endl;
        return 0;
    })";
    in_value["input"] = "1";
    in_value["cpu_limit"] = 1;
    in_value["mem_limit"] = 102400;

    // FastWriter构建原始字符串（无JSON格式），用于网络传输
    Json::FastWriter writer;
    in_json = writer.write(in_value);
    std::cout << "原始字符串：\n" << in_json << std::endl;
    
    // StyledWriter构建JSON格式字符串
    std::string out_json;
    // 编译并运行
    CompileAndRun::Start(in_json, &out_json);
    std::cout << "JSON字符串：\n" << out_json << std::endl;

    return 0;
}