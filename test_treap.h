#ifndef DYNAMIC_FOREST_TEST_TREAP_H
#define DYNAMIC_FOREST_TEST_TREAP_H

#include "treap.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>

template<typename DataType>
void DFS(TreapVertex<DataType>* root,
         std::vector<DataType>& values,
         std::vector<TreapVertex<DataType>*>& order) {
    if (!root) {
        return;
    }
    DFS(root->left_son, values, order);
    values.push_back(root->data);
    order.push_back(root);
    DFS(root->right_son, values, order);
}

template<typename T>
void CycleShiftLeftVector(std::vector<T>& src, uint32_t shift) {
    std::reverse(src.begin(), src.begin() + shift);
    std::reverse(src.begin() + shift, src.end());
    std::reverse(src.begin(), src.end());
}


void TestImplicit() {
    constexpr int SIZE = 5'000;
    std::vector<int> arr(SIZE);
    for (int i = 0; i < SIZE; ++i) {
        arr[i] = i;
    }

    std::random_device rd;
    std::mt19937 g(rd());
    for (int iter = 0; iter < 500; ++iter) {
        std::shuffle(arr.begin(), arr.end(), g);
        TreapVertex<int>* root = nullptr;
        std::vector<TreapVertex<int>> nodes(SIZE);
        std::vector<TreapVertex<int>*> order;


        std::vector<int> ref;
        ref.reserve(SIZE);


        for (int idx = 0; idx < SIZE; ++idx) {
            nodes[idx].data = arr[idx];
            nodes[idx].treap_priority = (uint32_t)g();

            int rnd_num = g();
            if (rnd_num < 0) {
                rnd_num = -rnd_num;
            }
            int pos = rnd_num % (idx + 1);
            ref.insert(pos + ref.begin(), arr[idx]);

            root = treap::InsertInTreap(root, &nodes[idx], pos);
        }
        std::vector<int> temp;
        temp.reserve(SIZE);
        DFS(root, temp, order);
        assert(temp.size() == (size_t)SIZE);
        for (int idx = 0; idx < SIZE; ++idx) {
            //std::cerr << temp[idx] << ' ';
            assert(temp[idx] == ref[idx]);
            assert(treap::PosNumberInTreap(order[idx]) == idx);
        }

        for (int iter_cycle = 0; iter_cycle < 10; ++iter_cycle) {
            int pos = uint32_t(g()) % SIZE;

            CycleShiftLeftVector(ref, pos);
            CycleShiftLeftVector(order, pos);

            root = treap::MoveToFirstPos(order[0]);

            for (int check_iter = 0; check_iter < 50; ++check_iter) {
                pos = uint32_t(g()) % SIZE;
                assert(treap::PosNumberInTreap(order[pos]) == pos);
            }
        }
    }
    std::cout << "IMPLICIT_TEST: SUCCESS" << std::endl;

}

#endif //DYNAMIC_FOREST_TEST_TREAP_H
