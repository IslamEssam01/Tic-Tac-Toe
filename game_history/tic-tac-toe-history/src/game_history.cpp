#include "game_history.h"
#include <iostream>
#include <sstream>
#include <iomanip>

GameHistory::GameHistory(const std::string& db_path) : db_path(db_path), db(nullptr) {
    // Initialize the database when the object is created
    initializeDatabase();
}

GameHistory::~GameHistory() {
    // Close the database connection when the object is destroyed
    if (db) {
        sqlite3_close(db);
    }
}

bool GameHistory::initializeDatabase() {
    // Open the database connection
    int rc = sqlite3_open(db_path.c_str(), &db);
    if (rc) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        db = nullptr;
        return false;
    }

    // Create the games table if it doesn't exist
    std::string create_table_query = 
        "CREATE TABLE IF NOT EXISTS games ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "moves TEXT NOT NULL, "  // Serialized list of moves
        "player_x INTEGER, "     // NULL if AI
        "player_o INTEGER, "     // NULL if AI
        "winner INTEGER, "       // -1 for draw, -2 for AI win, positive number for player win, NULL if game not finished
        "timestamp TEXT NOT NULL"
        ");";

    return executeQuery(create_table_query);
}

bool GameHistory::executeQuery(const std::string& query) {
    char* error_message = nullptr;
    int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &error_message);

    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << error_message << std::endl;
        sqlite3_free(error_message);
        return false;
    }
    return true;
}

bool GameHistory::tableExists(const std::string& table_name) {
    std::string query = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + table_name + "';";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        return false;
    }
    
    rc = sqlite3_step(stmt);
    bool exists = (rc == SQLITE_ROW);
    sqlite3_finalize(stmt);
    
    return exists;
}

std::string GameHistory::serializeMoves(const std::vector<Move>& moves) {
    if (moves.empty()) {
        return ""; // Return empty string for empty moves list
    }
    std::stringstream ss;
    for (size_t i = 0; i < moves.size(); ++i) {
        ss << moves[i].position;
        if (i < moves.size() - 1) {
            ss << ",";
        }
    }
    return ss.str();
}

std::vector<GameHistory::Move> GameHistory::deserializeMoves(const std::string& serialized_moves) {
    std::vector<Move> moves;
    if (serialized_moves.empty()) {
        return moves; // Return empty vector
    }
    std::stringstream ss(serialized_moves);
    std::string position_str;
    
    while (std::getline(ss, position_str, ',')) {
        Move move;
        move.position = std::stoi(position_str);
        moves.push_back(move);
    }
    
    return moves;
}

int GameHistory::initializeGame(std::optional<int> playerX_id, std::optional<int> playerO_id) {
    // Create a new game record with the given player IDs
    GameRecord game;
    game.playerX_id = playerX_id;
    game.playerO_id = playerO_id;
    game.winner_id = std::nullopt; // Game is ongoing
    game.moves.clear(); // Start with empty moves
    game.timestamp = std::chrono::system_clock::now(); // Record start time
    
    // Save the initial game state to get an ID
    if (!saveGame(game)) {
        return -1; // Return -1 if saving failed
    }
    
    // Get the ID of the last inserted game
    sqlite3_stmt* stmt;
    std::string query = "SELECT last_insert_rowid();";
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return -1;
    }
    
    rc = sqlite3_step(stmt);
    int game_id = -1;
    
    if (rc == SQLITE_ROW) {
        game_id = sqlite3_column_int(stmt, 0);
    }
    
    sqlite3_finalize(stmt);
    return game_id;
}

bool GameHistory::recordMove(int game_id, int position) {
    // Get the current game state
    GameRecord game = getGameById(game_id);
    
    // Add the new move
    Move move;
    move.position = position;
    game.moves.push_back(move);
    
    // Update the game in the database
    return updateGame(game_id, game);
}

bool GameHistory::setWinner(int game_id, std::optional<int> winner_id) {
    // Get the current game state
    GameRecord game = getGameById(game_id);
    
    // Update the winner
    game.winner_id = winner_id;
    
    // Update the game in the database
    return updateGame(game_id, game);
}

GameHistory::GameRecord GameHistory::getGameById(int game_id) {
    GameRecord game;
    game.id = game_id; // Set the ID directly to match the requested ID
    
    std::string query = "SELECT moves, player_x, player_o, winner, timestamp FROM games WHERE id = " + std::to_string(game_id) + ";";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return game;
    }
    
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        // Get moves
        std::string moves_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        game.moves = deserializeMoves(moves_str);
        
        // Get player X
        if (sqlite3_column_type(stmt, 1) != SQLITE_NULL) {
            game.playerX_id = sqlite3_column_int(stmt, 1);
        }
        
        // Get player O
        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) {
            game.playerO_id = sqlite3_column_int(stmt, 2);
        }
        
        // Get winner
        if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
            game.winner_id = sqlite3_column_int(stmt, 3);
        }
        
        // Get timestamp
        std::string timestamp_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
        std::tm tm = {};
        std::istringstream ss(timestamp_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        game.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }
    
    sqlite3_finalize(stmt);
    return game;
}

