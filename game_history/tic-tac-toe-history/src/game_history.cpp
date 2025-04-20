#include "game_history.h"
#include <iostream>
#include <sstream>

GameHistory::GameHistory(const std::string& dbPath) : dbPath(dbPath), db(nullptr) {}

GameHistory::~GameHistory() {
    if (db) {
        sqlite3_close(db);
    }
}

bool GameHistory::initialize() {
    int rc = sqlite3_open(dbPath.c_str(), &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }
    
    // Create games table if it doesn't exist
    std::string createTableQuery = 
        "CREATE TABLE IF NOT EXISTS games ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "moves TEXT NOT NULL,"  // Stored as comma-separated positions
        "player_x INTEGER,"     // NULL for AI
        "player_o INTEGER,"     // NULL for AI
        "winner INTEGER,"       // NULL for AI win, -1 for draw
        "timestamp INTEGER NOT NULL"
        ");";
    
    return executeQuery(createTableQuery);
}

bool GameHistory::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
    
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

int GameHistory::saveGame(
    const std::vector<int>& moves,
    const std::optional<int> playerX_ID,
    const std::optional<int> playerO_ID,
    const std::optional<int> winner_ID,
    time_t timestamp
) {
    if (!db) {
        std::cerr << "Database not initialized" << std::endl;
        return -1;
    }
    
    // Convert moves to comma-separated string
    std::stringstream movesStr;
    if (!moves.empty()) {
        movesStr << moves[0];
        for (size_t i = 1; i < moves.size(); ++i) {
            movesStr << "," << moves[i];
        }
    }
    
    // Prepare SQL statement
    std::string sql = "INSERT INTO games (moves, player_x, player_o, winner, timestamp) VALUES (?, ?, ?, ?, ?);";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }
    
    // Bind parameters
    sqlite3_bind_text(stmt, 1, movesStr.str().c_str(), -1, SQLITE_STATIC);
    
    if (playerX_ID.has_value()) {
        sqlite3_bind_int(stmt, 2, playerX_ID.value());
    } else {
        sqlite3_bind_null(stmt, 2);
    }
    
    if (playerO_ID.has_value()) {
        sqlite3_bind_int(stmt, 3, playerO_ID.value());
    } else {
        sqlite3_bind_null(stmt, 3);
    }
    
    if (winner_ID.has_value()) {
        sqlite3_bind_int(stmt, 4, winner_ID.value());
    } else {
        sqlite3_bind_null(stmt, 4);
    }
    
    sqlite3_bind_int64(stmt, 5, static_cast<sqlite3_int64>(timestamp));
    
    // Execute
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to insert data: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }
    
    return static_cast<int>(sqlite3_last_insert_rowid(db));
}

std::optional<int> GameHistory::getOptionalInt(sqlite3_stmt* stmt, int column) {
    if (sqlite3_column_type(stmt, column) == SQLITE_NULL) {
        return std::nullopt;
    }
    return sqlite3_column_int(stmt, column);
}

std::optional<GameRecord> GameHistory::getGame(int gameId) {
    if (!db) {
        std::cerr << "Database not initialized" << std::endl;
        return std::nullopt;
    }
    
    std::string sql = "SELECT id, moves, player_x, player_o, winner, timestamp FROM games WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, gameId);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return std::nullopt;
    }
    
    GameRecord record;
    record.id = sqlite3_column_int(stmt, 0);
    
    // Parse moves
    std::string movesStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    std::stringstream ss(movesStr);
    std::string item;
    while (std::getline(ss, item, ',')) {
        if (!item.empty()) {
            record.moves.push_back(std::stoi(item));
        }
    }
    
    record.playerX_ID = getOptionalInt(stmt, 2);
    record.playerO_ID = getOptionalInt(stmt, 3);
    record.winner_ID = getOptionalInt(stmt, 4);
    record.timestamp = static_cast<time_t>(sqlite3_column_int64(stmt, 5));
    
    sqlite3_finalize(stmt);
    
    return record;
}

std::vector<GameRecord> GameHistory::getAllGames() {
    std::vector<GameRecord> games;
    if (!db) {
        std::cerr << "Database not initialized" << std::endl;
        return games;
    }
    
    std::string sql = "SELECT id, moves, player_x, player_o, winner, timestamp FROM games ORDER BY timestamp DESC;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        GameRecord record;
        record.id = sqlite3_column_int(stmt, 0);
        
        // Parse moves
        std::string movesStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::stringstream ss(movesStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
            if (!item.empty()) {
                record.moves.push_back(std::stoi(item));
            }
        }
        
        record.playerX_ID = getOptionalInt(stmt, 2);
        record.playerO_ID = getOptionalInt(stmt, 3);
        record.winner_ID = getOptionalInt(stmt, 4);
        record.timestamp = static_cast<time_t>(sqlite3_column_int64(stmt, 5));
        
        games.push_back(record);
    }
    
    sqlite3_finalize(stmt);
    
    return games;
}

std::vector<GameRecord> GameHistory::getPlayerGames(int playerId) {
    std::vector<GameRecord> games;
    if (!db) {
        std::cerr << "Database not initialized" << std::endl;
        return games;
    }
    
    std::string sql = "SELECT id, moves, player_x, player_o, winner, timestamp FROM games "
                      "WHERE player_x = ? OR player_o = ? ORDER BY timestamp DESC;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }
    
    sqlite3_bind_int(stmt, 1, playerId);
    sqlite3_bind_int(stmt, 2, playerId);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        GameRecord record;
        record.id = sqlite3_column_int(stmt, 0);
        
        // Parse moves
        std::string movesStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::stringstream ss(movesStr);
        std::string item;
        while (std::getline(ss, item, ',')) {
            if (!item.empty()) {
                record.moves.push_back(std::stoi(item));
            }
        }
        
        record.playerX_ID = getOptionalInt(stmt, 2);
        record.playerO_ID = getOptionalInt(stmt, 3);
        record.winner_ID = getOptionalInt(stmt, 4);
        record.timestamp = static_cast<time_t>(sqlite3_column_int64(stmt, 5));
        
        games.push_back(record);
    }
    
    sqlite3_finalize(stmt);
    
    return games;
}

bool GameHistory::deleteGame(int gameId) {
    if (!db) {
        std::cerr << "Database not initialized" << std::endl;
        return false;
    }
    
    std::string sql = "DELETE FROM games WHERE id = ?;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    sqlite3_bind_int(stmt, 1, gameId);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}