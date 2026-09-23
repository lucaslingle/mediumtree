#pragma once
#include <array>
#include <algorithm>
#include <queue>
#include <memory>
#include <cstddef>
#include "state.hpp"
#include "tree.hpp"

template <typename State> class Tree;

template <typename State> class Node {
friend class Tree<State>; 
private:
    Node();
    Node(Node* p, int action);
    void populate_valid(const State &state);
    void eval();

    Node *parent;
    int prev_action;
    State state;
    double value;
    std::array<bool, State::get_num_actions()> valid;
    std::array<double, State::get_num_actions()> action_values;
    std::array<std::unique_ptr<Node<State>>, State::get_state_dim()> childrens = {};
};

template <typename State> class Tree {
private:
    std::unique_ptr<Node<State>> root = std::unique_ptr<Node<State>>(new Node<State>());
public:
    Tree() = default;
    double get_value() const { return root->value; }
    void grow();
    void eval();
    void user_play(std::size_t action);
    int computer_play();
};

template <typename State>
void Node<State>::populate_valid(const State &state) {
    for (std::size_t i = 0; i != State::get_num_actions(); ++i)
        valid[i] = state.is_valid(i);
}

template <typename State>
Node<State>::Node() : 
    parent(nullptr), prev_action(-1), state(State()), value(0.0) 
{
    populate_valid(state);
}

template <typename State>
Node<State>::Node(Node *p, int action) :
    parent(p), prev_action(action), state(State(p->state, action)), value(0.0)
{
    populate_valid(state);
}

template <typename State>
void Node<State>::eval() {
    auto status = state.get_status();
    if (status != InProgress) {
        if (status == Tie)
            value = 0;
        else
            value = (status == PlusWon) ? 1 : -1;
    } else {
        bool is_max = (state.get_turn() == 1);
        double best_val = (is_max) ? -100 : 100;
        for (std::size_t i = 0; i != State::get_num_actions(); ++i)
            if (valid[i]) {
                childrens[i]->eval();
                double val = childrens[i]->value;
                action_values[i] = val;
                best_val = (is_max) ? std::max(best_val, val) : std::min(best_val, val);
            } else {
                action_values[i] = (is_max) ? -100 : 100;
            }
        value = best_val;
    }
}

template <typename State>
void Tree<State>::grow() {
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
void Tree<State>::eval() {
    root->eval();
}

template <typename State>
void Tree<State>::user_play(std::size_t action) { 
    root->state.is_valid_thrower(action);
    root = std::move(root->childrens[action]);
    root->parent = nullptr;
}

template <typename State>
int Tree<State>::computer_play() {
    if (root->state.get_status() != InProgress)
        throw std::runtime_error("Game is not in progress.");

    auto best = std::find(root->action_values.begin(), root->action_values.end(), root->value);
    int action = std::distance(root->action_values.begin(), best);

    // This check shouldnt trigger since game is InProgress
    // there should be at least one action whose action_value is not a mask value
    root->state.is_valid_thrower(action);

    root = std::move(root->childrens[action]);
    root->parent = nullptr;
    return action;
}
