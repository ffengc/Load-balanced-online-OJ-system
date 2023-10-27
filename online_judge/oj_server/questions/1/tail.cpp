
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    // 通过定义临时对象，来完成方法的调用
    int ret = Solution().MyStrlen("hello world");
    if (ret == 11)
    {
        std::cout << "[exp1][input:\"hello world\"] 通过" << std::endl;
    }
    else
    {
        std::cout << "[exp1][input:\"hello world\"] 答案错误" << std::endl;
    }
}
void Test2()
{
    // 通过定义临时对象，来完成方法的调用
    int ret = Solution().MyStrlen("aaaabbbbcccc");
    if (ret == 12)
    {
        std::cout << "[exp2][input:\"aaaabbbbcccc\"] 通过" << std::endl;
    }
    else
    {
        std::cout << "[exp2][input:\"aaaabbbbcccc\"] 答案错误" << std::endl;
    }
}
void Test3()
{
    // 通过定义临时对象，来完成方法的调用
    int ret = Solution().MyStrlen("");
    if (ret == 0)
    {
        std::cout << "[exp3][input:\"\"] 通过" << std::endl;
    }
    else
    {
        std::cout << "[exp3][input:\"\"] 答案错误" << std::endl;
    }
}
int main()
{
    Test1();
    Test2();
    Test3();
    return 0;
}