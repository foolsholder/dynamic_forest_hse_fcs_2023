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

    void AddEdge(TreapVertex<Edge>* u_vertex, TreapVertex<Edge>* v_vertex,
                 TreapVertex<Edge>* edge_forward, TreapVertex<Edge>* edge_backward) {
        auto u_root = treap::GetTreapRoot(u_vertex);
        auto v_root = treap::GetTreapRoot(v_vertex);

        v_root = treap::MoveToFirstPos(v_vertex, v_root);
        auto augmented_root = treap::MergeTreap(v_root, edge_backward);
        augmented_root = treap::MergeTreap(edge_forward, augmented_root);


        u_root = treap::MoveToFirstPos(u_vertex, u_root);
        treap::MergeTreap(u_root, augmented_root);
    }

    void AddEdge(int u_num, int v_num) {
        auto u_vertex = GetVirtualVertex(u_num);
        auto v_vertex = GetVirtualVertex(v_num);


        Edge fwd = {u_num, v_num};
        Edge bwd = {v_num, u_num};
        uint64_t encode_forward = EncodeEdge(fwd);
        uint64_t encode_backward = EncodeEdge(bwd);

        graph_[u_num].push_front(v_num);
        graph_[v_num].push_front(u_num);

        list_iter_edges_.insert_or_assign(encode_forward, graph_[u_num].begin());
        list_iter_edges_.insert_or_assign(encode_backward, graph_[v_num].begin());

        auto [iter_f, suc_f] = edges_.insert_or_assign(encode_forward, CreateTreapVertex(fwd));
        auto [iter_b, suc_b] = edges_.insert_or_assign(encode_backward, CreateTreapVertex(bwd));

        AddEdge(u_vertex, v_vertex, &iter_f->second, &iter_b->second);
    }

    void RemoveEdge(TreapVertex<Edge>* edge_one, TreapVertex<Edge>* edge_two) {
        auto virt_root = treap::GetTreapRoot(edge_one);

        size_t one_pos = treap::PosNumberInTreap(edge_one);
        size_t two_pos = treap::PosNumberInTreap(edge_two);

        if (one_pos > two_pos) {
            std::swap(one_pos, two_pos);
            std::swap(edge_one, edge_two);
        }

        auto [before_one, with_one] = treap::SplitTreap(virt_root, one_pos);
        auto [one, after_one] = treap::SplitTreap(with_one, 1);

        two_pos = treap::PosNumberInTreap(edge_two);
        auto [before_two, with_two] = treap::SplitTreap(after_one, two_pos);
        auto [two, after_two] = treap::SplitTreap(with_two, 1);

        treap::MergeTreap(before_one, after_two);
    }

    void RemoveEdge(int u_num, int v_num) {
        uint64_t encode_one = EncodeEdge({u_num, v_num});
        uint64_t encode_two = EncodeEdge({v_num, u_num});

        auto iter_one = edges_.find(encode_one);
        auto iter_two = edges_.find(encode_two);

        RemoveEdge(&iter_one->second, &iter_two->second);

        edges_.erase(iter_one);
        edges_.erase(iter_two);

        auto map_one_iter = list_iter_edges_.find(encode_one);
        auto map_two_iter = list_iter_edges_.find(encode_two);

        graph_[u_num].erase(map_one_iter->second);
        graph_[v_num].erase(map_two_iter->second);
        list_iter_edges_.erase(map_one_iter);
        list_iter_edges_.erase(map_two_iter);
    }

    TreapVertex<Edge>* GetVirtualVertex(int v_num) const {
        if (graph_[v_num].empty()) {
            return nullptr;
        }
        auto ptr =  &edges_.find(EncodeEdge({v_num, graph_[v_num].front()}))->second;
        return const_cast<TreapVertex<Edge>*>(ptr);  // NOLINT
    }

    bool IsConnected(int u_num, int v_num) const {
        if (u_num == v_num) {
            return true;
        }
        auto u_vertex = GetVirtualVertex(u_num);
        auto v_vertex = GetVirtualVertex(v_num);

        if (!u_vertex || !v_vertex) {
            return false;
        }

        auto u_root = treap::GetTreapRoot(u_vertex);
        auto v_root = treap::GetTreapRoot(v_vertex);
        return u_root == v_root;
    }
    /*
    void PrintVirtualTree(int v_num) {
        auto root = treap::GetTreapRoot(GetVirtualVertex(v_num));
        treap::PrintTreap(root);
        std::cout << '\n';
    }
    */

private:
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
