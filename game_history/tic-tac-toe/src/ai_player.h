#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include "game.h"

class AIPlayer {
public:
    // Difficulty levels
    enum class Difficulty { Easy, Medium, Hard };
    
    AIPlayer(Difficulty level = Difficulty::Medium);
    
    // Get the AI's move for the current board state
    int getMove(const Game& game) const;
    
    // Set the difficulty level
    void setDifficulty(Difficulty level);
    
private:
    Difficulty difficulty;
    
    // AI move calculations based on difficulty
    int getRandomMove(const Game& game) const;
    int getBestMove(const Game& game) const;
    int evaluateBoard(const std::array<Game::Cell, 9>& board, Game::Cell player) const;
    int minimax(std::array<Game::Cell, 9> board, int depth, bool isMaximizing, Game::Cell player) const;
};

#endif // AI_PLAYER_H