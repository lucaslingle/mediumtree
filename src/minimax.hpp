#pragma once
#include <algorithm>
#include <queue>
#include <memory>
#include <cstddef>
#include "node.hpp"

template <typename State> class MinimaxTree {
private:
    std::unique_ptr<Node<State>> root = std::unique_ptr<Node<State>>(new Node<State>());
public:
    MinimaxTree() = default;
    double get_value() const { return root->value; }
    void grow();
    void eval();
    void user_play(std::size_t action);
    std::size_t computer_play();
};

template <typename State>
void MinimaxTree<State>::grow() {
    // breadth-first search
    Node<State>* node;
    std::queue<Node<State>*> q;
    q.push(root.get());
    while (!q.empty()) {
        node = q.front();
        q.pop();
        for (std::size_t i = 0; i != State::get_num_actions(); ++i) {
            if (node->valid[i] && node->childrens[i] == nullptr) {
                node->childrens[i] = std::unique_ptr<Node<State>>(new Node<State>(node, i));
                q.push(node->childrens[i].get());
            }
        }
    }
}

template <typename State>
void MinimaxTree<State>::eval() {
    root->eval();
}

template <typename State>
void MinimaxTree<State>::user_play(std::size_t action) { 
    root->state.is_valid_thrower(action);
    root = std::move(root->childrens[action]);
    root->parent = nullptr;
}

template <typename State>
std::size_t MinimaxTree<State>::computer_play() {
    if (root->state.get_status() != InProgress)
        throw std::runtime_error("Game is not in progress.");

    auto best = std::find(root->action_values.begin(), root->action_values.end(), root->value);
    std::size_t action = std::distance(root->action_values.begin(), best);

    // This check shouldnt trigger since game is InProgress
    // there should be at least one action whose action_value is not a mask value
    root->state.is_valid_thrower(action);

    root = std::move(root->childrens[action]);
    root->parent = nullptr;
    return action;
}
