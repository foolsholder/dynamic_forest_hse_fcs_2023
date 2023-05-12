#include <iostream>
#include "test.h"
#include "test_treap.h"


int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::cout.tie(nullptr);

    TestImplicit();
    TestAddEdge();
    TestSimple();
    TestSmall();
    TestMedium();
    TestLarge();

    return 0;
}
