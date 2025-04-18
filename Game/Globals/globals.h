#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>
#include <vector>
#include <utility>  //for using pair

// modes of play
enum class Player { None, X, O };

// convert play to char for printing
char playerToChar(Player p);
// Struct for details of winning
struct WinInfo {
    Player winner;                           // player who win
    std::string type;                        // "row", "col", "diag", "anti-diag", "none"
    int index;                               // no of row,col and diag
    std::vector<std::pair<int, int>> winCells; // places of winning
};
#endif 
