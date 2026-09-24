#pragma once
#include <iostream>
#include <array>
#include <string>
#include "state.hpp"

class TicTacToeState : public AbstractState<int, 9, 9> {
public:
    TicTacToeState() = default;
    TicTacToeState(const TicTacToeState &state, std::size_t action);
    std::ostream& print(std::ostream& os) const override;
    bool is_valid(std::size_t action) const override;
    void is_valid_thrower(std::size_t action) const override;
    Status compute_status() const override;
};
