#ifndef GAME_HISTORY_H
#define GAME_HISTORY_H

#include <string>
#include <vector>
#include <sqlite3.h>
#include <optional>
#include <chrono>
#include <QObject>

class GameHistory : public QObject {
    Q_OBJECT
public:
    // Constructor and destructor
    GameHistory(const std::string& db_path, QObject* parent = nullptr);
    ~GameHistory();

    // Initialize database
    bool initializeDatabase();

    // Struct to represent a move in the game
    struct Move {
        int position;  // Position on the board (0-8)
    };

    // Struct to represent a game record
    struct GameRecord {
        int id = 0;    // Game ID from database
        std::vector<Move> moves;
        std::optional<int> playerX_id;  // If AI plays X, this will be std::nullopt
        std::optional<int> playerO_id;  // If AI plays O, this will be std::nullopt
        std::optional<int> winner_id;   // -1 for draw, -2 for AI win, positive number for player win, std::nullopt if game not finished
        std::chrono::system_clock::time_point timestamp;
    };

    // Initialize a new game with player IDs and return game ID
    int initializeGame(std::optional<int> playerX_id, std::optional<int> playerO_id);
    
    // Record a move and save the game state
    bool recordMove(int game_id, int position);
    
    // Set the winner of a game (-1 for draw, -2 for AI win, positive for player win)
    bool setWinner(int game_id, std::optional<int> winner_id);
    
    // Check if a game is still active (no winner set)
    bool isGameActive(int game_id);
    
    // Check if a game exists in the database
    bool gameExists(int game_id);
    
    // Username mapping methods
    void registerPlayerUsername(int playerId, const std::string& username);
    std::string getPlayerUsername(int playerId);
    
    // Get a game by its ID
    GameRecord getGameById(int game_id);
    
    // Update an existing game
    bool updateGame(int game_id, const GameRecord& game);

    // Save a game to the history
    bool saveGame(const GameRecord& game);

    // Retrieve game history for a player
    std::vector<GameRecord> getPlayerGames(int player_id);

    // Retrieve all games
    std::vector<GameRecord> getAllGames();

    // Retrieve latest games (limit specifies how many)
    std::vector<GameRecord> getLatestGames(int limit);

signals:
    // Signals emitted when game events occur
    void gameInitialized(int gameId);
    void moveRecorded(int gameId, int position);
    void gameCompleted(int gameId, std::optional<int> winnerId);

private:
    std::string db_path;
    sqlite3* db;

    // Helper functions for database operations
    bool executeQuery(const std::string& query);
    bool tableExists(const std::string& table_name);

    // Helper functions for serialization and deserialization
    std::string serializeMoves(const std::vector<Move>& moves);
    std::vector<Move> deserializeMoves(const std::string& serialized_moves);
};

#endif // GAME_HISTORY_H