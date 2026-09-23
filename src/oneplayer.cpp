#include <iostream>
#include <cstddef>
#include "tictactoe.hpp"
#include "tree.hpp"

int main() {
    TicTacToeState game;
    Tree<TicTacToeState> tree;
    tree.grow();
    tree.eval();

    std::size_t action;
    while (game.get_status() == InProgress) {
        std::cout << game;
        std::cout << "Your turn. Where will you go?" << std::endl;

        if (std::cin >> action) {
            tree.user_play(action);
            game = TicTacToeState(game, action);
        } else {
            std::cerr << "Invalid input." << std::endl;
            return -1;
        }

        if (game.get_status() == InProgress) {
            action = tree.computer_play();
            game = TicTacToeState(game, action);
        }
    }
    std::cout << game;
    std::cout << "Game status: " << game.get_status_string() << std::endl;
    return 0;
}