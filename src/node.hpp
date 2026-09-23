#pragma once
#include <array>
#include <memory>
#include <cstddef>

template <typename State> class Node {
public:
    Node();
    Node(Node* p, std::size_t action);
    void populate_valid(const State &state);
    void eval();
    Node *parent;
    std::size_t prev_action;
    State state;
    double value;
    std::array<bool, State::get_num_actions()> valid;
    std::array<double, State::get_num_actions()> action_values;
    std::array<std::unique_ptr<Node<State>>, State::get_num_actions()> childrens = {};
};

template <typename State>
void Node<State>::populate_valid(const State &state) {
    for (std::size_t i = 0; i != State::get_num_actions(); ++i)
        valid[i] = state.is_valid(i);
}

template <typename State>
Node<State>::Node() : 
    parent(nullptr), prev_action(0), state(State()), value(0.0) 
{
    populate_valid(state);
}

template <typename State>
Node<State>::Node(Node *p, std::size_t action) :
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
            value = (status == PlayerOneWon) ? 1 : -1;
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
