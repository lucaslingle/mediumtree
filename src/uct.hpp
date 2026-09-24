#pragma once
#include <algorithm>
#include <queue>
#include <memory>
#include <cstddef>
#include <stdexcept>
#include <cmath>
#include <random>

template <typename State> class UCTNode {
public:
    UCTNode();
    UCTNode(UCTNode* p, std::size_t action);
    void populate_valid(const State &state);
    bool is_leaf();
    UCTNode *parent;
    std::size_t prev_action;
    State state;
    double value;
    std::array<bool, State::get_num_actions()> valid;
    std::array<double, State::get_num_actions()> action_values;
    std::array<std::size_t, State::get_num_actions()> visit_counts = {};
    std::array<std::unique_ptr<MinimaxNode<State>>, State::get_num_actions()> childrens = {};
};

template <typename State>
void UCTNode<State>::populate_valid(const State &state) {
    for (std::size_t i = 0; i != State::get_num_actions(); ++i)
        valid[i] = state.is_valid(i);
}

template <typename State>
UCTNode<State>::UCTNode()
: parent(nullptr), prev_action(0), state(State()), value(0.0)
{
    populate_valid(state);
}

template <typename State>
UCTNode<State>::UCTNode(UCTNode *p, std::size_t action)
: parent(p), prev_action(action), state(State(p->state, action)), value(0.0)
{
    populate_valid(state);
}

template <typename State>
bool UCTNode<State>::is_leaf() {
    for (std::size_t i = 0; i != State::get_num_actions(); ++i) {
        if (valid[i] && childrens[i] == nullptr)
            return true;
    }
    return false;
}

template <typename State> class UCTTree {
private:
    std::unique_ptr<UCTNode<State>> root = std::unique_ptr<UCTNode<State>>(new UCTNode<State>());
    double coef;
    std::mt19937 rng(std::random_device{}());
    UCTNode* selection() const;
    UCTNode* expansion(UCTNode*);
    Status simulation(UCTNode*);
    void backpropagation(UCTNode*, Status);
public:
    UCTTree(double coef): c(coef) { };
    double get_value() const { return root->value; }
    void user_play(std::size_t action);
    std::size_t computer_play();
};

template <typename State>
UCTNode* selection() {
    using A = State::get_num_actions();
    UCTNode* node = root.get();
    while (!node->is_leaf() && node->state.get_status() == InProgress) {
        std::array<double, A> upper_bounds = action_values;
        for (std::size_t i = 0; i != A; ++i)
            if (valid[i])
                upper_bounds[i] += coef * std::sqrt(std::log(std::sum(node->visit_count)) / node->visit_count[i]);
            else
                upper_bounds[i] = -100;
        node = std::find(upper_bounds.begin(), upper_bounds.end(), root->value).get();
    }
    // when we get here we have either a leaf or a terminal node
    return node;
}

template <typename State>
UCTNode* expansion(UCTNode *node) {
    // node is leaf or terminal
    if (node->state.get_status() != InProgress) // terminal
        return node;
    using A = State::get_num_actions();
    for (std::size_t i = 0; i != A; ++i)  // leaf
        if (node->childrens[i] == nullptr) {
            node->childrens[i] = std::unique_ptr<UCTNode<State>>(new UCTNode<State>(node, i));
            return node->childrens[i].get();
        }
    throw std::runtime_error("Node provided should be leaf or terminal.");
}

template <typename State>
Status simulation(UCTNode *node) {
    // node is leaf or terminal
    if (node->state.get_status() != InProgress)
        return node->state.get_status();
    using A = State::get_num_actions();
    State state = node->state;
    std::array<bool, A> valid;

    while (state.get_status() == InProgress) {
        for (std::size_t i = 0; i != State::get_num_actions(); ++i)
            valid[i] = state.is_valid(i);
        std::discrete_distribution<std::size_t> dist(valid.begin(), valid.end());
        std::size_t action = dist(rng);
        if (!valid[action])
            throw std::runtime_error("Simulated an invalid action.");
        state = State(state, action);
    }
    return state.get_status();
}

template <typename State>
void backpropagation(UCTNode *node, Status s) {
    // node is terminal, so assign value and then backprop somehow
}