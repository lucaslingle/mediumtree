#pragma once
#include <iostream>
#include <array>
#include <string>

enum Status {
    InProgress,
    PlayerOneWon,
    PlayerTwoWon,
    Tie
};

template <typename PieceType, std::size_t StateDim, std::size_t ActionCount>
class AbstractState {
public:
    friend std::ostream& operator<<(std::ostream &os, const AbstractState &state) {
        return state.print(os);
    }
    AbstractState() = default;
    static constexpr std::size_t get_state_dim() { return StateDim; }
    static constexpr std::size_t get_num_actions() { return ActionCount; }
    std::string get_status_string() const;
    bool get_turn() const { return turn; }
    
    virtual std::ostream& print(std::ostream& os) const = 0;
    virtual bool is_valid(std::size_t action) const = 0;
    virtual void is_valid_thrower(std::size_t action) const = 0;
    virtual Status get_status() const = 0;
    virtual ~AbstractState() = default;
protected:
    std::array<PieceType, StateDim> board = {};
    bool turn = true;
    AbstractState(const AbstractState &state, std::size_t action)
    : board(state.board), turn(state.turn) { }
};

template <typename PieceType, std::size_t StateDim, std::size_t ActionCount>
inline std::string AbstractState<PieceType, StateDim, ActionCount>::get_status_string() const {
    Status s = get_status();
    switch (s) {
        case Status::InProgress:   return "In Progress";
        case Status::PlayerOneWon: return "Player 1 won";
        case Status::PlayerTwoWon: return "Player 2 won";
        default:                   return "Tie";
    }
}
