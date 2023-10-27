
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    // 通过定义临时对象，来完成方法的调用
    int ret = Solution().MyMax({1,2,3,4,5,6,7,8,9,10});
    if (ret == 10)
    {
        std::cout << "[exp1][input: {1,2,3,4,5,6,7,8,9,10}] 通过" << std::endl;
    }
    else
    {
        std::cout << "[exp1][input: {1,2,3,4,5,6,7,8,9,10}] 答案错误" << std::endl;
    }
}
void Test2()
{
    // 通过定义临时对象，来完成方法的调用
    int ret = Solution().MyMax({-100, 0, 1, -3, 2});
    if (ret == 2)
    {
        std::cout << "[exp2][input: {-100, 0, 1, -3, 2}] 通过" << std::endl;

    }
    else
    {
        std::cout << "[exp2][input: {-100, 0, 1, -3, 2}] 答案错误" << std::endl;
    }
}
void Test3()
{
    // 通过定义临时对象，来完成方法的调用
    int ret = Solution().MyMax({0,0,0,0,0});
    if (ret == 0)
    {
        std::cout << "[exp3][input: {0,0,0,0,0}] 通过" << std::endl;

    }
    else
    {
        std::cout << "[exp3][input: {0,0,0,0,0}] 答案错误" << std::endl;
    }
}
int main()
{
    Test1();
    Test2();
    Test3();
    return 0;
}