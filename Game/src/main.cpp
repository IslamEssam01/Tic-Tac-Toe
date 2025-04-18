#include "Board.h"
#include "AI.h"
#include <iostream>
#include <string>
#include <limits>

// Helper function to clear input buffer
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Helper function to get valid player move
std::pair<int, int> getPlayerMove(const Board& board) {
    int row, col;
    while (true) {
        std::cout << "Enter your move (row[0-2] col[0-2]): ";
        if (std::cin >> row >> col) {
            if (row >= 0 && row < 3 && col >= 0 && col < 3) {
                if (board.isCellEmpty(row, col)) {
                    return {row, col};
                } else {
                    std::cout << "That cell is already occupied!\n";
                }
            } else {
                std::cout << "Invalid input! Row and column must be between 0 and 2.\n";
            }
        } else {
            std::cout << "Invalid input! Please enter two numbers.\n";
            clearInputBuffer();
        }
    }
}

int main() {
    Board board;
    char playerChoice;
    Player humanPlayer, aiPlayer;
    
    // Get player's choice of X or O
    while (true) {
        std::cout << "Do you want to play as X or O? (x/o): ";
        std::cin >> playerChoice;
        playerChoice = std::tolower(playerChoice);
        
        if (playerChoice == 'x') {
            humanPlayer = Player::X;
            aiPlayer = Player::O;
            break;
        } else if (playerChoice == 'o') {
            humanPlayer = Player::O;
            aiPlayer = Player::X;
            break;
        } else {
            std::cout << "Invalid choice! Please enter 'x' or 'o'.\n";
            clearInputBuffer();
        }
    }

    // Game loop
    Player currentPlayer = Player::X;  // X always goes first
    std::cout << "\nGame starting! Use row (0-2) and column (0-2) to make your move.\n\n";
    
    while (!board.isGameOver()) {
        // Print current board state
        std::cout << "\nCurrent board:\n";
        board.print();
        std::cout << "\n";

        std::pair<int, int> move;
        
        if (currentPlayer == humanPlayer) {
            // Human's turn
            std::cout << "Your turn (Player " << playerToChar(humanPlayer) << ")!\n";
            move = getPlayerMove(board);
        } else {
            // AI's turn
            std::cout << "AI's turn (Player " << playerToChar(aiPlayer) << ")...\n";
            move = findBestMove(board, aiPlayer);
        }
        
        board.makeMove(move.first, move.second, currentPlayer);
        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    }

    // Game over - print final state
    std::cout << "\nFinal board:\n";
    board.print();
    std::cout << "\n";

    // Print result
    WinInfo result = board.checkWinner();
    if (result.winner != Player::None) {
        if (result.winner == humanPlayer) {
            std::cout << "Congratulations! You won!\n";
        } else {
            std::cout << "AI wins! Better luck next time!\n";
        }
    } else {
        std::cout << "It's a draw!\n";
    }

    return 0;
}
