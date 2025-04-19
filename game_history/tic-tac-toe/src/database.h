#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <optional>
#include "game.h"
#include "player.h"

class Database {
private:
    sqlite3* db;
    bool isConnected;

public:
    Database();
    ~Database();
    
    // Connection management
    bool connect(const std::string& dbFile = "tictactoe.db");
    void disconnect();
    bool isOpen() const;
    
    // Schema management
    bool initializeSchema();
    
    // Player operations
    std::optional<int> addPlayer(const Player& player);
    std::optional<Player> getPlayer(int id);
    std::vector<Player> getAllPlayers();
    
    // Game operations
    bool saveGame(const Game& game);
    std::vector<Game> getGameHistory();
    std::vector<Game> getPlayerGames(int playerId);
    
    // Helper methods
    static int callback(void* data, int argc, char** argv, char** azColName);
};

#endif // DATABASE_H