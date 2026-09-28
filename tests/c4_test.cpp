#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <gtest/gtest.h>
#include "c4.hpp"

namespace {

ConnectFourState play(std::initializer_list<std::size_t> actions) {
    ConnectFourState state;
    for (std::size_t action : actions)
        state = ConnectFourState(state, action);
    return state;
}

}  // namespace

TEST(ConnectFourState, StartsEmptyWithPlayerOneToMove) {
    ConnectFourState state;
    EXPECT_EQ(state.get_status(), InProgress);
    EXPECT_TRUE(state.get_turn());
    for (std::size_t col = 0; col != ConnectFourState::get_num_actions(); ++col)
        EXPECT_TRUE(state.is_valid(col)) << "column " << col;
}

TEST(ConnectFourState, RejectsOutOfRangeColumn) {
    ConnectFourState state;
    EXPECT_FALSE(state.is_valid(7));
    EXPECT_THROW(ConnectFourState(state, 7), std::invalid_argument);
}

TEST(ConnectFourState, RejectsFullColumn) {
    // Alternating players, so six pieces in column 0 without a vertical four.
    ConnectFourState state = play({0, 0, 0, 0, 0, 0});
    EXPECT_EQ(state.get_status(), InProgress);
    EXPECT_FALSE(state.is_valid(0));
    EXPECT_THROW(ConnectFourState(state, 0), std::invalid_argument);
    EXPECT_TRUE(state.is_valid(1));
}

TEST(ConnectFourState, DetectsVerticalWin) {
    EXPECT_EQ(play({0, 1, 0, 1, 0, 1, 0}).get_status(), PlayerOneWon);
}

TEST(ConnectFourState, DetectsHorizontalWin) {
    // 1 fills the bottom row of columns 0-3; 2 stacks on top.
    EXPECT_EQ(play({0, 0, 1, 1, 2, 2, 3}).get_status(), PlayerOneWon);
}

TEST(ConnectFourState, DetectsRisingDiagonalWin) {
    // 1 ends with pieces at heights 1, 2, 3, 4 in columns 0, 1, 2, 3.
    EXPECT_EQ(play({0, 1, 1, 2, 2, 3, 2, 3, 3, 6, 3}).get_status(), PlayerOneWon);
}

TEST(ConnectFourState, DetectsFallingDiagonalWin) {
    // Mirror image of the rising diagonal: columns 6, 5, 4, 3.
    EXPECT_EQ(play({6, 5, 5, 4, 4, 3, 4, 3, 3, 0, 3}).get_status(), PlayerOneWon);
}

TEST(ConnectFourState, DetectsPlayerTwoWin) {
    EXPECT_EQ(play({0, 1, 0, 1, 0, 1, 6, 1}).get_status(), PlayerTwoWon);
}
