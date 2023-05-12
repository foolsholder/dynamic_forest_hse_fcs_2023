#ifndef DYNAMIC_FOREST_TEST_H
#define DYNAMIC_FOREST_TEST_H

#include <iostream>
#include <list>
#include "simple_graph.h"
#include "euler_tour_tree.h"

void TestAddEdge() {
    {
        int size = 6;
        DynamicForest forest{size};

        forest.AddEdge(0, 1);

        assert(forest.IsConnected(0, 1) == 1);
        assert(forest.IsConnected(1, 3) == 0);

        forest.AddEdge(2, 3);

        assert(forest.IsConnected(2, 3) == 1);
        assert(forest.IsConnected(1, 3) == 0);

        forest.AddEdge(1, 2);

        assert(forest.IsConnected(1, 2) == 1);
        assert(forest.IsConnected(0, 2) == 1);
        assert(forest.IsConnected(1, 3) == 1);
        assert(forest.IsConnected(0, 3) == 1);
        assert(forest.IsConnected(2, 5) == 0);

        forest.AddEdge(2, 4);
        assert(forest.IsConnected(1, 4) == 1);
        assert(forest.IsConnected(5, 4) == 0);
        assert(forest.IsConnected(0, 4) == 1);
        assert(forest.IsConnected(3, 4) == 1);
    }
    {
        uint32_t random_seed = 1337;
        int size = 500;
        int queries_cnt = 500;
        int count_checks = 500;

        DynamicForest forest{size};
        SimpleGraph graph{size};

        std::mt19937 rng{random_seed};

        for (int iter_num = 0; iter_num < queries_cnt; ++iter_num) {
            size_t u, v;

            auto pairs = graph.ToPairs(false);
            if (pairs.empty()) {
                continue;
            }
            size_t idx = rng() % pairs.size();
            auto [fst, snd] = pairs[idx];
            u = fst;
            v = snd;

            graph.AddEdge(u, v);
            graph.CalculateConnectMatrix();
            forest.AddEdge(u, v);

            for (int check_iter = 0; check_iter < count_checks; ++check_iter) {
                u = rng() % size;
                v = rng() % size;

                if (graph.IsConnected(u, v) != forest.IsConnected(u, v)) {
                    //forest.PrintVirtualTree(u);
                    throw;
                }
            }
        }
    }

    std::cout << "ADD_EDGE_TEST: SUCCESS" << std::endl;
}


void TestSimple() {
    int size = 5;
    DynamicForest forest{size};

    forest.AddEdge(0, 1);

    assert(forest.IsConnected(0, 1) == 1);
    assert(forest.IsConnected(1, 3) == 0);

    forest.AddEdge(2, 3);

    assert(forest.IsConnected(2, 3) == 1);
    assert(forest.IsConnected(1, 3) == 0);

    forest.AddEdge(1, 2);

    assert(forest.IsConnected(1, 2) == 1);
    assert(forest.IsConnected(0, 2) == 1);
    assert(forest.IsConnected(1, 3) == 1);
    assert(forest.IsConnected(0, 3) == 1);

    forest.RemoveEdge(2, 3);
    assert(forest.IsConnected(1, 3) == 0);
    assert(forest.IsConnected(2, 3) == 0);


    forest.AddEdge(1, 3);
    assert(forest.IsConnected(1, 3) == 1);
    assert(forest.IsConnected(2, 3) == 1);


    forest.RemoveEdge(0, 1);
    assert(forest.IsConnected(3, 0) == 0);
    assert(forest.IsConnected(2, 0) == 0);
    std::cout << "SIMPLE_TEST: SUCCESS" << std::endl;
}

DynamicForest TestRandom(const uint32_t random_seed = 998, int size = 500, int queries_cnt = 999, int count_checks=100) {
    DynamicForest forest{size};
    SimpleGraph graph{size};

    std::mt19937 rng{random_seed};

    for (int iter_num = 0; iter_num < queries_cnt; ++iter_num) {
        size_t u, v;

        if (4 * iter_num + 1 < size * 3 || rng() % 2 == 1) {
            auto pairs = graph.ToPairs(false);
            if (pairs.empty()) {
                continue;
            }
            size_t idx = rng() % pairs.size();
            auto [fst, snd] = pairs[idx];
            u = fst;
            v = snd;

            graph.AddEdge(u, v);
            graph.CalculateConnectMatrix();
            forest.AddEdge(u, v);
            //std::cout << "Add " << u << ", " << v << '\n';
            //forest.PrintVirtualTree(u);
        } else {
            auto pairs = graph.ToPairs(true);
            if (pairs.empty()) {
                continue;
            }
            size_t idx = rng() % pairs.size();
            auto [fst, snd] = pairs[idx];
            u = fst;
            v = snd;

            graph.RemoveEdge(u, v);
            graph.CalculateConnectMatrix();
            forest.RemoveEdge(u, v);
            //std::cout << "Remove " << u << ", " << v << '\n';
            //forest.PrintVirtualTree(u);
            //forest.PrintVirtualTree(v);
        }

        for (int check_iter = 0; check_iter < count_checks; ++check_iter) {
            u = rng() % size;
            v = rng() % size;

            if (graph.IsConnected(u, v) != forest.IsConnected(u, v)) {
                //forest.PrintVirtualTree(u);
                throw;
            }
        }
    }
    return forest;
}

void TestTiny(const uint32_t random_seed = 998) {
    auto forest = TestRandom(random_seed, 25, 30, 100);
    //forest.PrintVirtualTree(13);
    std::cout << std::endl;
}

void TestSmall(const uint32_t random_seed = 998) {
    TestRandom(random_seed, 100, 1000, 1000);
    std::cout << "TEST SMALL: SUCCESS" << std::endl;
}

void TestMedium(const uint32_t random_seed = 998) {
    TestRandom(random_seed);
    std::cout << "TEST MEDIUM: SUCCESS" << std::endl;
}

void TestLarge(const uint32_t random_seed = 998) {
    int size = 100'000;

    DynamicForest forest{size};

    std::mt19937 rng{random_seed};

    std::list<std::pair<size_t, size_t>> edges;
    for (int v = 1; v < size; ++v) {
        size_t anc = rng() % v;
        forest.AddEdge(anc, v);
        edges.push_back({anc, v});
    }

    int cnt = 100'000;
    for (int it = 0; it < cnt; ++it) {
        auto [v, u] = *edges.begin();
        edges.pop_front();
        forest.RemoveEdge(u, v);
        assert(forest.IsConnected(v, u) == 0);
        size_t old_u = u, old_v = v;
        int tries = 25;
        do {
            u = rng() % size;
            v = rng() % size;
            --tries;
        } while (tries && (u == v || forest.IsConnected(v, u)));
        if (!forest.IsConnected(v, u)) {
            assert(forest.GetComponentsNumber() == 2);
            forest.AddEdge(u, v);
            edges.push_back({u, v});
            assert(forest.IsConnected(0, u));
            assert(forest.IsConnected(size - 1, v));
            assert(forest.IsConnected(1337, 29'589));
        } else {
            forest.AddEdge(old_u, old_v);
            edges.push_back({old_v, old_u});
        }
        assert(forest.GetComponentsNumber() == 1);
    }

    std::cout << "TEST LARGE: SUCCESS" << std::endl;
}

#endif //DYNAMIC_FOREST_TEST_H

