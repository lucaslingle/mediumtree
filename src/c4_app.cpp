#include <iostream>
#include <cstddef>
#include "c4.hpp"
#include "uct.hpp"

int main() {
    ConnectFourState game;
    UCTTree<ConnectFourState> tree(1.4, 1000);

    std::size_t action;
    while (game.get_status() == InProgress) {
        std::cout << game;
        std::cout << "Your turn. Where will you go?" << std::endl;

        if (std::cin >> action) {
            tree.user_play(action);
            game = ConnectFourState(game, action);
        } else {
            std::cerr << "Invalid input." << std::endl;
            return -1;
        }

        if (game.get_status() == InProgress) {
            action = tree.computer_play();
            game = ConnectFourState(game, action);
        }
    }
    std::cout << game;
    std::cout << "Game status: " << game.get_status_string() << std::endl;
    return 0;
}