bool GameHistory::updateGame(int game_id, const GameRecord& game) {
    // Convert timestamp to string
    auto time_t = std::chrono::system_clock::to_time_t(game.timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    std::string timestamp_str = ss.str();

    // Serialize moves
    std::string serialized_moves = serializeMoves(game.moves);

    // Prepare the SQL statement
    std::stringstream query;
    query << "UPDATE games SET ";
    
    // Update moves
    query << "moves = '" << serialized_moves << "', ";
    
    // Update player X (NULL if AI)
    query << "player_x = ";
    if (game.playerX_id.has_value()) {
        query << game.playerX_id.value();
    } else {
        query << "NULL";
    }
    query << ", ";
    
    // Update player O (NULL if AI)
    query << "player_o = ";
    if (game.playerO_id.has_value()) {
        query << game.playerO_id.value();
    } else {
        query << "NULL";
    }
    query << ", ";
    
    // Update winner (-1 for draw, -2 for AI win, positive number for player win, NULL if game not finished)
    query << "winner = ";
    if (game.winner_id.has_value()) {
        query << game.winner_id.value();
    } else {
        query << "NULL";
    }
    
    // Add WHERE clause
    query << " WHERE id = " << game_id << ";";
    
    return executeQuery(query.str());
}

bool GameHistory::saveGame(const GameRecord& game) {
    // Convert timestamp to string
    auto time_t = std::chrono::system_clock::to_time_t(game.timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    std::string timestamp_str = ss.str();

    // Serialize moves
    std::string serialized_moves = serializeMoves(game.moves);

    // Prepare the SQL statement
    std::stringstream query;
    query << "INSERT INTO games (moves, player_x, player_o, winner, timestamp) VALUES (";
    
    // Add moves
    query << "'" << serialized_moves << "', ";
    
    // Add player X (NULL if AI)
    if (game.playerX_id.has_value()) {
        query << game.playerX_id.value();
    } else {
        query << "NULL";
    }
    query << ", ";
    
    // Add player O (NULL if AI)
    if (game.playerO_id.has_value()) {
        query << game.playerO_id.value();
    } else {
        query << "NULL";
    }
    query << ", ";
    
    // Add winner (-1 for draw, -2 for AI win, positive number for player win, NULL if game not finished)
    if (game.winner_id.has_value()) {
        query << game.winner_id.value();
    } else {
        query << "NULL";
    }
    query << ", ";
    
    // Add timestamp
    query << "'" << timestamp_str << "'";
    
    query << ");";
    
    return executeQuery(query.str());
}

std::vector<GameHistory::GameRecord> GameHistory::getPlayerGames(int player_id) {
    std::vector<GameRecord> games;
    std::string query = "SELECT id, moves, player_x, player_o, winner, timestamp FROM games "
                       "WHERE player_x = " + std::to_string(player_id) + 
                       " OR player_o = " + std::to_string(player_id) + 
                       " ORDER BY timestamp DESC;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        GameRecord game;
        
        // Get game ID
        game.id = sqlite3_column_int(stmt, 0);
        
        // Get moves
        std::string moves_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        game.moves = deserializeMoves(moves_str);
        
        // Get player X
        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) {
            game.playerX_id = sqlite3_column_int(stmt, 2);
        }
        
        // Get player O
        if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
            game.playerO_id = sqlite3_column_int(stmt, 3);
        }
        
        // Get winner
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
            game.winner_id = sqlite3_column_int(stmt, 4);
        }
        
        // Get timestamp
        std::string timestamp_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        std::tm tm = {};
        std::istringstream ss(timestamp_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        game.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        games.push_back(game);
    }
    
    sqlite3_finalize(stmt);
    return games;
}

std::vector<GameHistory::GameRecord> GameHistory::getAllGames() {
    std::vector<GameRecord> games;
    std::string query = "SELECT id, moves, player_x, player_o, winner, timestamp FROM games ORDER BY id DESC;";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        GameRecord game;
        
        // Get game ID
        game.id = sqlite3_column_int(stmt, 0);
        
        // Get moves
        std::string moves_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        game.moves = deserializeMoves(moves_str);
        
        // Get player X
        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) {
            game.playerX_id = sqlite3_column_int(stmt, 2);
        }
        
        // Get player O
        if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
            game.playerO_id = sqlite3_column_int(stmt, 3);
        }
        
        // Get winner
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
            game.winner_id = sqlite3_column_int(stmt, 4);
        }
        
        // Get timestamp
        std::string timestamp_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        std::tm tm = {};
        std::istringstream ss(timestamp_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        game.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        games.push_back(game);
    }
    
    sqlite3_finalize(stmt);
    return games;
}

std::vector<GameHistory::GameRecord> GameHistory::getLatestGames(int limit) {
    std::vector<GameRecord> games;
    std::string query = "SELECT id, moves, player_x, player_o, winner, timestamp FROM games ORDER BY id DESC LIMIT " + std::to_string(limit) + ";";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    
    if (rc != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return games;
    }
    
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        GameRecord game;
        
        // Get game ID
        game.id = sqlite3_column_int(stmt, 0);
        
        // Get moves
        std::string moves_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        game.moves = deserializeMoves(moves_str);
        
        // Get player X
        if (sqlite3_column_type(stmt, 2) != SQLITE_NULL) {
            game.playerX_id = sqlite3_column_int(stmt, 2);
        }
        
        // Get player O
        if (sqlite3_column_type(stmt, 3) != SQLITE_NULL) {
            game.playerO_id = sqlite3_column_int(stmt, 3);
        }
        
        // Get winner
        if (sqlite3_column_type(stmt, 4) != SQLITE_NULL) {
            game.winner_id = sqlite3_column_int(stmt, 4);
        }
        
        // Get timestamp
        std::string timestamp_str = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5));
        std::tm tm = {};
        std::istringstream ss(timestamp_str);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
        game.timestamp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
        
        games.push_back(game);
    }
    
    sqlite3_finalize(stmt);
    return games;
}

bool GameHistory::isGameActive(int game_id) {
    GameRecord game = getGameById(game_id);
    return !game.winner_id.has_value(); // Game is active if winner_id is null
}