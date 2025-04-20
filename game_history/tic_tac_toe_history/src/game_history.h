#ifndef GAME_HISTORY_H
#define GAME_HISTORY_H

#include <string>
#include <vector>
#include "database.h"

class GameHistory {
public:
    GameHistory(const std::string& db_path);
    int add_player(const std::string& name); // Added method
    void add_game(int player_x_id, int player_o_id, int winner_id, const std::vector<int>& moves);
    std::vector<std::string> get_history();

private:
    Database db_;
    std::string get_current_time();
};

#endif