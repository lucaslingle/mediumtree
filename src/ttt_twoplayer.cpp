#include <iostream>
#include <cstddef>
#include "ttt.hpp"

int main() {
    TicTacToeState game;
    std::size_t action;
    while (game.get_status() == InProgress) {
        std::cout << game;
        std::cout << (game.get_turn() ? 1 : 2) << " to play. Where should they go?" << std::endl;
        
        if (std::cin >> action)
            game = TicTacToeState(game, action);
        else {
            std::cerr << "Invalid input." << std::endl;
            return -1;
        }
    }
    std::cout << "Game status: " << game.get_status_string() << std::endl;
    return 0;
}
