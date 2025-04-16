#ifndef GAME_HISTORY_H
#define GAME_HISTORY_H

#include <string>
#include <vector>

struct Move {
    int game_id;
    char player; // 'X' or 'O'
    int position; // 1-9
    int move_number;
};

class GameHistory {
public:
    GameHistory();
    int startNewGame();
    void addMove(int game_id, char player, int position);
    std::vector<Move> getGameMoves(int game_id) const;
    bool isValidMove(int position) const;

private:
    std::vector<Move> moves_;
    int current_game_id_;
    std::vector<int> board_; // Tracks occupied positions (1-9)
};

#endif // GAME_HISTORY_H