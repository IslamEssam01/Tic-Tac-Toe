#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "game_history.h"

// Database class for handling game moves storage
class Database {
public:
    Database(const std::string& db_name); // Constructor: opens the database
    ~Database(); // Destructor: closes the database
    void saveMove(const Move& move); // Saves a move to the database
    std::vector<Move> getGameMoves(int game_id) const; // Retrieves moves for a game

private:
    void createTables(); // Creates the necessary tables
    void* db_; // SQLite database handle
};

#endif // DATABASE_H
