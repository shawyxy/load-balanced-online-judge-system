// #include "log.hpp"
// using namespace ns_log;
// int main()
// {
//     LOG(INFO) << "自定义内容\n";
//     return 0;
// }

// 这个头文件是 JsonCpp 库的一部分，用来处理 JSON 数据
#include <jsoncpp/json/json.h>
#include <iostream>
#include <string>

void func(std::string *out) {

}

int main()
{
    // Json::Value 是 JsonCpp 提供的一个类，表示一个 JSON 值
    Json::Value root;
    // 这样的操作会自动为 JSON 对象添加新的键值对。
    root["code"] = "#include <iostream> int main(){}";
    root["input"] = "1 2 3 4";
    root["author"] = "xy";
    root["time"] = "2024/10";
    // 用于将 JSON 对象转换为格式化的 JSON 字符串。
    Json::StyledWriter writer;
    // 将 root 对象转换为 JSON 字符串
    std::string str = writer.write(root);
    std::cout << str << std::endl;
    return 0;
}