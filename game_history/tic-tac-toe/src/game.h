#ifndef GAME_H
#define GAME_H

#include <array>
#include <vector>
#include <chrono>
#include <optional>
#include "player.h"

class Game {
public:
    enum class Cell { Empty, X, O };
    enum class GameState { InProgress, XWins, OWins, Draw };
    
    // Represents a move in the game
    struct Move {
        int position; // 0-8 for the 3x3 grid
        Cell player;  // X or O
    };
    
private:
    std::array<Cell, 9> board;
    Player playerX;
    Player playerO;
    std::vector<Move> moves;
    std::optional<Player> winner;
    GameState state;
    std::chrono::system_clock::time_point gameTime;
    
public:
    Game(const Player& x_player, const Player& o_player);
    
    // Game logic
    bool makeMove(int position);
    GameState getState() const;
    std::optional<Player> getWinner() const;
    bool isGameOver() const;
    Cell getCurrentPlayer() const;
    
    // Board access
    Cell getCell(int position) const;
    const std::array<Cell, 9>& getBoard() const;
    
    // Game data access
    const std::vector<Move>& getMoves() const;
    const Player& getPlayerX() const;
    const Player& getPlayerO() const;
    std::chrono::system_clock::time_point getGameTime() const;
    
    // Utility functions
    static int getWinnerID(const Game& game);
    static std::string movesToString(const std::vector<Move>& moves);
    static std::vector<Move> movesFromString(const std::string& movesStr);

private:
    void updateGameState();
    bool checkWinCondition(Cell player) const;
    bool checkDraw() const;
};

#endif // GAME_H