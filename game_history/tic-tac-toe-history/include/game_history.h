#ifndef GAME_HISTORY_H
#define GAME_HISTORY_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include <optional>
#include <chrono>

class GameHistory {
public:
    // Constructor and destructor
    GameHistory(const std::string& db_path);
    ~GameHistory();

    // Initialize database
    bool initializeDatabase();

    // Struct to represent a move in the game
    struct Move {
        int position;  // Position on the board (0-8)
    };

    // Struct to represent a game record
    struct GameRecord {
        std::vector<Move> moves;
        std::optional<int> playerX_id;  // If AI plays X, this will be std::nullopt
        std::optional<int> playerO_id;  // If AI plays O, this will be std::nullopt
        std::optional<int> winner_id;   // -1 for draw, std::nullopt if AI wins or game not finished
        std::chrono::system_clock::time_point timestamp;
    };

    // Save a game to the history
    bool saveGame(const GameRecord& game);

    // Retrieve game history for a player
    std::vector<GameRecord> getPlayerGames(int player_id);

    // Retrieve all games
    std::vector<GameRecord> getAllGames();

    // Retrieve latest games (limit specifies how many)
    std::vector<GameRecord> getLatestGames(int limit);

private:
    sqlite3* db;
    std::string db_path;

    // Helper functions for database operations
    bool executeQuery(const std::string& query);
    bool tableExists(const std::string& table_name);

    // Helper functions for serialization and deserialization
    std::string serializeMoves(const std::vector<Move>& moves);
    std::vector<Move> deserializeMoves(const std::string& serialized_moves);
};

#endif // GAME_HISTORY_H