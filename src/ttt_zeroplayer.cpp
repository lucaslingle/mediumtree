#include <iostream>
#include "ttt.hpp"
#include "minimax.hpp"

int main() {
    MinimaxTree<TicTacToeState> tree;
    tree.grow();
    tree.eval();
    std::cout << tree.get_value() << std::endl;
    return 0;
}
