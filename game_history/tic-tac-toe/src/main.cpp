#include <iostream>
#include <string>
#include <vector>
#include "game.h"
#include "player.h"
#include "database.h"
#include "ai_player.h"

// Function prototypes
void displayWelcome();
void displayBoard(const Game& game);
void playGame(Database& db);
void viewGameHistory(Database& db);
void displayGameDetails(const Game& game);
Player createPlayer(Database& db);
Player selectPlayer(Database& db);

int main() {
    Database db;
    
    if (!db.connect()) {
        std::cerr << "Failed to connect to database." << std::endl;
        return 1;
    }
    
    displayWelcome();
    
    bool quit = false;
    
    while (!quit) {
        std::cout << "\n=== MAIN MENU ===" << std::endl;
        std::cout << "1. Play New Game" << std::endl;
        std::cout << "2. View Game History" << std::endl;
        std::cout << "3. Quit" << std::endl;
        std::cout << "Choice: ";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                playGame(db);
                break;
            case 2:
                viewGameHistory(db);
                break;
            case 3:
                quit = true;
                break;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
    }
    
    db.disconnect();
    std::cout << "Thank you for playing!" << std::endl;
    
    return 0;
}

void displayWelcome() {
    std::cout << "=======================================" << std::endl;
    std::cout << "   Welcome to Tic-Tac-Toe with History" << std::endl;
    std::cout << "=======================================" << std::endl;
    std::cout << "Play games and track your history!" << std::endl;
}

void displayBoard(const Game& game) {
    std::cout << "\n";
    std::cout << " " << cellToChar(game.getCell(0)) << " | " << cellToChar(game.getCell(1)) << " | " << cellToChar(game.getCell(2)) << std::endl;
    std::cout << "---+---+---" << std::endl;
    std::cout << " " << cellToChar(game.getCell(3)) << " | " << cellToChar(game.getCell(4)) << " | " << cellToChar(game.getCell(5)) << std::endl;
    std::cout << "---+---+---" << std::endl;
    std::cout << " " << cellToChar(game.getCell(6)) << " | " << cellToChar(game.getCell(7)) << " | " << cellToChar(game.getCell(8)) << std::endl;
    std::cout << "\n";
}

char cellToChar(Game::Cell cell) {
    switch (cell) {
        case Game::Cell::X: return 'X';
        case Game::Cell::O: return 'O';
        default: return ' ';
    }
}

void playGame(Database& db) {
    std::cout << "\n=== NEW GAME ===" << std::endl;
    std::cout << "1. Player vs Player" << std::endl;
    std::cout << "2. Player vs AI" << std::endl;
    std::cout << "Choice: ";
    
    int gameType;
    std::cin >> gameType;
    
    Player playerX;
    Player playerO;
    
    if (gameType == 1) {
        std::cout << "\nPlayer X:" << std::endl;
        playerX = selectPlayer(db);
        
        std::cout << "\nPlayer O:" << std::endl;
        playerO = selectPlayer(db);
    } else {
        std::cout << "\nWho will play as X?" << std::endl;
        std::cout << "1. Player" << std::endl;
        std::cout << "2. AI" << std::endl;
        std::cout << "Choice: ";
        
        int playerChoice;
        std::cin >> playerChoice;
        
        if (playerChoice == 1) {
            playerX = selectPlayer(db);
            playerO = Player::createAI();
        } else {
            playerX = Player::createAI();
            playerO = selectPlayer(db);
        }
        
        if (playerX.isAI() || playerO.isAI()) {
            std::cout << "\nSelect AI difficulty:" << std::endl;
            std::cout << "1. Easy" << std::endl;
            std::cout << "2. Medium" << std::endl;
            std::cout << "3. Hard" << std::endl;
            std::cout << "Choice: ";
            
            int difficultyChoice;
            std::cin >> difficultyChoice;
            
            AIPlayer::Difficulty aiDifficulty;
            switch (difficultyChoice) {
                case 1: aiDifficulty = AIPlayer::Difficulty::Easy; break;
                case 2: aiDifficulty = AIPlayer::Difficulty::Medium; break;
                case 3: aiDifficulty = AIPlayer::Difficulty::Hard; break;
                default: aiDifficulty = AIPlayer::Difficulty::Medium;
            }
            
            AIPlayer ai(aiDifficulty);
            // Set up the AI for later use
        }
    }
    
    Game game(playerX, playerO);
    AIPlayer ai;
    
    while (!game.isGameOver()) {
        displayBoard(game);
        
        Game::Cell currentPlayer = game.getCurrentPlayer();
        const Player& player = (currentPlayer == Game::Cell::X) ? playerX : playerO;
        
        std::cout << "Player " << (currentPlayer == Game::Cell::X ? "X" : "O") << "'s turn" << std::endl;
        
        int move;
        
        if (player.isAI()) {
            move = ai.getMove(game);
            std::cout << "AI placed at position " << move << std::endl;
        } else {
            std::cout << "Enter position (0-8): ";
            std::cin >> move;
        }
        
        if (move < 0 || move > 8 || !game.makeMove(move)) {
            if (!player.isAI()) {
                std::cout << "Invalid move! Try again." << std::endl;
            }
            continue;
        }
    }
    
    displayBoard(game);
    
    switch (game.getState()) {
        case Game::GameState::XWins:
            std::cout << "Player X wins!" << std::endl;
            break;
        case Game::GameState::OWins:
            std::cout << "Player O wins!" << std::endl;
            break;
        case Game::GameState::Draw:
            std::cout << "It's a draw!" << std::endl;
            break;
        default:
            break;
    }
    
    // Save game to database
    if (db.saveGame(game)) {
        std::cout << "Game saved to history!" << std::endl;
    } else {
        std::cout << "Failed to save game to history." << std::endl;
    }
}

