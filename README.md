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
