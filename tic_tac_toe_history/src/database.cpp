#include "database.h"
#include <sqlite3.h>
#include <stdexcept>

// Constructor: opens the database and creates tables
Database::Database(const std::string& db_name) {
    if (sqlite3_open(db_name.c_str(), reinterpret_cast<sqlite3**>(&db_)) != SQLITE_OK) {
        throw std::runtime_error("Cannot open database: " + std::string(sqlite3_errmsg(static_cast<sqlite3*>(db_))));
    }
    createTables(); // Create tables if not exist
}

// Destructor: closes the database
Database::~Database() {
    sqlite3_close(static_cast<sqlite3*>(db_));
}

// Creates the 'moves' table if not exists
void Database::createTables() {
    const char* sql = "CREATE TABLE IF NOT EXISTS moves ("
                      "game_id INTEGER, "
                      "player TEXT, "
                      "position INTEGER, "
                      "move_number INTEGER, "
                      "PRIMARY KEY (game_id, move_number));";
    char* err_msg = nullptr;
    if (sqlite3_exec(static_cast<sqlite3*>(db_), sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        std::string error = err_msg;
        sqlite3_free(err_msg);
        throw std::runtime_error("Failed to create table: " + error);
    }
}

// Saves a move to the database
void Database::saveMove(const Move& move) {
    sqlite3_stmt* stmt;
    const char* sql = "INSERT INTO moves (game_id, player, position, move_number) VALUES (?, ?, ?, ?);";
    
    if (sqlite3_prepare_v2(static_cast<sqlite3*>(db_), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(static_cast<sqlite3*>(db_))));
    }

    sqlite3_bind_int(stmt, 1, move.game_id);  // Bind game ID
    sqlite3_bind_text(stmt, 2, std::string(1, move.player).c_str(), -1, SQLITE_STATIC); // Bind player
    sqlite3_bind_int(stmt, 3, move.position); // Bind position
    sqlite3_bind_int(stmt, 4, move.move_number); // Bind move number

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Failed to execute statement: " + std::string(sqlite3_errmsg(static_cast<sqlite3*>(db_))));
    }
    sqlite3_finalize(stmt); // Clean up
}

// Retrieves all moves for a specific game
std::vector<Move> Database::getGameMoves(int game_id) const {
    std::vector<Move> moves;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT game_id, player, position, move_number FROM moves WHERE game_id = ? ORDER BY move_number;";

    if (sqlite3_prepare_v2(static_cast<sqlite3*>(db_), sql, -1, &stmt, nullptr) != SQLITE_OK) {
        throw std::runtime_error("Failed to prepare statement: " + std::string(sqlite3_errmsg(static_cast<sqlite3*>(db_))));
    }

    sqlite3_bind_int(stmt, 1, game_id); // Bind game ID

    // Retrieve moves from the database
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Move move;
        move.game_id = sqlite3_column_int(stmt, 0);   // Game ID
        move.player = *reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)); // Player
        move.position = sqlite3_column_int(stmt, 2);  // Position
        move.move_number = sqlite3_column_int(stmt, 3); // Move number
        moves.push_back(move); // Add move to the list
    }
    
    sqlite3_finalize(stmt); // Clean up
    return moves; // Return list of moves
}
