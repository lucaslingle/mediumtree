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
    State state;
    double value;
    std::size_t num_visits;
    std::array<bool, State::get_num_actions()> valid;
    std::array<std::unique_ptr<UCTNode<State>>, State::get_num_actions()> childrens = {};
};

template <typename State>
void UCTNode<State>::populate_valid(const State &state) {
    for (std::size_t i = 0; i != State::get_num_actions(); ++i)
        valid[i] = state.is_valid(i);
}

template <typename State>
UCTNode<State>::UCTNode()
: parent(nullptr), state(State()), value(0.0)
{
    populate_valid(state);
}

template <typename State>
UCTNode<State>::UCTNode(UCTNode *p, std::size_t action)
: parent(p), state(State(p->state, action)), value(0.0)
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
public:
    UCTTree(double coef): c(coef) { };
    double get_value() const { return root->value; }
    void user_play(std::size_t action);
    std::size_t computer_play(std::size_t num_rollouts);

    UCTNode* selection() const;
    UCTNode* expansion(UCTNode*);
    Status simulation(UCTNode*);
    void backpropagation(UCTNode*, Status);
    void add_node();
};

template <typename State>
UCTNode* selection() {
    using A = State::get_num_actions();
    UCTNode* node = root.get();
    while (!node->is_leaf() && node->state.get_status() == InProgress) {
        std::array<double, A> upper_bounds = {};
        for (std::size_t i = 0; i != A; ++i) {
            upper_bounds[i] = node->childrens[i].value;
            if (valid[i])
                upper_bounds[i] += coef * std::sqrt(std::log(node->num_visits) / node->childrens[i].num_visits);
            else
                upper_bounds[i] = -100;
        }
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
    double terminal_value;
    bool terminal_turn = node->state.get_turn();
    Status terminal_status = node->state.get_status();
    if (terminal_turn && terminal_status == PlayerOneWon)
        terminal_value = 1.0;
    if (!terminal_turn && terminal_status == PlayerOneWon)
        terminal_value = 0.0;
    if (terminal_turn && terminal_status == PlayerTwoWon)
        terminal_value = 0.0;
    if (!terminal_turn && terminal_status == PlayerTwoWon)
        terminal_value = 1.0;
    if (terminal_status == Tie)
        terminal_value = 0.5;

    node->value = terminal_value;
    while (node = node->parent) {
        double termval = (node.get_turn() == terminal_turn) ? terminal_value : (1.0 - terminal_value);
        double &value = node->value;
        std::size_t &num_visits = node->num_visits;
        value *= (num_visits / (num_visits + 1));
        value += (1 / (num_visits + 1)) * termval;
    }
}
