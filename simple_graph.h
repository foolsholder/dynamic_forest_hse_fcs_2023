#ifndef DYNAMIC_FOREST_SIMPLE_GRAPH_H
#define DYNAMIC_FOREST_SIMPLE_GRAPH_H

#include <vector>
#include <unordered_set>

class SimpleGraph{
public:
    SimpleGraph(int size) : size_{size}, graph_(size), visited_(size) {
        connect_.resize(size, std::vector<int>(size, -1));
    }

    void AddEdge(int v_num, int u_num) {
        graph_[v_num].insert(u_num);
        graph_[u_num].insert(v_num);
    }

    void RemoveEdge(int v_num, int u_num) {
        graph_[v_num].erase(u_num);
        graph_[u_num].erase(v_num);
    }

    void Dfs(size_t ver, int val) {
        visited_[ver] = val;

        for (auto u : graph_[ver]) {
            if (visited_[u] == -1) {
                Dfs(u, val + 1);
            }
        }
    }

    void CalculateConnectMatrix() {
        for (auto& row : connect_) {
            std::fill(row.begin(), row.end(), -1);
        }
        for (size_t ver = 0; ver < size_; ++ver) {
            std::fill(visited_.begin(), visited_.end(), -1);
            Dfs(ver, 0);
            std::copy(visited_.begin(), visited_.end(), connect_[ver].begin());
        }
    }

    bool IsConnected(int u_num, int v_num) const {
        return connect_[u_num][v_num] != -1;
    }

    std::vector<std::pair<int, int>> ToPairs(bool connect) const {
        std::vector<std::pair<int, int>> res{};
        if (!connect) {
            for (int ver = 0; ver < size_; ++ver) {
                for (int next_ver = ver + 1; next_ver < size_; ++next_ver) {
                    if (connect_[ver][next_ver] == -1) {
                        res.emplace_back(ver, next_ver);
                    }
                }
            }
        } else {
            for (int ver = 0; ver < size_; ++ver) {
                for (auto next_ver : graph_[ver]) {
                    res.emplace_back(ver, next_ver);
                }
            }
        }
        return res;
    }

private:
    int size_{};
    std::vector<std::unordered_set<int>> graph_{};
    std::vector<int> visited_{};
    std::vector<std::vector<int>> connect_{};
};

#endif //DYNAMIC_FOREST_SIMPLE_GRAPH_H
