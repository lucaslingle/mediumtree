#include <stdexcept>
#include "ttt.hpp"

TicTacToeState::TicTacToeState(const TicTacToeState &state, std::size_t action)
: AbstractState(state, action) {
    is_valid_thrower(action);
    board[action] = get_turn() ? 1 : 2;
    turn = !get_turn();
    status = compute_status();
}

std::ostream& TicTacToeState::print(std::ostream &os) const {
    for (std::size_t i = 0; i != 9; ++i) {
        if (i % 3 == 0)
            os << "\n";
        os << board[i] << " ";
    }
    os << "\n";
    return os;
}

bool TicTacToeState::is_valid(std::size_t action) const {
    if (get_status() != InProgress)
        return false;
    if (action >= get_num_actions())
        return false;
    if (board[action] != 0)
        return false;
    return true;
}

void TicTacToeState::is_valid_thrower(std::size_t action) const {
    if (get_status() != InProgress)
        throw std::runtime_error("Game is not in progress.");
    if (action >= get_num_actions())
        throw std::invalid_argument("Invalid action.");
    if (board[action] != 0)
        throw std::invalid_argument("Invalid action.");
}

Status TicTacToeState::compute_status() const {
    for (int i = 0; i != 3; ++i) {
        // check row i
        if (board[3 * i + 0] == board[3 * i + 1] && board[3 * i + 1] == board[3 * i + 2]) {
            if (board[3 * i] == 1)
                return PlayerOneWon;
            if (board[3 * i] == 2)
                return PlayerTwoWon;
        }
        // check column i
        if (board[0 + i] == board[3 + i] && board[3 + i] == board[6 + i]) {
            if (board[0 + i] == 1)
                return PlayerOneWon;
            if (board[0 + i] == 2)
                return PlayerTwoWon;
        }
    }
    // check diagonals
    if (board[3 * 0 + 0] == board[3 * 1 + 1] && board[3 * 1 + 1] == board[3 * 2 + 2]) {
        if (board[3 * 0 + 0] == 1)
            return PlayerOneWon;
        if (board[3 * 0 + 0] == 2)
            return PlayerTwoWon;
    }
    if (board[3 * 0 + 2] == board[3 * 1 + 1] && board[3 * 1 + 1] == board[3 * 2 + 0]) {
        if (board[3 * 0 + 2] == 1)
            return PlayerOneWon;
        if (board[3 * 0 + 2] == 2)
            return PlayerTwoWon;
    }
    for (int i = 0; i != 9; ++i)
        if (board[i] == 0)
            return InProgress;
    return Tie;
}
