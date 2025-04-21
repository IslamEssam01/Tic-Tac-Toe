#include "game_history.h"
#include <iostream>
#include <chrono>
#include <thread>

// Simple function to print a game record
void printGameRecord(const GameHistory::GameRecord& game) {
    std::cout << "Game ID: " << game.id << std::endl;
    std::cout << "Game started at: ";
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
        std::cout << "Game in progress";
    } else if (game.winner_id.value() == -1) {
        std::cout << "Draw";
    } else if (game.winner_id.value() == -2) {
        std::cout << "AI";
    } else {
        std::cout << "Player " << game.winner_id.value();
    }
    std::cout << std::endl << std::endl;
}

int main() {
    // Initialize the game history
    GameHistory history("tictactoe.db");
    
    // Example 1: Initialize a game between two human players
    int player1_id = 1;
    int player2_id = 2;
    int game1_id = history.initializeGame(player1_id, player2_id);
    
    if (game1_id > 0) {
        std::cout << "Game initialized with ID: " << game1_id << std::endl;
        
        // Record moves as they happen
        history.recordMove(game1_id, 4); // X plays center
        history.recordMove(game1_id, 0); // O plays top-left
        history.recordMove(game1_id, 8); // X plays bottom-right
        history.recordMove(game1_id, 2); // O plays top-right
        history.recordMove(game1_id, 6); // X plays bottom-left (X wins)
        
        // Set winner as player1
        history.setWinner(game1_id, player1_id);
        
        // Print the final game state
        auto game1 = history.getGameById(game1_id);
        std::cout << "Game 1 final state:" << std::endl;
        printGameRecord(game1);
    }
    
    // Example 2: Initialize a game between a human player and AI
    int player3_id = 3;
    int game2_id = history.initializeGame(player3_id, std::nullopt); // Player vs AI
    
    if (game2_id > 0) {
        std::cout << "Game initialized with ID: " << game2_id << std::endl;
        
        // Record moves
        history.recordMove(game2_id, 0); // X plays top-left
        history.recordMove(game2_id, 4); // O (AI) plays center
        history.recordMove(game2_id, 1); // X plays top-middle
        history.recordMove(game2_id, 7); // O (AI) plays bottom-middle
        history.recordMove(game2_id, 6); // X plays bottom-left
        history.recordMove(game2_id, 3); // O (AI) plays middle-left (AI wins)
        
        // Set winner as AI
        history.setWinner(game2_id, -2);
        
        // Print the final game state
        auto game2 = history.getGameById(game2_id);
        std::cout << "Game 2 final state:" << std::endl;
        printGameRecord(game2);
    }
    
    // Example 3: Initialize a game that ends in a draw
    int player4_id = 4;
    int player5_id = 5;
    int game3_id = history.initializeGame(player4_id, player5_id);
    
    if (game3_id > 0) {
        std::cout << "Game initialized with ID: " << game3_id << std::endl;
        
        // Record moves for a draw game
        history.recordMove(game3_id, 4); // X plays center
        history.recordMove(game3_id, 0); // O plays top-left
        history.recordMove(game3_id, 8); // X plays bottom-right
        history.recordMove(game3_id, 2); // O plays top-right
        history.recordMove(game3_id, 6); // X plays bottom-left
        history.recordMove(game3_id, 3); // O plays middle-left
        history.recordMove(game3_id, 5); // X plays middle-right
        history.recordMove(game3_id, 1); // O plays top-middle
        history.recordMove(game3_id, 7); // X plays bottom-middle
        
        // Set as draw
        history.setWinner(game3_id, -1);
        
        // Print the final game state
        auto game3 = history.getGameById(game3_id);
        std::cout << "Game 3 final state:" << std::endl;
        printGameRecord(game3);
    }
    
    // Example 4: Game in progress
    int player6_id = 6;
    int player7_id = 7;
    int game4_id = history.initializeGame(player6_id, player7_id);
    
    if (game4_id > 0) {
        std::cout << "Game initialized with ID: " << game4_id << std::endl;
        
        // Record a few moves but don't set a winner
        history.recordMove(game4_id, 4); // X plays center
        history.recordMove(game4_id, 0); // O plays top-left
        history.recordMove(game4_id, 8); // X plays bottom-right
        
        // Print the in-progress game state
        auto game4 = history.getGameById(game4_id);
        std::cout << "Game 4 (in progress) state:" << std::endl;
        printGameRecord(game4);
        
        // Check if game is active
        std::cout << "Is game 4 active? " << (history.isGameActive(game4_id) ? "Yes" : "No") << std::endl;
    }
    
    // Retrieve and print all games
    std::cout << "\n--- All Games ---\n";
    auto allGames = history.getAllGames();
    for (const auto& g : allGames) {
        printGameRecord(g);
    }
    
    return 0;
}