#include "AI.h"
#include "globals.h"
#include <limits>
#include <algorithm>

// Helper function to get the opponent
Player otherPlayer(Player p) {
    return (p == Player::X) ? Player::O : Player::X;
}

// Minimax with Alpha-Beta Pruning
int minimax(Board board, Player currentPlayer, Player aiPlayer, int alpha, int beta, int depth) {
    // Base case: game is over
    if (board.isGameOver()) {
        WinInfo winInfo = board.checkWinner();
        if (winInfo.winner == aiPlayer) return 10 - depth;      // AI wins (prefer faster wins)
        else if (winInfo.winner != Player::None) return -10 + depth; // Opponent wins (prefer slower losses)
        else return 0;                                          // Draw
    }

    if (currentPlayer == aiPlayer) {
        // Maximizing player (AI)
        for (int i = 0; i < 9; i++) {
            int row = i / 3;
            int col = i % 3;
            if (board.isCellEmpty(row, col)) {
                Board newBoard = board;  // Copy board
                newBoard.makeMove(row, col, currentPlayer);
                int eval = minimax(newBoard, otherPlayer(currentPlayer), aiPlayer, alpha, beta, depth + 1);
                alpha = std::max(alpha, eval);
                if (beta <= alpha) break;  // Beta cut-off
            }
        }
        return alpha;
    } else {
        // Minimizing player (opponent)
        for (int i = 0; i < 9; i++) {
            int row = i / 3;
            int col = i % 3;
            if (board.isCellEmpty(row, col)) {
                Board newBoard = board;  // Copy board
                newBoard.makeMove(row, col, currentPlayer);
                int eval = minimax(newBoard, otherPlayer(currentPlayer), aiPlayer, alpha, beta, depth + 1);
                beta = std::min(beta, eval);
                if (beta <= alpha) break;  // Alpha cut-off
            }
        }
        return beta;
    }
}

// Find the best move for the AI
std::pair<int, int> findBestMove(const Board& board, Player aiPlayer) {
    // If board is empty, take center
    bool isEmpty = true;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (!board.isCellEmpty(i, j)) {
                isEmpty = false;
                break;
            }
        }
        if (!isEmpty) break;
    }
    if (isEmpty) {
        return {1, 1};  // Return center position
    }

    int bestScore = std::numeric_limits<int>::min();
    std::pair<int, int> bestMove = {-1, -1};
    
    // Check for immediate winning move first
    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;
        if (board.isCellEmpty(row, col)) {
            Board newBoard = board;
            newBoard.makeMove(row, col, aiPlayer);
            if (newBoard.checkWinner().winner == aiPlayer) {
                return {row, col};  // Return immediate winning move
            }
        }
    }

    // If no immediate win, perform minimax search
    for (int i = 0; i < 9; i++) {
        int row = i / 3;
        int col = i % 3;
        if (board.isCellEmpty(row, col)) {
            Board newBoard = board;
            newBoard.makeMove(row, col, aiPlayer);
            int score = minimax(newBoard, otherPlayer(aiPlayer), aiPlayer,
                               std::numeric_limits<int>::min(), // Start alpha very small (-infinity)
                               std::numeric_limits<int>::max(), // Start beta very large (infinity)
                               0);  // Start depth at 0
            if (score > bestScore) {
                bestScore = score;
                bestMove = {row, col};
            }
        }
    }
    return bestMove;
}
