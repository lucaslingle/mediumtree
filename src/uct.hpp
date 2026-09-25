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
        if (valid[i] && childrens[i] == nullptr)
            return true;
    }
    return false;
}

template <typename State> class UCTTree {
private:
    std::unique_ptr<UCTNode<State>> root = std::unique_ptr<UCTNode<State>>(new UCTNode<State>());
    double coef;
    std::mt19937 rng{std::random_device{}()};
public:
    UCTTree(double c): coef(c) { };
    double get_value() const { return root->value; }
    void user_play(std::size_t action);
    std::size_t computer_play(std::size_t num_rollouts);

    UCTNode<State>* selection() const;
    UCTNode<State>* expansion(UCTNode<State>*);
    Status simulation(UCTNode<State>*);
    void backpropagation(UCTNode<State>*, Status);
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
        auto iter = std::max_element(upper_bounds.begin(), upper_bounds.end());
        node = (*iter).get();
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
        if (node->valid[i] && node->childrens[i] == nullptr) {
            node->childrens[i] = std::unique_ptr<UCTNode<State>>(new UCTNode<State>(node, i));
            return node->childrens[i].get();
        }
    throw std::runtime_error("Node provided should be leaf or terminal.");
}

template <typename State>
Status UCTTree<State>::simulation(UCTNode<State> *node) {
    // node is leaf or terminal
    if (node->state.get_status() != InProgress)
        return node->state.get_status();

    constexpr std::size_t A = State::get_num_actions();
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
void UCTTree<State>::backpropagation(UCTNode<State> *node, Status terminal_status) {
    // node is leaf or terminal, so assign value and then backprop
    double rollout_value;
    bool leaf_turn = node->state.get_turn();

    // The usual convention is to score each node from the point of view 
    // of the player who made the move into it. So winning rollouts from
    // leaf are actually scored as bad, etc.
    if (leaf_turn && terminal_status == PlayerOneWon)
        rollout_value = 0.0;
    if (!leaf_turn && terminal_status == PlayerOneWon)
        rollout_value = 1.0;
    if (leaf_turn && terminal_status == PlayerTwoWon)
        rollout_value = 1.0;
    if (!leaf_turn && terminal_status == PlayerTwoWon)
        rollout_value = 0.0;
    if (terminal_status == Tie)
        rollout_value = 0.5;
    if (terminal_status == InProgress)
        throw std::runtime_error("Got terminal status InProgress.");

    node->value = rollout_value;
    node->num_visits += 1;
    while ((node = node->parent)) {
        double rel_rollout_value = (nod->state.get_turn() == leaf_turn) ? rollout_value : (1.0 - rollout_value);
        double &value = node->value;
        std::size_t &num_visits = node->num_visits;
        value *= (static_cast<double>(num_visits) / (num_visits + 1));
        value += (1.0 / (num_visits + 1)) * rel_rollout_value;
        num_visits += 1;
    }
}
