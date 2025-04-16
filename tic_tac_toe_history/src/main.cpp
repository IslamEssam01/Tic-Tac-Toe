#include "game_history.h"
#include "database.h"
#include <iostream>

int main() {
    try {
        Database db("tic_tac_toe.db");
        GameHistory game;

        // Start a new game
        int game_id = game.startNewGame();
        std::cout << "Started game with ID: " << game_id << std::endl;

        // Simulate some moves
        game.addMove(game_id, 'X', 1);
        db.saveMove({game_id, 'X', 1, 1});
        game.addMove(game_id, 'O', 2);
        db.saveMove({game_id, 'O', 2, 2});
        game.addMove(game_id, 'X', 3);
        db.saveMove({game_id, 'X', 3, 3});

        // Retrieve and display moves
        auto moves = db.getGameMoves(game_id);
        std::cout << "Moves for game " << game_id << ":\n";
        for (const auto& move : moves) {
            std::cout << "Move " << move.move_number << ": Player " << move.player
                      << " at position " << move.position << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}