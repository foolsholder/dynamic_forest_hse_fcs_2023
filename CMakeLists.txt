cmake_minimum_required(VERSION 3.23)
project(dynamic_forest)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS_ASAN "-g -fsanitize=address,undefined -fno-sanitize-recover=all"
        CACHE STRING "Compiler flags in asan build"
        FORCE)

add_executable(dynamic_forest main.cpp euler_tour_tree.h simple_graph.h test.h treap.h test_treap.h)

