
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    int ret = Solution().MyMax({1,2,3,4,5,6,7,8,9,10});
    if (ret == 10)
    {
        std::cout << "[exp1][input: {1,2,3,4,5,6,7,8,9,10}] pass" << std::endl;
    }
    else
    {
        std::cout << "[exp1][input: {1,2,3,4,5,6,7,8,9,10}] wrong" << std::endl;
    }
}
void Test2()
{
    int ret = Solution().MyMax({-100, 0, 1, -3, 2});
    if (ret == 2)
    {
        std::cout << "[exp2][input: {-100, 0, 1, -3, 2}] pass" << std::endl;

    }
    else
    {
        std::cout << "[exp2][input: {-100, 0, 1, -3, 2}] wrong" << std::endl;
    }
}
void Test3()
{
    int ret = Solution().MyMax({0,0,0,0,0});
    if (ret == 0)
    {
        std::cout << "[exp3][input: {0,0,0,0,0}] pass" << std::endl;

    }
    else
    {
        std::cout << "[exp3][input: {0,0,0,0,0}] wrong" << std::endl;
    }
}
int main()
{
    Test1();
    Test2();
    Test3();
    return 0;
}