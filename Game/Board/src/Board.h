#ifndef BOARD_H
#define BOARD_H

#include "globals.h"  // Add this include at the top

// manage game logic
class Board {
public:
    Board();

    bool makeMove(int row, int col, Player p);
    bool isValidMove(int row, int col) const;
    bool isCellEmpty(int row, int col) const;
    bool isFull() const;
    void reset();
    void print() const;

    WinInfo checkWinner() const;
    bool isGameOver() const;

private:
    Player grid[3][3]; // 3*3 board
};

#endif // BOARD_H
