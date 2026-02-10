
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    int ret = Solution().MyStrlen("hello world");
    if (ret == 11)
    {
        std::cout << "[exp1][input:\"hello world\"] pass" << std::endl;
    }
    else
    {
        std::cout << "[exp1][input:\"hello world\"] wrong" << std::endl;
    }
}
void Test2()
{
    int ret = Solution().MyStrlen("aaaabbbbcccc");
    if (ret == 12)
    {
        std::cout << "[exp2][input:\"aaaabbbbcccc\"] pass" << std::endl;
    }
    else
    {
        std::cout << "[exp2][input:\"aaaabbbbcccc\"] wrong" << std::endl;
    }
}
void Test3()
{
    int ret = Solution().MyStrlen("");
    if (ret == 0)
    {
        std::cout << "[exp3][input:\"\"] pass" << std::endl;
    }
    else
    {
        std::cout << "[exp3][input:\"\"] wrong" << std::endl;
    }
}
int main()
{
    Test1();
    Test2();
    Test3();
    return 0;
}