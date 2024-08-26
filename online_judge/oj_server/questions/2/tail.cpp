
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1()
{
    bool ret = Solution().isPalindrome(121);
    if (ret)
    {
        std::cout << "[exp1][input: 121] pass" << std::endl;
    }
    else
    {
        std::cout << "[exp1][input: 121] wrong" << std::endl;
    }
}
void Test2()
{
    bool ret = Solution().isPalindrome(-10);
    if (!ret)
    {
        std::cout << "[exp2][input: -10] pass" << std::endl;

    }
    else
    {
        std::cout << "[exp2][input: -10] wrong" << std::endl;
    }
}
int main()
{
    Test1();
    Test2();
    return 0;
}