void viewGameHistory(Database& db) {
    std::cout << "\n=== GAME HISTORY ===" << std::endl;
    std::cout << "1. All Games" << std::endl;
    std::cout << "2. Games by Player" << std::endl;
    std::cout << "Choice: ";
    
    int choice;
    std::cin >> choice;
    
    std::vector<Game> games;
    
    if (choice == 1) {
        games = db.getGameHistory();
    } else {
        // Get player ID
        Player player = selectPlayer(db);
        if (player.getId().has_value()) {
            games = db.getPlayerGames(player.getId().value());
        } else {
            std::cout << "Invalid player selection." << std::endl;
            return;
        }
    }
    
    if (games.empty()) {
        std::cout << "No games found in history." << std::endl;
        return;
    }
    
    std::cout << "\nFound " << games.size() << " games:" << std::endl;
    
    for (size_t i = 0; i < games.size(); ++i) {
        std::cout << i + 1 << ". ";
        displayGameSummary(games[i]);
    }
    
    std::cout << "\nEnter game number to view details (0 to return): ";
    int gameNum;
    std::cin >> gameNum;
    
    if (gameNum > 0 && gameNum <= static_cast<int>(games.size())) {
        displayGameDetails(games[gameNum - 1]);
    }
}

void displayGameSummary(const Game& game) {
    // Display a short summary of the game
    std::string playerX = game.getPlayerX().isAI() ? "AI" : game.getPlayerX().getName();
    std::string playerO = game.getPlayerO().isAI() ? "AI" : game.getPlayerO().getName();
    
    std::cout << playerX << " (X) vs " << playerO << " (O) - ";
    
    switch (game.getState()) {
        case Game::GameState::XWins:
            std::cout << playerX << " won";
            break;
        case Game::GameState::OWins:
            std::cout << playerO << " won";
            break;
        case Game::GameState::Draw:
            std::cout << "Draw";
            break;
        default:
            std::cout << "Incomplete";
    }
    
    std::cout << " - " << game.getMoves().size() << " moves" << std::endl;
}

void displayGameDetails(const Game& game) {
    std::cout << "\n=== GAME DETAILS ===" << std::endl;
    
    std::string playerX = game.getPlayerX().isAI() ? "AI" : game.getPlayerX().getName();
    std::string playerO = game.getPlayerO().isAI() ? "AI" : game.getPlayerO().getName();
    
    std::cout << "Player X: " << playerX << std::endl;
    std::cout << "Player O: " << playerO << std::endl;
    
    switch (game.getState()) {
        case Game::GameState::XWins:
            std::cout << "Winner: " << playerX << " (X)" << std::endl;
            break;
        case Game::GameState::OWins:
            std::cout << "Winner: " << playerO << " (O)" << std::endl;
            break;
        case Game::GameState::Draw:
            std::cout << "Result: Draw" << std::endl;
            break;
        default:
            std::cout << "Result: Incomplete" << std::endl;
    }
    
    std::cout << "Moves: " << std::endl;
    
    // Display the entire game
    Game replayGame(game.getPlayerX(), game.getPlayerO());
    displayBoard(replayGame);
    
    for (const auto& move : game.getMoves()) {
        char player = replayGame.getCurrentPlayer() == Game::Cell::X ? 'X' : 'O';
        replayGame.makeMove(move.position);
        
        std::cout << "Move " << (&move - &game.getMoves()[0] + 1) << ": Player " << player 
                  << " placed at position " << move.position << std::endl;
        displayBoard(replayGame);
    }
}

Player createPlayer(Database& db) {
    std::cout << "Enter player name: ";
    std::string name;
    std::cin.ignore(); // Clear input buffer
    std::getline(std::cin, name);
    
    Player player(name);
    auto playerId = db.addPlayer(player);
    
    if (playerId.has_value()) {
        player.setId(playerId.value());
        std::cout << "Player created with ID: " << playerId.value() << std::endl;
    } else {
        std::cout << "Failed to create player in database." << std::endl;
    }
    
    return player;
}

Player selectPlayer(Database& db) {
    // Get list of players
    auto players = db.getAllPlayers();
    
    std::cout << "Select player:" << std::endl;
    std::cout << "0. Create new player" << std::endl;
    
    for (size_t i = 0; i < players.size(); ++i) {
        std::cout << i + 1 << ". " << players[i].getName() << std::endl;
    }
    
    std::cout << "Choice: ";
    int choice;
    std::cin >> choice;
    
    if (choice == 0) {
        return createPlayer(db);
    } else if (choice > 0 && choice <= static_cast<int>(players.size())) {
        return players[choice - 1];
    }
    
    // Default to a new player
    std::cout << "Invalid choice. Creating new player." << std::endl;
    return createPlayer(db);
}