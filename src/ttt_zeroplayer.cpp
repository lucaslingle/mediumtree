#include <iostream>
#include "ttt.hpp"
#include "tree.hpp"

int main() {
    Tree<TicTacToeState> tree;
    tree.grow();
    tree.eval();
    std::cout << tree.get_value() << std::endl;
    return 0;
}
