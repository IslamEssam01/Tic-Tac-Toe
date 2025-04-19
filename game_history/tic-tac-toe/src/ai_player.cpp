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
    Game::Cell opponent = (player == Game::Cell::X) ? Game::Cell::O : Game::Cell::X;
    
    // Check rows
    for (int i = 0; i < 9; i += 3) {
        if (board[i] == player && board[i + 1] == player && board[i + 2] == player) {
            return 10;
        }
        if (board[i] == opponent && board[i + 1] == opponent && board[i + 2] == opponent) {
            return -10;
        }
    }
    
    // Check columns
    for (int i = 0; i < 3; ++i) {
        if (board[i] == player && board[i + 3] == player && board[i + 6] == player) {
            return 10;
        }
        if (board[i] == opponent && board[i + 3] == opponent && board[i + 6] == opponent) {
            return -10;
        }
    }
    
    // Check diagonals
    if (board[0] == player && board[4] == player && board[8] == player) {
        return 10;
    }
    if (board[0] == opponent && board[4] == opponent && board[8] == opponent) {
        return -10;
    }
    
    if (board[2] == player && board[4] == player && board[6] == player) {
        return 10;
    }
    if (board[2] == opponent && board[4] == opponent && board[6] == opponent) {
        return -10;
    }
    
    return 0; // No winner yet
}

int AIPlayer::minimax(std::array<Game::Cell, 9> board, int depth, bool isMaximizing, Game::Cell player) const {
    // Terminal conditions
    int score = evaluateBoard(board, player);
    
    if (score == 10) return score - depth;  // Win, prefer faster wins
    if (score == -10) return score + depth; // Loss, prefer slower losses
    
    // Check if the board is full (draw)
    bool isFull = true;
    for (int i = 0; i < 9; ++i) {
        if (board[i] == Game::Cell::Empty) {
            isFull = false;
            break;
        }
    }
    
    if (isFull) return 0;
    
    Game::Cell opponent = (player == Game::Cell::X) ? Game::Cell::O : Game::Cell::X;
    
    // Maximizing player's turn (AI)
    if (isMaximizing) {
        int bestScore = -1000;
        
        for (int i = 0; i < 9; ++i) {
            if (board[i] == Game::Cell::Empty) {
                board[i] = player;
                bestScore = std::max(bestScore, minimax(board, depth + 1, false, player));
                board[i] = Game::Cell::Empty; // Undo move
            }
        }
        
        return bestScore;
    } 
    // Minimizing player's turn (opponent)
    else {
        int bestScore = 1000;
        
        for (int i = 0; i < 9; ++i) {
            if (board[i] == Game::Cell::Empty) {
                board[i] = opponent;
                bestScore = std::min(bestScore, minimax(board, depth + 1, true, player));
                board[i] = Game::Cell::Empty; // Undo move
            }
        }
        
        return bestScore;
    }
}