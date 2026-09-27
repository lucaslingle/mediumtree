# mediumtree
UCT in C++, applied to connect four and tic tac toe

### Build
For now, you can simply build with clang as follows
```bash
clang++ -std=c++11 -stdlib=libc++ c4_app.cpp c4.cpp -o c4_app  # connect four
clang++ -std=c++11 -stdlib=libc++ ttt_app.cpp ttt.cpp -o ttt_app  # tic tac toe
```

### Usage

#### Conenct Four
Start a Connect Four game against the computer as follows:
```bash
./build/c4_app
```
The move locations use zero-based order, like so:
```
0 1 2 3 4 5
```
The computer will move immediately after you, before the board is printed. The player 1 and 2 icons are denoted by those numbers. Empty spaces are denoted zero. 

#### Tic Tac Toe
The board and controls for Tic Tac Toe the same as [smalltree](https://github.com/lucaslingle/smalltree), except that 1 and 2 are used for X and O (instead of 1 and -1).
