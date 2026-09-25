#pragma once
#include <algorithm>
#include <queue>
#include <memory>
#include <cstddef>
#include <stdexcept>
#include <cmath>
#include <random>
#include <limits>
#include <array>
#include "state.hpp"

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
: parent(nullptr), state(State()), value(0.0), num_visits(0)
{
    populate_valid(state);
}

template <typename State>
UCTNode<State>::UCTNode(UCTNode *p, std::size_t action)
: parent(p), state(State(p->state, action)), value(0.0), num_visits(0)
{
    populate_valid(state);
}

template <typename State>
bool UCTNode<State>::is_leaf() {
    for (std::size_t i = 0; i != State::get_num_actions(); ++i) {
        if (valid[i] && !childrens[i])
            return true;
    }
    return false;
}

template <typename State> class UCTTree {
private:
    std::unique_ptr<UCTNode<State>> root = std::unique_ptr<UCTNode<State>>(new UCTNode<State>());
    double coef;
    std::size_t num_rollouts;
    std::mt19937 rng{std::random_device{}()};
public:
    UCTTree(double c, std::size_t rollouts): coef(c), num_rollouts(rollouts) { 
        if (num_rollouts < State::get_num_actions())
            throw std::invalid_argument("Require num rollouts >= num actions.");
    };
    double get_value() const { return root->value; }
    void user_play(std::size_t action);
    std::size_t computer_play();

    // the functions below are only public for unit testing purposes.
    UCTNode<State>* selection() const;
    UCTNode<State>* expansion(UCTNode<State>*);
    Status simulation(UCTNode<State>*);
    void backpropagation(UCTNode<State>*, Status);
    UCTNode<State>* add_node();
};

template <typename State>
UCTNode<State>* UCTTree<State>::selection() const {
    constexpr std::size_t A = State::get_num_actions();
    UCTNode<State> *node = root.get();
    while (!node->is_leaf() && node->state.get_status() == InProgress) {
        std::array<double, A> upper_bounds = {};
        for (std::size_t i = 0; i != A; ++i) {
            if (node->valid[i]) {
                if (node->childrens[i]) {
                    upper_bounds[i] = node->childrens[i]->value;
                    upper_bounds[i] += coef * std::sqrt(std::log(node->num_visits) / node->childrens[i]->num_visits);
                } else {
                    upper_bounds[i] = std::numeric_limits<double>::infinity();
                }
            } else {
                upper_bounds[i] = -std::numeric_limits<double>::infinity();
            }
        }
        auto best = std::max_element(upper_bounds.begin(), upper_bounds.end());
        std::size_t action = std::distance(upper_bounds.begin(), best);
        node = node->childrens[action].get();
    }
    // when we get here we have either a leaf or a terminal node
    return node;
}

template <typename State>
UCTNode<State>* UCTTree<State>::expansion(UCTNode<State> *node) {
    // node is leaf or terminal
    if (node->state.get_status() != InProgress) // terminal
        return node;
    constexpr std::size_t A = State::get_num_actions();
    for (std::size_t i = 0; i != A; ++i)  // leaf
        if (node->valid[i] && !node->childrens[i]) {
            node->childrens[i] = std::unique_ptr<UCTNode<State>>(new UCTNode<State>(node, i));
            return node->childrens[i].get();
        }
    throw std::runtime_error("Node provided should be leaf or terminal.");
}

template <typename State>
Status UCTTree<State>::simulation(UCTNode<State> *node) {
    // node is child of leaf or terminal
    if (node->state.get_status() != InProgress)
        return node->state.get_status();

    constexpr std::size_t A = State::get_num_actions();
    std::array<bool, A> valid;
    State state = node->state;
    while (state.get_status() == InProgress) {
        for (std::size_t i = 0; i != A; ++i)
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
void UCTTree<State>::backpropagation(UCTNode<State> *node, Status terminal_status) {
    // node is child of leaf or terminal, so assign value and then backprop
    double rollout_value;
    bool leafchild_turn = node->state.get_turn();

    // The usual convention is to score each node from the point of view 
    // of the player who made the move into it. So winning rollouts from
    // child of leaf are actually scored as bad within that node, etc.
    if (leafchild_turn && terminal_status == PlayerOneWon)
        rollout_value = 0.0;
    if (!leafchild_turn && terminal_status == PlayerOneWon)
        rollout_value = 1.0;
    if (leafchild_turn && terminal_status == PlayerTwoWon)
        rollout_value = 1.0;
    if (!leafchild_turn && terminal_status == PlayerTwoWon)
        rollout_value = 0.0;
    if (terminal_status == Tie)
        rollout_value = 0.5;
    if (terminal_status == InProgress)
        throw std::runtime_error("Got terminal status InProgress.");

    node->value = rollout_value;
    node->num_visits += 1;
    while ((node = node->parent)) {
        double rel_rollout_value = (node->state.get_turn() == leafchild_turn) ? rollout_value : (1.0 - rollout_value);
        double &value = node->value;
        std::size_t &num_visits = node->num_visits;
        value *= (static_cast<double>(num_visits) / (num_visits + 1));
        value += (1.0 / (num_visits + 1)) * rel_rollout_value;
        num_visits += 1;
    }
}

template <typename State>
UCTNode<State>* UCTTree<State>::add_node() {
    // returned raw pointer to node is for unit testing purposes only
    UCTNode<State> *node_l = selection();
    UCTNode<State> *node_c = expansion(node_l);
    Status rollout_result = simulation(node_c);
    backpropagation(node_c, rollout_result);
    return node_c;
}

template <typename State>
void UCTTree<State>::user_play(std::size_t action) { 
    root->state.is_valid_thrower(action);
    if (!root->childrens[action])
        root->childrens[action] = std::unique_ptr<UCTNode<State>>(new UCTNode<State>(root.get(), action));
    root = std::move(root->childrens[action]);
    root->parent = nullptr;
}

template <typename State>
std::size_t UCTTree<State>::computer_play() {
    if (root->state.get_status() != InProgress)
        throw std::runtime_error("Game is not in progress.");

    for (std::size_t rollout = 0; rollout != num_rollouts; ++rollout)
        add_node();

    std::size_t most_visits = 0;
    std::size_t most_visited;
    for (std::size_t i = 0; i != State::get_num_actions(); ++i) {
        if (!root->childrens[i])  // shouldnt happen if num_rollouts > num_actions
            continue;
        if (root->valid[i]) {
            if (root->childrens[i]->num_visits > most_visits) {
                most_visited = i;
                most_visits = root->childrens[i]->num_visits;
            }
        }
    }
    std::size_t action = most_visited;

    // This check shouldnt trigger
    root->state.is_valid_thrower(action);

    root = std::move(root->childrens[action]);
    root->parent = nullptr;
    return action;
}
