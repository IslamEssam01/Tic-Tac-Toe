#ifndef GAME_HISTORY_H
#define GAME_HISTORY_H

#include <string>
#include <vector>
#include <optional>
#include <sqlite3.h>
#include <ctime>

/**
 * @brief Represents a move in the game
 */
struct Move {
    int position; // 0-8 representing the position on the board
    int player;   // 1 for X, 2 for O
};

/**
 * @brief Represents a complete game record
 */
struct GameRecord {
    int id; // Database ID
    std::vector<int> moves; // Positions of moves in sequence
    std::optional<int> playerX_ID; // ID of player X (std::nullopt for AI)
    std::optional<int> playerO_ID; // ID of player O (std::nullopt for AI)
    std::optional<int> winner_ID;  // Winner ID, -1 for draw, std::nullopt for AI win
    time_t timestamp;     // When the game was played
};

/**
 * @brief Class to manage game history using SQLite
 */
class GameHistory {
public:
    /**
     * @brief Construct a new Game History object
     * 
     * @param dbPath Path to the SQLite database file
     */
    GameHistory(const std::string& dbPath = "game_history.db");
    
    /**
     * @brief Destroy the Game History object
     */
    ~GameHistory();
    
    /**
     * @brief Initialize the database connection and create tables if needed
     * 
     * @return true if successful, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Save a game to the database
     * 
     * @param moves Vector of move positions in the sequence they were played
     * @param playerX_ID ID of player X, std::nullopt for AI
     * @param playerO_ID ID of player O, std::nullopt for AI
     * @param winner_ID ID of winner, -1 for draw, std::nullopt for AI win
     * @param timestamp Time when the game was played
     * @return int The ID of the inserted game, -1 if failed
     */
    int saveGame(
        const std::vector<int>& moves,
        const std::optional<int> playerX_ID,
        const std::optional<int> playerO_ID,
        const std::optional<int> winner_ID,
        time_t timestamp = std::time(nullptr)
    );
    
    /**
     * @brief Get a game record by ID
     * 
     * @param gameId ID of the game to retrieve
     * @return std::optional<GameRecord> The game record if found, std::nullopt otherwise
     */
    std::optional<GameRecord> getGame(int gameId);
    
    /**
     * @brief Get all games in the database
     * 
     * @return std::vector<GameRecord> Vector of all games
     */
    std::vector<GameRecord> getAllGames();
    
    /**
     * @brief Get games for a specific player
     * 
     * @param playerId ID of the player
     * @return std::vector<GameRecord> Vector of games played by the player
     */
    std::vector<GameRecord> getPlayerGames(int playerId);
    
    /**
     * @brief Delete a game from the database
     * 
     * @param gameId ID of the game to delete
     * @return true if successful, false otherwise
     */
    bool deleteGame(int gameId);
    
private:
    sqlite3* db;
    std::string dbPath;
    
    bool executeQuery(const std::string& query);
    std::optional<int> getOptionalInt(sqlite3_stmt* stmt, int column);
};

#endif // GAME_HISTORY_H