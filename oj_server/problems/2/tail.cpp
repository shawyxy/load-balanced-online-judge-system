// 如果编译选项没有定义宏，那么引入代码接口
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{

}

void Test2()
{

}

int main()
{
    Test1();
    Test2();

    return 0;
}