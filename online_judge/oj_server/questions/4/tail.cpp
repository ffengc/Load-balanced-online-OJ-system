
#ifndef COMPILER_ONLINE
#include "header.cpp"
#endif

void Test1() {
    int ret = Solution().totalNQueens(1);
    if (ret == 1)
        std::cout << "[exp1][input: 1] pass" << std::endl;
    else
        std::cout << "[exp1][input: 1] wrong" << std::endl;
}

void Test2() {
    int ret = Solution().totalNQueens(2);
    if (ret == 0)
        std::cout << "[exp2][input: 2] pass" << std::endl;
    else
        std::cout << "[exp2][input: 2] wrong" << std::endl;
}

void Test3() {
    int ret = Solution().totalNQueens(3);
    if (ret == 0)
        std::cout << "[exp3][input: 3] pass" << std::endl;
    else
        std::cout << "[exp3][input: 3] wrong" << std::endl;
}

void Test4() {
    int ret = Solution().totalNQueens(4);
    if (ret == 2)
        std::cout << "[exp4][input: 4] pass" << std::endl;
    else
        std::cout << "[exp4][input: 4] wrong" << std::endl;
}

void Test5() {
    int ret = Solution().totalNQueens(5);
    if (ret == 10)
        std::cout << "[exp5][input: 5] pass" << std::endl;
    else
        std::cout << "[exp5][input: 5] wrong" << std::endl;
}

void Test6() {
    int ret = Solution().totalNQueens(6);
    if (ret == 4)
        std::cout << "[exp6][input: 6] pass" << std::endl;
    else
        std::cout << "[exp6][input: 6] wrong" << std::endl;
}

void Test7() {
    int ret = Solution().totalNQueens(7);
    if (ret == 40)
        std::cout << "[exp7][input: 7] pass" << std::endl;
    else
        std::cout << "[exp7][input: 7] wrong" << std::endl;
}

void Test8() {
    int ret = Solution().totalNQueens(8);
    if (ret == 92)
        std::cout << "[exp8][input: 8] pass" << std::endl;
    else
        std::cout << "[exp8][input: 8] wrong" << std::endl;
}

void Test9() {
    int ret = Solution().totalNQueens(9);
    if (ret == 352)
        std::cout << "[exp9][input: 9] pass" << std::endl;
    else
        std::cout << "[exp9][input: 9] wrong" << std::endl;
}

int main() {
    Test1();
    Test2();
    Test3();
    Test4();
    Test5();
    Test6();
    Test7();
    Test8();
    Test9();

    return 0;
}