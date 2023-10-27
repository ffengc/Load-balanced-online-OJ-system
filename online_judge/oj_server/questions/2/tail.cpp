
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    // 通过定义临时对象，来完成方法的调用
    bool ret = Solution().isPalindrome(121);
    if (ret)
    {
        std::cout << "[exp1][input: 121] 通过" << std::endl;
    }
    else
    {
        std::cout << "[exp1][input: 121] 答案错误" << std::endl;
    }
}
void Test2()
{
    // 通过定义临时对象，来完成方法的调用
    bool ret = Solution().isPalindrome(-10);
    if (!ret)
    {
        std::cout << "[exp2][input: -10] 通过" << std::endl;

    }
    else
    {
        std::cout << "[exp2][input: -10] 答案错误" << std::endl;
    }
}
int main()
{
    Test1();
    Test2();
    return 0;
}