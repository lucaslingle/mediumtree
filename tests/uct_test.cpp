#include <cstddef>
#include <stdexcept>
#include <gtest/gtest.h>
#include "c4.hpp"
#include "ttt.hpp"
#include "uct.hpp"

TEST(UCTTree, RequiresAtLeastOneRolloutPerAction) {
    EXPECT_THROW(UCTTree<TicTacToeState>(1.4, 8), std::invalid_argument);
    EXPECT_NO_THROW(UCTTree<TicTacToeState>(1.4, 9));
}

TEST(UCTTree, ExpandsEveryRootChildBeforeGoingDeeper) {
    UCTTree<TicTacToeState> tree(1.4, 9);
    for (std::size_t i = 0; i != TicTacToeState::get_num_actions(); ++i) {
        UCTNode<TicTacToeState> *node = tree.add_node();
        ASSERT_NE(node->parent, nullptr);
        EXPECT_EQ(node->parent->parent, nullptr) << "rollout " << i << " went below the root";
        EXPECT_EQ(node->num_visits, 1u);
    }
    // All nine root children now exist, so the next rollout goes one level deeper.
    UCTNode<TicTacToeState> *node = tree.add_node();
    ASSERT_NE(node->parent, nullptr);
    ASSERT_NE(node->parent->parent, nullptr);
    EXPECT_EQ(node->parent->parent->parent, nullptr);
}

TEST(UCTTree, RootValueStaysInUnitInterval) {
    UCTTree<TicTacToeState> tree(1.4, 9);
    for (int i = 0; i != 1000; ++i)
        tree.add_node();
    EXPECT_GE(tree.get_value(), 0.0);
    EXPECT_LE(tree.get_value(), 1.0);
}

TEST(UCTTree, UserPlayRejectsInvalidMove) {
    UCTTree<TicTacToeState> tree(1.4, 9);
    tree.user_play(4);
    EXPECT_THROW(tree.user_play(4), std::invalid_argument);
}

// The tests below depend on random rollouts. They use enough rollouts that the
// right move wins by a wide margin, but a failure here is not always a bug.

TEST(UCTTree, TicTacToeTakesImmediateWin) {
    // 1 1 .      2 to move; 1 threatens 2, but 5 wins outright for 2.
    // 2 2 .
    // . . 1
    UCTTree<TicTacToeState> tree(1.4, 5000);
    for (std::size_t action : {0, 3, 1, 4, 8})
        tree.user_play(action);
    EXPECT_EQ(tree.computer_play(), 5u);
}

TEST(UCTTree, TicTacToeBlocksImmediateLoss) {
    // 2 . .      2 to move; must block 1 at 5.
    // 1 1 .
    // . . .
    UCTTree<TicTacToeState> tree(1.4, 5000);
    for (std::size_t action : {4, 0, 3})
        tree.user_play(action);
    EXPECT_EQ(tree.computer_play(), 5u);
}

TEST(UCTTree, ConnectFourBlocksVerticalThreat) {
    // 1 has three stacked in column 3; 2 must play column 3.
    UCTTree<ConnectFourState> tree(1.4, 20000);
    for (std::size_t action : {3, 0, 3, 6, 3})
        tree.user_play(action);
    EXPECT_EQ(tree.computer_play(), 3u);
}
