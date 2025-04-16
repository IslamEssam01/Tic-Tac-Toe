#include "game_history.h"
#include <stdexcept>

GameHistory::GameHistory() : current_game_id_(0), board_(9, 0) {}

int GameHistory::startNewGame() {
    ++current_game_id_;
    board_.assign(9, 0); // Reset board
    return current_game_id_;
}

void GameHistory::addMove(int game_id, char player, int position) {
    if (game_id != current_game_id_) {
        throw std::runtime_error("Invalid game ID");
    }
    if (player != 'X' && player != 'O') {
        throw std::runtime_error("Invalid player");
    }
    if (position < 1 || position > 9) {
        throw std::runtime_error("Invalid position");
    }
    if (!isValidMove(position)) {
        throw std::runtime_error("Position already taken");
    }

    Move move;
    move.game_id = game_id;
    move.player = player;
    move.position = position;
    move.move_number = moves_.size() + 1;
    moves_.push_back(move);
    board_[position - 1] = 1; // Mark position as taken
}

std::vector<Move> GameHistory::getGameMoves(int game_id) const {
    std::vector<Move> game_moves;
    for (const auto& move : moves_) {
        if (move.game_id == game_id) {
            game_moves.push_back(move);
        }
    }
    return game_moves;
}

bool GameHistory::isValidMove(int position) const {
    return board_[position - 1] == 0;
}