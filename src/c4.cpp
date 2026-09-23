#include "c4.hpp"

ConnectFourState::ConnectFourState(const ConnectFourState &state, std::size_t action)
: AbstractState(state, action) {
    is_valid_thrower(action);
    for (std::size_t row = 5; row != -1; --row)
        if (board[row * 7 + action] == 0) {
            board[row * 7 + action] = get_turn() ? 1 : 2;
            break;
        }
    turn = !get_turn();
}

std::ostream& ConnectFourState::print(std::ostream &os) const {
    for (std::size_t i = 0; i != 6 * 7; ++i) {
        if (i % 7 == 0)
            os << "\n";
        os << board[i] << " ";
    }
    os << "\n";
    return os;
}

bool ConnectFourState::is_valid(std::size_t action) const {
    if (get_status() != InProgress)
        return false;
    if (action >= get_num_actions())
        return false;
    for (std::size_t row = 5; row != -1; --row)
        if (board[row * 7 + action] == 0)
            return true;
    return false;
}

void ConnectFourState::is_valid_thrower(std::size_t action) const {
    if (get_status() != InProgress)
        throw std::runtime_error("Game is not in progress.");
    if (action >= get_num_actions())
        throw std::invalid_argument("Invalid action.");
    bool valid = false;
    for (std::size_t row = 5; row != -1; --row)
        if (board[row * 7 + action] == 0) {
            valid = true;
            break;
        }
    if (!valid)
        throw std::invalid_argument("Invalid action.");
}

Status ConnectFourState::get_status() const {
    // check vertical
    for (int col = 0; col != 7; ++col) {
        for (int top_row = 0; top_row != 3; ++top_row) {
            int loc0 = board[top_row * 7 + col];
            int loc1 = board[(top_row + 1) * 7 + col];
            int loc2 = board[(top_row + 2) * 7 + col];
            int loc3 = board[(top_row + 3) * 7 + col];
            if ((loc0 == loc1) && (loc1 == loc2) && (loc2 == loc3)) {
                if (loc0 == 1)
                    return PlayerOneWon;
                if (loc0 == 2)
                    return PlayerTwoWon;
            }
        }
    }

    // check horizontal
    for (int row = 0; row != 6; ++row) {
        for (int left_col = 0; left_col != 4; ++left_col) {
            int loc0 = board[row * 7 + left_col];
            int loc1 = board[row * 7 + left_col + 1];
            int loc2 = board[row * 7 + left_col + 2];
            int loc3 = board[row * 7 + left_col + 3];
            if ((loc0 == loc1) && (loc1 == loc2) && (loc2 == loc3)) {
                if (loc0 == 1)
                    return PlayerOneWon;
                if (loc0 == 2)
                    return PlayerTwoWon;
            }
        }
    }

    // check diagonal forward-slash-like ('/')
    for (int col = 3; col != 7; ++col) {
        for (int top_row = 0; top_row != 3; ++top_row) {
            int loc0 = board[top_row * 7 + col];
            int loc1 = board[(top_row + 1) * 7 + (col - 1)];
            int loc2 = board[(top_row + 2) * 7 + (col - 2)];
            int loc3 = board[(top_row + 3) * 7 + (col - 3)];
            if ((loc0 == loc1) && (loc1 == loc2) && (loc2 == loc3)) {
                if (loc0 == 1)
                    return PlayerOneWon;
                if (loc0 == 2)
                    return PlayerTwoWon;
            }
        }
    }

    // check diagonal forward-slash-like ('\')
    for (int col = 0; col != 4; ++col) {
        for (int top_row = 0; top_row != 3; ++top_row) {
            int loc0 = board[top_row * 7 + col];
            int loc1 = board[(top_row + 1) * 7 + (col + 1)];
            int loc2 = board[(top_row + 2) * 7 + (col + 2)];
            int loc3 = board[(top_row + 3) * 7 + (col + 3)];
            if ((loc0 == loc1) && (loc1 == loc2) && (loc2 == loc3)) {
                if (loc0 == 1)
                    return PlayerOneWon;
                if (loc0 == 2)
                    return PlayerTwoWon;
            }
        }
    }

    for (int i = 0; i != 6 * 7; ++i)
        if (board[i] == 0)
            return InProgress;
    return Tie;
}
