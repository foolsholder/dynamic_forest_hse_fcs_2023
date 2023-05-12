#ifndef DYNAMIC_FOREST_EULER_TOUR_TREE_H
#define DYNAMIC_FOREST_EULER_TOUR_TREE_H

#include <memory>
#include <vector>
#include <random>
#include <iostream>
#include <unordered_map>
#include <algorithm>
#include <list>

#include "treap.h"

struct Edge {
    int from;
    int to;

    operator std::string() const {
        return "(" + std::to_string(from) + "," + std::to_string(to) + ")";
    }
};


class DynamicForest {
public:
    DynamicForest(int vertex_count, const uint32_t seed = 1337)
        : size_{vertex_count}, graph_(size_), rng_{seed} {
        graph_.assign(size_, {});
    }

    int GetComponentsNumber() const {
        return size_ - static_cast<int>(edges_.size() / 2);
    }

    void AddEdge(int u_num, int v_num) {
        auto encode_forward = EncodeEdge({u_num, v_num});
        auto encode_backward = EncodeEdge({v_num, u_num});

        edges_[encode_forward] = CreateTreapVertex({u_num, v_num});
        edges_[encode_backward] = CreateTreapVertex({v_num, u_num});

        AddEdge(
            GetVirtualVertex(u_num),
            GetVirtualVertex(v_num),
            &edges_[encode_forward],
            &edges_[encode_backward]
        );

        graph_[u_num].push_front(v_num);
        graph_[v_num].push_front(u_num);
        list_iter_edges_[encode_forward] = graph_[u_num].begin();
        list_iter_edges_[encode_backward] = graph_[v_num].begin();
    }

    void RemoveEdge(int u_num, int v_num) {
        auto encode_forward = EncodeEdge({u_num, v_num});
        auto encode_backward = EncodeEdge({v_num, u_num});

        TreapVertex<Edge>* edge_f = &edges_[encode_forward];
        TreapVertex<Edge>* edge_b = &edges_[encode_backward];

        RemoveEdge(edge_f, edge_b);

        auto iter_f = list_iter_edges_[encode_forward];
        auto iter_b = list_iter_edges_[encode_backward];
        graph_[u_num].erase(iter_f);
        graph_[v_num].erase(iter_b);

        edges_.erase(encode_forward);
        edges_.erase(encode_backward);

        list_iter_edges_.erase(encode_forward);
        list_iter_edges_.erase(encode_backward);
    }

    bool IsConnected(int u_num, int v_num) {
        if (u_num == v_num) {
            return true;
        }
        auto u_vertex = GetVirtualVertex(u_num);
        auto v_vertex = GetVirtualVertex(v_num);
        if (!u_vertex || !v_vertex) {
            return false;
        }
        return treap::GetTreapRoot(v_vertex) == treap::GetTreapRoot(u_vertex);
    }
    /*
    void PrintVirtualTree(int v_num) {
        auto root = treap::GetTreapRoot(GetVirtualVertex(v_num));
        treap::PrintTreap(root);
        std::cout << '\n';
    }
    */

private:
    void AddEdge(TreapVertex<Edge>* u_vertex, TreapVertex<Edge>* v_vertex,
                 TreapVertex<Edge>* edge_forward, TreapVertex<Edge>* edge_backward) {
        auto u_subtree = treap::MoveToFirstPos(u_vertex);
        auto v_subtree = treap::MoveToFirstPos(v_vertex);
        auto augmented_v_tree = treap::MergeTreap(edge_forward, v_subtree);
        augmented_v_tree = treap::MergeTreap(augmented_v_tree, edge_backward);
        treap::MergeTreap(u_subtree, augmented_v_tree);
    }

    void RemoveEdge(TreapVertex<Edge>* edge_one, TreapVertex<Edge>* edge_two) {
        auto pos_one = treap::PosNumberInTreap(edge_one);
        auto pos_two = treap::PosNumberInTreap(edge_two);

        auto root = treap::GetTreapRoot(edge_one);
        if (pos_one > pos_two) {
            std::swap(edge_one, edge_two);
            std::swap(pos_one, pos_two);
        }
        auto [mid, right] = treap::SplitTreap(root, pos_two);
        auto [edge_bb, new_right] = treap::SplitTreap(right, 1);
        auto [left, new_subtree] = treap::SplitTreap(mid, pos_one);
        auto [edge_ff, new_subtree_correct] = treap::SplitTreap(new_subtree, 1);

        assert(edge_bb == edge_two);
        assert(edge_one == edge_ff);

        treap::MergeTreap(left, new_right);
    }

    TreapVertex<Edge>* GetVirtualVertex(int v_num) {
        if (graph_[v_num].empty()) {
            return nullptr;
        }
        auto u_num = graph_[v_num].front();
        auto encoding = EncodeEdge({v_num, u_num});
        TreapVertex<Edge>& ref = (edges_[encoding]);
        return &ref;
    }

    TreapVertex<Edge> CreateTreapVertex(Edge edge) {
        TreapVertex<Edge> vertex{edge};
        vertex.treap_priority = rng_();
        return vertex;
    }

    uint64_t EncodeEdge(const Edge& edge) const {
        return edge.from * static_cast<uint64_t>(size_) + edge.to;
    }
    
    int size_{};
    std::vector<std::list<int>> graph_{};
    std::unordered_map<uint64_t, TreapVertex<Edge>> edges_{};
    std::unordered_map<uint64_t, std::list<int>::iterator> list_iter_edges_;
    std::mt19937 rng_{};
};

#endif //DYNAMIC_FOREST_EULER_TOUR_TREE_H
