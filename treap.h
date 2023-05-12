#ifndef DYNAMIC_FOREST_TREAP_H
#define DYNAMIC_FOREST_TREAP_H

#include <cinttypes>
#include <utility>


template<typename DataType>
struct TreapVertex {
    DataType data;

    uint32_t size_of_treap{1};
    uint32_t treap_priority{};

    TreapVertex* ancestor{};
    TreapVertex* left_son{};
    TreapVertex* right_son{};

    uint32_t Size() const {
        return size_of_treap;
    }

    uint32_t LeftSize() const {
        if (left_son) {
            return left_son->size_of_treap;
        }
        return 0;
    }

    uint32_t RightSize() const {
        if (right_son) {
            return right_son->size_of_treap;
        }
        return 0;
    }

    void Update() {
        size_of_treap = 1 + LeftSize() + RightSize();
        ancestor = nullptr;  //  update ancestor from ancestor
        if (left_son) {
            left_son->ancestor = this;
        }
        if (right_son) {
            right_son->ancestor = this;
        }
    }
};


namespace treap {
    template<typename DataType>
    uint32_t SubtreeSize(TreapVertex<DataType>* vertex) {
        if (!vertex) {
            return 0;
        }
        return vertex->Size();
    }

    template<typename DataType>
    TreapVertex<DataType>* GetTreapRoot(TreapVertex<DataType>* vertex) {
        if (!vertex) {
            return nullptr;
        }
        while (vertex->ancestor) {
            vertex = vertex->ancestor;
        }
        return vertex;
    }

    template<typename DataType>
    uint32_t PosNumberInTreap(TreapVertex<DataType>* vertex) {
        if (!vertex) {
            return 0;
        }
        uint32_t pos = vertex->LeftSize();
        while (vertex->ancestor) {
            if (vertex->ancestor->right_son == vertex) {
                pos += vertex->ancestor->LeftSize() + 1;
            }
            vertex = vertex->ancestor;
        }
        return pos;
    }

    template<typename DataType>
    std::pair<TreapVertex<DataType>*, TreapVertex<DataType>*>
    SplitTreap(TreapVertex<DataType> *vertex, uint32_t pivot) {
        if (!vertex) {
            return {nullptr, nullptr};
        }
        if (vertex->LeftSize() >= pivot) {
            auto [left, right] = SplitTreap(vertex->left_son, pivot);
            vertex->left_son = right;
            vertex->Update();
            return {left, vertex};
        } else {
            uint32_t shift = 1 + vertex->LeftSize();
            auto [left, right] = SplitTreap(vertex->right_son, pivot - shift);
            vertex->right_son = left;
            vertex->Update();
            return {vertex, right};
        }
    }

    template<typename DataType>
    TreapVertex<DataType>* MergeTreap(
            TreapVertex<DataType> *left,
            TreapVertex<DataType> *right) {
        if (!left) {
            return right;
        }
        if (!right) {
            return left;
        }
        if (left->treap_priority > right->treap_priority) {
            left->right_son = MergeTreap(left->right_son, right);
            left->Update();
            return left;
        } else {
            right->left_son = MergeTreap(left, right->left_son);
            right->Update();
            return right;
        }
    }

    template<typename DataType>
    TreapVertex<DataType>* InsertInTreap(
            TreapVertex<DataType>* root, TreapVertex<DataType>* new_vertex, int pos) {
        auto [left, right] = SplitTreap(root, pos);
        return MergeTreap(left, MergeTreap(new_vertex, right));
    }

    template<typename DataType>
    TreapVertex<DataType>* CycleShiftLeft(
            TreapVertex<DataType>* root, uint32_t shift) {
        auto [left, right] = SplitTreap(root, shift);
        return MergeTreap(right, left);
    }

    template<typename DataType>
    TreapVertex<DataType>* MoveToFirstPos(
            TreapVertex<DataType>* vertex,
            TreapVertex<DataType>* virtual_root = nullptr) {
        if (!vertex) {
            return nullptr;
        }
        uint32_t pos = PosNumberInTreap(vertex);
        if (!virtual_root) {
            virtual_root = GetTreapRoot(vertex);
        }
        return CycleShiftLeft(virtual_root, pos);
    }

    /*
    template<typename DataType>
    void PrintTreap(TreapVertex<DataType>* vertex) {
        if (!vertex) {
            return;
        }
        PrintTreap(vertex->left_son);
        std::cerr << std::string(vertex->edge) << ' ';
        PrintTreap(vertex->right_son);
    }
    */
}


#endif //DYNAMIC_FOREST_TREAP_H
