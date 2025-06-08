#include "Board.h"
#include <iostream>

// Constructor: start the game all players are none
Board::Board() {
    reset();
}

// record move of play if valid
bool Board::makeMove(int row, int col, Player p) {
    if (isValidMove(row, col)) {
        grid[row][col] = p;
        return true;
    }
    return false;
}

// check for validation of move (inside board and empty)
bool Board::isValidMove(int row, int col) const {
    return (row >= 0 && row < 3 && col >= 0 && col < 3 && isCellEmpty(row, col));
}

// check empty
bool Board::isCellEmpty(int row, int col) const {
    return grid[row][col] == Player::None;
}

// check for full board
bool Board::isFull() const {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (grid[i][j] == Player::None)
                return false;
    return true;
}

// reset my board
void Board::reset() {
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            grid[i][j] = Player::None;
}

// for printing on console
void Board::print() const {
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            std::cout << playerToChar(grid[i][j]) << " ";
        }
        std::cout << std::endl;
    }
}

// decide who won, func return srtuct wininfo
WinInfo Board::checkWinner() const {
    // in rows
    for (int i = 0; i < 3; ++i) {
        if (grid[i][0] != Player::None &&
            grid[i][0] == grid[i][1] &&
            grid[i][1] == grid[i][2]) {
            return { grid[i][0], "row", i, { {i, 0}, {i, 1}, {i, 2} } };
        }
    }

    // in cols
    for (int i = 0; i < 3; ++i) {
        if (grid[0][i] != Player::None &&
            grid[0][i] == grid[1][i] &&
            grid[1][i] == grid[2][i]) {
            return { grid[0][i], "col", i, { {0, i}, {1, i}, {2, i} } };
        }
    }

    // main diag its index -1
    if (grid[0][0] != Player::None &&
        grid[0][0] == grid[1][1] &&
        grid[1][1] == grid[2][2]) {
        return { grid[0][0], "diag", -1, { {0, 0}, {1, 1}, {2, 2} } };
    }

    // anti-diag its index -1
    if (grid[0][2] != Player::None &&
        grid[0][2] == grid[1][1] &&
        grid[1][1] == grid[2][0]) {
        return { grid[0][2], "anti-diag", -1, { {0, 2}, {1, 1}, {2, 0} } };
    }

    return { Player::None, "none", -1, {} };
}

// Game over!
bool Board::isGameOver() const {
    return (checkWinner().winner != Player::None || isFull());
}
