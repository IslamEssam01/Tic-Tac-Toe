#include "Board.h"
#include <iostream>
#include <limits>

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void printInstructions() {
    std::cout << "Welcome to Tic-Tac-Toe!\n";
    std::cout << "Players take turns entering row (0-2) and column (0-2) numbers.\n";
    std::cout << "Player X goes first.\n\n";
}

std::pair<int, int> getMove(Player currentPlayer) {
    int row, col;
    bool validInput = false;
    
    do {
        std::cout << "Player " << playerToChar(currentPlayer) << "'s turn.\n";
        std::cout << "Enter row (0-2): ";
        if (!(std::cin >> row)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        
        std::cout << "Enter column (0-2): ";
        if (!(std::cin >> col)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input. Please enter a number.\n";
            continue;
        }
        
        validInput = true;
    } while (!validInput);
    
    return {row, col};
}

int main() {
    Board board;
    Player currentPlayer = Player::X;
    
    clearScreen();
    printInstructions();
    
    while (!board.isGameOver()) {
        board.print();
        std::cout << "\n";
        
        auto [row, col] = getMove(currentPlayer);
        
        if (!board.makeMove(row, col, currentPlayer)) {
            std::cout << "Invalid move! Try again.\n";
            continue;
        }
        
        clearScreen();
        
        WinInfo winInfo = board.checkWinner();
        if (winInfo.winner != Player::None) {
            board.print();
            std::cout << "\nPlayer " << playerToChar(winInfo.winner) << " wins!\n";
            break;
        }
        
        if (board.isFull()) {
            board.print();
            std::cout << "\nIt's a draw!\n";
            break;
        }
        
        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    }
    
    return 0;
}
