#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "game_history.h"

class Database {
public:
    Database(const std::string& db_name);
    ~Database();
    void saveMove(const Move& move);
    std::vector<Move> getGameMoves(int game_id) const;

private:
    void createTables();
    void* db_; // SQLite database handle
};

#endif // DATABASE_H