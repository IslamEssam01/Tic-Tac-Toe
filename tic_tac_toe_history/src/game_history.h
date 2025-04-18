#ifndef GAME_HISTORY_H
#define GAME_HISTORY_H

#include <string>
#include <vector>

// Represents a move in the game
struct Move {
    int game_id;     
    char player;     // 'X' or 'O' player
    int position;    // Position of the move (1-9)
    int move_number; 
};

// Manages the game history and validates moves
class GameHistory {
public:
    GameHistory(); // Constructor: Initializes game state
    int startNewGame(); // Starts a new game, resets the board
    void addMove(int game_id, char player, int position); // Adds a move to the history
    std::vector<Move> getGameMoves(int game_id) const; // Retrieves moves for a specific game
    bool isValidMove(int position) const; // Checks if a position is available

private:
    std::vector<Move> moves_; // List of moves made in the game
    int current_game_id_;     // Current active game ID
    std::vector<int> board_;  // Tracks the positions on the board (1-9)
};

#endif // GAME_HISTORY_H
