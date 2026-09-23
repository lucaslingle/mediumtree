#pragma once
#include <iostream>
#include <array>
#include <string>
#include "state.hpp"

class TicTacToeState : public AbstractState<int, 9, 9> {
public:
    TicTacToeState() = default;
    Status get_status() const override;
    bool is_valid(std::size_t action) const override;
    void is_valid_thrower(std::size_t action) const override;
    std::ostream& print(std::ostream& os) const override;
    TicTacToeState(const TicTacToeState &state, std::size_t action);
};
