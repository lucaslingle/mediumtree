#include <cstddef>
#include <initializer_list>
#include <stdexcept>
#include <gtest/gtest.h>
#include "ttt.hpp"

namespace {

TicTacToeState play(std::initializer_list<std::size_t> actions) {
    TicTacToeState state;
    for (std::size_t action : actions)
        state = TicTacToeState(state, action);
    return state;
}

}  // namespace

TEST(TicTacToeState, StartsEmptyWithPlayerOneToMove) {
    TicTacToeState state;
    EXPECT_EQ(state.get_status(), InProgress);
    EXPECT_TRUE(state.get_turn());
    for (std::size_t i = 0; i != TicTacToeState::get_num_actions(); ++i)
        EXPECT_TRUE(state.is_valid(i)) << "action " << i;
}

TEST(TicTacToeState, TurnAlternates) {
    EXPECT_FALSE(play({4}).get_turn());
    EXPECT_TRUE(play({4, 0}).get_turn());
}

TEST(TicTacToeState, RejectsOccupiedSquare) {
    TicTacToeState state = play({4});
    EXPECT_FALSE(state.is_valid(4));
    EXPECT_THROW(TicTacToeState(state, 4), std::invalid_argument);
}

TEST(TicTacToeState, RejectsOutOfRangeAction) {
    TicTacToeState state;
    EXPECT_FALSE(state.is_valid(9));
    EXPECT_THROW(TicTacToeState(state, 9), std::invalid_argument);
}

TEST(TicTacToeState, DetectsRowWin) {
    // 1 takes the top row while 2 plays in the middle row.
    EXPECT_EQ(play({0, 3, 1, 4, 2}).get_status(), PlayerOneWon);
}

TEST(TicTacToeState, DetectsColumnWin) {
    // 2 takes the right column.
    EXPECT_EQ(play({0, 2, 1, 5, 3, 8}).get_status(), PlayerTwoWon);
}

TEST(TicTacToeState, DetectsDiagonalWins) {
    EXPECT_EQ(play({0, 1, 4, 2, 8}).get_status(), PlayerOneWon);
    EXPECT_EQ(play({2, 0, 4, 1, 6}).get_status(), PlayerOneWon);
}

TEST(TicTacToeState, DetectsTie) {
    // 1 2 1
    // 1 2 2
    // 2 1 1
    TicTacToeState state = play({0, 1, 2, 4, 3, 5, 7, 6, 8});
    EXPECT_EQ(state.get_status(), Tie);
    EXPECT_EQ(state.get_status_string(), "Tie");
}

TEST(TicTacToeState, RejectsMovesAfterGameOver) {
    TicTacToeState state = play({0, 3, 1, 4, 2});
    EXPECT_FALSE(state.is_valid(8));
    EXPECT_THROW(TicTacToeState(state, 8), std::runtime_error);
}
