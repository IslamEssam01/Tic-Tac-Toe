#include "src/game_history.h"
#include <iostream>
#include <iomanip>
#include <ctime>

// Helper function to print game information
void printGameInfo(const GameRecord& game) {
    std::cout << "Game ID: " << game.id << std::endl;
    
    std::cout << "Moves: ";
    for (size_t i = 0; i < game.moves.size(); ++i) {
        std::cout << game.moves[i];
        if (i < game.moves.size() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << std::endl;
    
    std::cout << "Player X: ";
    if (game.playerX_ID.has_value()) {
        std::cout << game.playerX_ID.value();
    } else {
        std::cout << "AI";
    }
    std::cout << std::endl;
    
    std::cout << "Player O: ";
    if (game.playerO_ID.has_value()) {
        std::cout << game.playerO_ID.value();
    } else {
        std::cout << "AI";
    }
    std::cout << std::endl;
    
    std::cout << "Winner: ";
    if (!game.winner_ID.has_value()) {
        std::cout << "AI";
    } else if (game.winner_ID.value() == -1) {
        std::cout << "Draw";
    } else {
        std::cout << game.winner_ID.value();
    }
    std::cout << std::endl;
    
    std::cout << "Time: " << std::put_time(std::localtime(&game.timestamp), "%Y-%m-%d %H:%M:%S") << std::endl;
    std::cout << "-----------------------------" << std::endl;
}

int main() {
    // Create a game history database
    GameHistory history("game_history.db");
    
    // Initialize the database
    if (!history.initialize()) {
        std::cerr << "Failed to initialize database" << std::endl;
        return 1;
    }
    
    // Example: Save a game where Player 1 (X) vs Player 2 (O), Player 1 wins
    std::vector<int> moves = {0, 4, 1, 5, 2}; // X wins with top row
    int gameId = history.saveGame(moves, 1, 2, 1);
    std::cout << "Saved game with ID: " << gameId << std::endl;
    
    // Example: Save a game where Player 3 (X) vs AI (O), Draw
    std::vector<int> moves2 = {0, 1, 2, 3, 5, 4, 6, 7, 8}; // Draw game
    int gameId2 = history.saveGame(moves2, 3, std::nullopt, -1);
    std::cout << "Saved game with ID: " << gameId2 << std::endl;
    
    // Example: Save a game where AI (X) vs Player 4 (O), AI wins
    std::vector<int> moves3 = {0, 3, 1, 4, 2}; // X (AI) wins with top row
    int gameId3 = history.saveGame(moves3, std::nullopt, 4, std::nullopt);
    std::cout << "Saved game with ID: " << gameId3 << std::endl;
    
    // Retrieve and print a specific game
    std::cout << "\nRetrieving game " << gameId << ":" << std::endl;
    auto game = history.getGame(gameId);
    if (game) {
        printGameInfo(*game);
    } else {
        std::cout << "Game not found" << std::endl;
    }
    
    // Print all games
    std::cout << "\nAll games:" << std::endl;
    auto allGames = history.getAllGames();
    for (const auto& g : allGames) {
        printGameInfo(g);
    }
    
    // Print games for a specific player
    std::cout << "\nGames for player 1:" << std::endl;
    auto playerGames = history.getPlayerGames(1);
    for (const auto& g : playerGames) {
        printGameInfo(g);
    }
    
    // Delete a game
    std::cout << "\nDeleting game " << gameId3 << std::endl;
    if (history.deleteGame(gameId3)) {
        std::cout << "Game deleted successfully" << std::endl;
    } else {
        std::cout << "Failed to delete game" << std::endl;
    }
    
    // Print all games after deletion
    std::cout << "\nAll games after deletion:" << std::endl;
    allGames = history.getAllGames();
    for (const auto& g : allGames) {
        printGameInfo(g);
    }
    
    return 0;
}