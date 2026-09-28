# mediumtree
UCT in C++, applied to connect four and tic tac toe

### Build
Build with CMake from the repository root:
```bash
cmake -S . -B build
cmake --build build
```
This produces `build/c4_app` (connect four) and `build/ttt_app` (tic tac toe).

### Usage

#### Connect Four
Start a Connect Four game against the computer as follows:
```bash
./build/c4_app
```
The move locations use zero-based order, like so:
```
0 1 2 3 4 5 6
```
The computer will move immediately after you, before the board is printed. The player 1 and 2 icons are denoted by those numbers. Empty spaces are denoted zero. 

#### Tic Tac Toe
Start a Tic Tac Toe game against the computer as follows:
```bash
./build/ttt_app
```
The board and controls for Tic Tac Toe are the same as [smalltree](https://github.com/lucaslingle/smalltree), except that 1 and 2 are used for X and O.

### Testing
Unit tests use [GoogleTest](https://github.com/google/googletest), which CMake downloads automatically the first time you configure. The build steps above also produce `build/unit_tests`. Run the tests with:
```bash
ctest --test-dir build
```
or run the test executable directly for GoogleTest's own output:
```bash
./build/unit_tests
```
Tests live in `tests/`. A few UCT tests depend on random rollouts, so they can fail by chance, though this should be rare.

To build without the tests (and skip the GoogleTest download), configure with:
```bash
cmake -S . -B build -DMEDIUMTREE_BUILD_TESTS=OFF
```

### References
```
https://en.wikipedia.org/wiki/Tic-tac-toe
https://en.wikipedia.org/wiki/Connect_Four
https://en.wikipedia.org/wiki/Monte_Carlo_tree_search#Principle_of_operation
https://www.researchgate.net/publication/221112399_Bandit_Based_Monte-Carlo_Planning
```
