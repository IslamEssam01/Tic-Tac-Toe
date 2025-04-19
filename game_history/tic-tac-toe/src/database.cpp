#include "database.h"
#include <iostream>
#include <ctime>

Database::Database() : db(nullptr), isConnected(false) {}

Database::~Database() {
    disconnect();
}

bool Database::connect(const std::string& dbFile) {
    if (isConnected) {
        return true;
    }
    
    int rc = sqlite3_open(dbFile.c_str(), &db);
    if (rc) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return false;
    }
    
    isConnected = true;
    return initializeSchema();
}

void Database::disconnect() {
    if (isConnected) {
        sqlite3_close(db);
        db = nullptr;
        isConnected = false;
    }
}

bool Database::isOpen() const {
    return isConnected;
}

bool Database::initializeSchema() {
    if (!isConnected) {
        return false;
    }
    
    const char* playersTable = R"(
        CREATE TABLE IF NOT EXISTS players (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL
        );
    )";
    
    const char* gamesTable = R"(
        CREATE TABLE IF NOT EXISTS games (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            moves TEXT NOT NULL,
            player_x_id INTEGER,
            player_o_id INTEGER,
            winner_id INTEGER,
            game_time DATETIME,
            FOREIGN KEY (player_x_id) REFERENCES players (id),
            FOREIGN KEY (player_o_id) REFERENCES players (id)
        );
    )";
    
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, playersTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    rc = sqlite3_exec(db, gamesTable, nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    
    return true;
}

std::optional<int> Database::addPlayer(const Player& player) {
    if (!isConnected || player.isAI()) {
        return std::nullopt;
    }
    
    const char* sql = "INSERT INTO players (name) VALUES (?);";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_text(stmt, 1, player.getName().c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        std::cerr << "Failed to execute statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return std::nullopt;
    }
    
    int newId = sqlite3_last_insert_rowid(db);
    sqlite3_finalize(stmt);
    
    return newId;
}

std::optional<Player> Database::getPlayer(int id) {
    if (!isConnected) {
        return std::nullopt;
    }
    
    const char* sql = "SELECT id, name FROM players WHERE id = ?;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return std::nullopt;
    }
    
    sqlite3_bind_int(stmt, 1, id);
    
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        sqlite3_finalize(stmt);
        return std::nullopt;
    }
    
    int playerId = sqlite3_column_int(stmt, 0);
    std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    
    sqlite3_finalize(stmt);
    
    return Player(playerId, name);
}

std::vector<Player> Database::getAllPlayers() {
    std::vector<Player> players;
    
    if (!isConnected) {
        return players;
    }
    
    const char* sql = "SELECT id, name FROM players;";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return players;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        players.emplace_back(id, name);
    }
    
    sqlite3_finalize(stmt);
    return players;
}

bool Database::saveGame(const Game& game) {
    if (!isConnected) {
        return false;
    }
    
    const char* sql = "INSERT INTO games (moves, player_x_id, player_o_id, winner_id, game_time) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }
    
    // Convert moves to string
    std::string movesStr = Game::movesToString(game.getMoves());
    
    // Bind moves
    sqlite3_bind_text(stmt, 1, movesStr.c_str(), -1, SQLITE_STATIC);
    
    // Bind player X ID (either the ID or NULL if AI)
    const Player& playerX = game.getPlayerX();
    if (!playerX.isAI() && playerX.getId().has_value()) {
        sqlite3_bind_int(stmt, 2, playerX.getId().value());
    } else {
        sqlite3_bind_null(stmt, 2);
    }
    
    // Bind player O ID (either the ID or NULL if AI)
    const Player& playerO = game.getPlayerO();
    if (!playerO.isAI() && playerO.getId().has_value()) {
        sqlite3_bind_int(stmt, 3, playerO.getId().value());
    } else {
        sqlite3_bind_null(stmt, 3);
    }
    
    // Bind winner ID (winner ID, -1 for draw, or NULL for AI win/no winner)
    int winnerId = Game::getWinnerID(game);
    if (winnerId > 0) {
        sqlite3_bind_int(stmt, 4, winnerId);
    } else if (winnerId == -1) {
        sqlite3_bind_int(stmt, 4, -1);  // Draw
    } else {
        sqlite3_bind_null(stmt, 4);  // AI win or no winner
    }
    
    // Bind game time
    auto gameTime = game.getGameTime();
    std::time_t time = std::chrono::system_clock::to_time_t(gameTime);
    std::string timeStr = std::ctime(&time);
    timeStr.pop_back();  // Remove trailing newline
    sqlite3_bind_text(stmt, 5, timeStr.c_str(), -1, SQLITE_STATIC);
    
    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    return rc == SQLITE_DONE;
}

std::vector<Game> Database::getGameHistory() {
    std::vector<Game> games;
    
    if (!isConnected) {
        return games;
    }
    
    const char* sql = R"(
        SELECT g.moves, xp.id, xp.name, op.id, op.name, g.winner_id, g.game_time
        FROM games g
        LEFT JOIN players xp ON g.player_x_id = xp.id
        LEFT JOIN players op ON g.player_o_id = op.id
        ORDER BY g.game_time DESC;
    )";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::string movesStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        
        // Create players
        Player playerX = sqlite3_column_type(stmt, 1) == SQLITE_NULL ? 
                         Player::createAI() : 
                         Player(sqlite3_column_int(stmt, 1), 
                                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
                                
        Player playerO = sqlite3_column_type(stmt, 3) == SQLITE_NULL ? 
                         Player::createAI() : 
                         Player(sqlite3_column_int(stmt, 3), 
                                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        
        // Create game
        Game game(playerX, playerO);
        
        // Replay moves
        auto moves = Game::movesFromString(movesStr);
        for (const auto& move : moves) {
            game.makeMove(move.position);
        }
        
        games.push_back(game);
    }
    
    sqlite3_finalize(stmt);
    return games;
}

std::vector<Game> Database::getPlayerGames(int playerId) {
    std::vector<Game> games;
    
    if (!isConnected) {
        return games;
    }
    
    const char* sql = R"(
        SELECT g.moves, xp.id, xp.name, op.id, op.name, g.winner_id, g.game_time
        FROM games g
        LEFT JOIN players xp ON g.player_x_id = xp.id
        LEFT JOIN players op ON g.player_o_id = op.id
        WHERE g.player_x_id = ? OR g.player_o_id = ?
        ORDER BY g.game_time DESC;
    )";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }
    
    sqlite3_bind_int(stmt, 1, playerId);
    sqlite3_bind_int(stmt, 2, playerId);
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        std::string movesStr = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        
        // Create players
        Player playerX = sqlite3_column_type(stmt, 1) == SQLITE_NULL ? 
                         Player::createAI() : 
                         Player(sqlite3_column_int(stmt, 1), 
                                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
                                
        Player playerO = sqlite3_column_type(stmt, 3) == SQLITE_NULL ? 
                         Player::createAI() : 
                         Player(sqlite3_column_int(stmt, 3), 
                                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        
        // Create game
        Game game(playerX, playerO);
        
        // Replay moves
        auto moves = Game::movesFromString(movesStr);
        for (const auto& move : moves) {
            game.makeMove(move.position);
        }
        
        games.push_back(game);
    }
    
    sqlite3_finalize(stmt);
    return games;
}

int Database::callback(void* data, int argc, char** argv, char** azColName) {
    // Generic callback for SQLite operations
    for (int i = 0; i < argc; i++) {
        std::cout << azColName[i] << " = " << (argv[i] ? argv[i] : "NULL") << std::endl;
    }
    std::cout << std::endl;
    return 0;
}