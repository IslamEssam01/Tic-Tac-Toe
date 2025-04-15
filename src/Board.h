#ifndef BOARD_H
#define BOARD_H

#include <string>
#include <vector>
#include <utility>  //for using pair

// modes of play
enum class Player { None, X, O };

// convert play to char for printing
inline char playerToChar(Player p) {
    switch (p) {
        case Player::X: return 'X';
        case Player::O: return 'O';
        default:        return '-';
    }
}

// Struct for details of winning
struct WinInfo {
    Player winner;                           // player who win
    std::string type;                        // "row", "col", "diag", "anti-diag", "none"
    int index;                               // no of row,col and diag
    std::vector<std::pair<int, int>> winCells; // places of winning
};

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