#include "ai_player.h"
#include <random>
#include <algorithm>
#include <vector>

AIPlayer::AIPlayer(Difficulty level) : difficulty(level) {}

void AIPlayer::setDifficulty(Difficulty level) {
    difficulty = level;
}

int AIPlayer::getMove(const Game& game) const {
    switch (difficulty) {
        case Difficulty::Easy:
            return getRandomMove(game);
        case Difficulty::Medium:
            // 50% chance of best move, 50% chance of random move
            if (std::rand() % 2 == 0) {
                return getBestMove(game);
            } else {
                return getRandomMove(game);
            }
        case Difficulty::Hard:
            return getBestMove(game);
        default:
            return getRandomMove(game);
    }
}

int AIPlayer::getRandomMove(const Game& game) const {
    std::vector<int> availableMoves;
    
    // Find all empty cells
    for (int i = 0; i < 9; ++i) {
        if (game.getCell(i) == Game::Cell::Empty) {
            availableMoves.push_back(i);
        }
    }
    
    // Return a random empty cell
    if (!availableMoves.empty()) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, availableMoves.size() - 1);
        return availableMoves[dis(gen)];
    }
    
    return -1; // No valid moves
}

int AIPlayer::getBestMove(const Game& game) const {
    const auto& board = game.getBoard();
    Game::Cell aiPlayer = game.getCurrentPlayer();
    
    int bestScore = -1000;
    int bestMove = -1;
    
    // Try each empty cell
    for (int i = 0; i < 9; ++i) {
        if (board[i] == Game::Cell::Empty) {
            std::array<Game::Cell, 9> boardCopy = board;
            boardCopy[i] = aiPlayer;
            
            int score = minimax(boardCopy, 0, false, aiPlayer);
            
            if (score > bestScore) {
                bestScore = score;
                bestMove = i;
            }
        }
    }
    
    return bestMove;
}

int AIPlayer::evaluateBoard(const std::array<Game::Cell, 9>& board, Game::Cell player) const {
    // Check rows, columns, and diagonals for a win
    Game::Cell opponent = (player