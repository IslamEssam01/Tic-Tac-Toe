#include "game_history.h"
#include <iostream>
#include <chrono>

// Simple function to print a game record
void printGameRecord(const GameHistory::GameRecord& game) {
    std::cout << "Game played at: ";
    auto time_t = std::chrono::system_clock::to_time_t(game.timestamp);
    std::cout << std::ctime(&time_t);
    
    std::cout << "Moves: ";
    for (const auto& move : game.moves) {
        std::cout << move.position << " ";
    }
    std::cout << std::endl;
    
    std::cout << "Player X: ";
    if (game.playerX_id.has_value()) {
        std::cout << game.playerX_id.value();
    } else {
        std::cout << "AI";
    }
    std::cout << std::endl;
    
    std::cout << "Player O: ";
    if (game.playerO_id.has_value()) {
        std::cout << game.playerO_id.value();
    } else {
        std::cout << "AI";
    }
    std::cout << std::endl;
    
    std::cout << "Winner: ";
    if (!game.winner_id.has_value()) {
        std::cout << "AI or Game not finished";
    } else if (game.winner_id.value() == -1) {
        std::cout << "Draw";
    } else {
        std::cout << "Player " << game.winner_id.value();
    }
    std::cout << std::endl << std::endl;
}

int main() {
    // Initialize the game history
    GameHistory history("tictactoe.db");
    
    // Create a sample game record
    GameHistory::GameRecord game;
    
    // Add some moves (example: X plays center, O plays top-left, X plays bottom-right, etc.)
    game.moves.push_back({4}); // Center
    game.moves.push_back({0}); // Top-left
    game.moves.push_back({8}); // Bottom-right
    game.moves.push_back({2}); // Top-right
    game.moves.push_back({6}); // Bottom-left
    
    // Set player IDs (example: player 1 vs player 2)
    game.playerX_id = 1;
    game.playerO_id = 2;
    
    // Set winner (example: player 1 wins)
    game.winner_id = 1;
    
    // Set timestamp to current time
    game.timestamp = std::chrono::system_clock::now();
    
    // Save the game to history
    if (history.saveGame(game)) {
        std::cout << "Game saved successfully!" << std::endl;
    } else {
        std::cerr << "Failed to save game." << std::endl;
        return 1;
    }
    
    // Create another sample game with AI
    GameHistory::GameRecord aiGame;
    
    // Add some moves
    aiGame.moves.push_back({0}); // Top-left
    aiGame.moves.push_back({4}); // Center
    aiGame.moves.push_back({1}); // Top-middle
    aiGame.moves.push_back({7}); // Bottom-middle
    aiGame.moves.push_back({2}); // Top-right
    
    // Set player IDs (example: player 1 vs AI)
    aiGame.playerX_id = 1;
    aiGame.playerO_id = std::nullopt; // AI plays O
    
    // AI wins
    aiGame.winner_id = std::nullopt;
    
    // Set timestamp to current time
    aiGame.timestamp = std::chrono::system_clock::now();
    
    // Save the AI game to history
    if (history.saveGame(aiGame)) {
        std::cout << "AI game saved successfully!" << std::endl;
    } else {
        std::cerr << "Failed to save AI game." << std::endl;
        return 1;
    }
    
    // Create a draw game
    GameHistory::GameRecord drawGame;
    
    // Add moves for a draw game
    drawGame.moves.push_back({4}); // Center
    drawGame.moves.push_back({0}); // Top-left
    drawGame.moves.push_back({8}); // Bottom-right
    drawGame.moves.push_back({2}); // Top-right
    drawGame.moves.push_back({6}); // Bottom-left
    drawGame.moves.push_back({3}); // Middle-left
    drawGame.moves.push_back({5}); // Middle-right
    drawGame.moves.push_back({1}); // Top-middle
    drawGame.moves.push_back({7}); // Bottom-middle
    
    // Set player IDs
    drawGame.playerX_id = 3;
    drawGame.playerO_id = 4;
    
    // Set as draw
    drawGame.winner_id = -1;
    
    // Set timestamp
    drawGame.timestamp = std::chrono::system_clock::now();
    
    // Save the draw game
    if (history.saveGame(drawGame)) {
        std::cout << "Draw game saved successfully!" << std::endl;
    } else {
        std::cerr << "Failed to save draw game." << std::endl;
        return 1;
    }
    
    // Retrieve and print all games
    std::cout << "\n--- All Games ---\n";
    auto allGames = history.getAllGames();
    for (const auto& g : allGames) {
        printGameRecord(g);
    }
    
    // Retrieve and print games for player 1
    std::cout << "\n--- Player 1's Games ---\n";
    auto playerGames = history.getPlayerGames(1);
    for (const auto& g : playerGames) {
        printGameRecord(g);
    }
    
    // Retrieve and print the latest game
    std::cout << "\n--- Latest Game ---\n";
    auto latestGames = history.getLatestGames(1);
    if (!latestGames.empty()) {
        printGameRecord(latestGames[0]);
    }
    
    return 0;
}