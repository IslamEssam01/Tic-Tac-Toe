#include "game.h"
#include <sstream>

Game::Game(const Player& x_player, const Player& o_player)
    : playerX(x_player), playerO(o_player), state(GameState::InProgress),
      gameTime(std::chrono::system_clock::now()) {
    // Initialize board
    board.fill(Cell::Empty);
}

bool Game::makeMove(int position) {
    // Check if the move is valid
    if (position < 0 || position > 8 || board[position] != Cell::Empty || isGameOver()) {
        return false;
    }
    
    // Determine current player
    Cell currentPlayer = moves.size() % 2 == 0 ? Cell::X : Cell::O;
    
    // Make the move
    board[position] = currentPlayer;
    moves.push_back({position, currentPlayer});
    
    // Update game state
    updateGameState();
    
    return true;
}

void Game::updateGameState() {
    // Check for win
    if (checkWinCondition(Cell::X)) {
        state = GameState::XWins;
        winner = playerX;
        return;
    }
    
    if (checkWinCondition(Cell::O)) {
        state = GameState::OWins;
        winner = playerO;
        return;
    }
    
    // Check for draw
    if (checkDraw()) {
        state = GameState::Draw;
        return;
    }
    
    // Game is still in progress
    state = GameState::InProgress;
}

bool Game::checkWinCondition(Cell player) const {
    // Check rows
    for (int i = 0; i < 9; i += 3) {
        if (board[i] == player && board[i + 1] == player && board[i + 2] == player) {
            return true;
        }
    }
    
    // Check columns
    for (int i = 0; i < 3; ++i) {
        if (board[i] == player && board[i + 3] == player && board[i + 6] == player) {
            return true;
        }
    }
    
    // Check diagonals
    if (board[0] == player && board[4] == player && board[8] == player) {
        return true;
    }
    
    if (board[2] == player && board[4] == player && board[6] == player) {
        return true;
    }
    
    return false;
}

bool Game::checkDraw() const {
    // If all cells are filled and no one has won, it's a draw
    for (const auto& cell : board) {
        if (cell == Cell::Empty) {
            return false;
        }
    }
    return true;
}

Game::GameState Game::getState() const {
    return state;
}

std::optional<Player> Game::getWinner() const {
    return winner;
}

bool Game::isGameOver() const {
    return state != GameState::InProgress;
}

Game::Cell Game::getCurrentPlayer() const {
    return moves.size() % 2 == 0 ? Cell::X : Cell::O;
}

Game::Cell Game::getCell(int position) const {
    if (position >= 0 && position < 9) {
        return board[position];
    }
    return Cell::Empty;
}

const std::array<Game::Cell, 9>& Game::getBoard() const {
    return board;
}

const std::vector<Game::Move>& Game::getMoves() const {
    return moves;
}

const Player& Game::getPlayerX() const {
    return playerX;
}

const Player& Game::getPlayerO() const {
    return playerO;
}

std::chrono::system_clock::time_point Game::getGameTime() const {
    return gameTime;
}

int Game::getWinnerID(const Game& game) {
    if (game.getState() == GameState::Draw) {
        return -1;  // Draw
    }
    
    auto winner = game.getWinner();
    if (!winner) {
        return 0;  // No winner yet
    }
    
    if (winner->isAI()) {
        return 0;  // AI winner (None)
    }
    
    auto id = winner->getId();
    return id.value_or(0);  // Player ID or 0 if not set
}

std::string Game::movesToString(const std::vector<Move>& moves) {
    std::stringstream ss;
    for (size_t i = 0; i < moves.size(); ++i) {
        ss << moves[i].position;
        if (i < moves.size() - 1) {
            ss << ",";
        }
    }
    return ss.str();
}

std::vector<Game::Move> Game::movesFromString(const std::string& movesStr) {
    std::vector<Move> moves;
    std::stringstream ss(movesStr);
    std::string item;
    
    bool isX = true;  // X always starts
    
    while (std::getline(ss, item, ',')) {
        int position = std::stoi(item);
        moves.push_back({position, isX ? Cell::X : Cell::O});
        isX = !isX;  // Alternate between X and O
    }
    
    return moves;
}