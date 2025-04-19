#include <gtest/gtest.h>
#include "../src/game.h"

class GameTest : public ::testing::Test {
protected:
    Player playerX{"Player X"};
    Player playerO{"Player O"};
    
    void SetUp() override {
        // Initialize the players
        playerX = Player("Player X");
        playerO = Player("Player O");
    }
};

// Test game initialization
TEST_F(GameTest, InitializeGame) {
    Game game(playerX, playerO);
    
    // Check initial game state
    EXPECT_EQ(game.getState(), Game::GameState::InProgress);
    EXPECT_FALSE(game.isGameOver());
    EXPECT_EQ(game.getPlayerX().getName(), "Player X");
    EXPECT_EQ(game.getPlayerO().getName(), "Player O");
    
    // Check that the board is empty
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(game.getCell(i), Game::Cell::Empty);
    }
    
    // Check that there are no moves yet
    EXPECT_TRUE(game.getMoves().empty());
}

// Test making valid moves
TEST_F(GameTest, MakeValidMoves) {
    Game game(playerX, playerO);
    
    // Make two valid moves
    EXPECT_TRUE(game.makeMove(0)); // X in top-left
    EXPECT_TRUE(game.makeMove(4)); // O in center
    
    // Check the cells
    EXPECT_EQ(game.getCell(0), Game::Cell::X);
    EXPECT_EQ(game.getCell(4), Game::Cell::O);
    
    // Check that the moves were recorded
    EXPECT_EQ(game.getMoves().size(), 2);
    EXPECT_EQ(game.getMoves()[0].position, 0);
    EXPECT_EQ(game.getMoves()[1].position, 4);
}

// Test making invalid moves
TEST_F(GameTest, MakeInvalidMoves) {
    Game game(playerX, playerO);
    
    // Make a valid move
    EXPECT_TRUE(game.makeMove(0));
    
    // Try to place in the same spot
    EXPECT_FALSE(game.makeMove(0));
    
    // Try to place outside the board
    EXPECT_FALSE(game.makeMove(-1));
    EXPECT_FALSE(game.makeMove(9));
    
    // Check that only one move was recorded
    EXPECT_EQ(game.getMoves().size(), 1);
}

// Test X wins
TEST_F(GameTest, XWins) {
    Game game(playerX, playerO);
    
    // X wins with top row
    EXPECT_TRUE(game.makeMove(0)); // X
    EXPECT_TRUE(game.makeMove(3)); // O
    EXPECT_TRUE(game.makeMove(1)); // X
    EXPECT_TRUE(game.makeMove(4)); // O
    EXPECT_TRUE(game.makeMove(2)); // X
    
    // Check game state
    EXPECT_EQ(game.getState(), Game::GameState::XWins);
    EXPECT_TRUE(game.isGameOver());
    EXPECT_EQ(game.getWinner().value().getName(), "Player X");
}

// Test O wins
TEST_F(GameTest, OWins) {
    Game game(playerX, playerO);
    
    // O wins with middle column
    EXPECT_TRUE(game.makeMove(0)); // X
    EXPECT_TRUE(game.makeMove(1)); // O
    EXPECT_TRUE(game.makeMove(2)); // X
    EXPECT_TRUE(game.makeMove(4)); // O
    EXPECT_TRUE(game.makeMove(6)); // X
    EXPECT_TRUE(game.makeMove(7)); // O
    
    // Check game state
    EXPECT_EQ(game.getState(), Game::GameState::OWins);
    EXPECT_TRUE(game.isGameOver());
    EXPECT_EQ(game.getWinner().value().getName(), "Player O");
}

// Test draw
TEST_F(GameTest, Draw) {
    Game game(playerX, playerO);
    
    // Draw game
    EXPECT_TRUE(game.makeMove(0)); // X
    EXPECT_TRUE(game.makeMove(1)); // O
    EXPECT_TRUE(game.makeMove(2)); // X
    EXPECT_TRUE(game.makeMove(4)); // O
    EXPECT_TRUE(game.makeMove(3)); // X
    EXPECT_TRUE(game.makeMove(5)); // O
    EXPECT_TRUE(game.makeMove(7)); // X
    EXPECT_TRUE(game.makeMove(6)); // O
    EXPECT_TRUE(game.makeMove(8)); // X
    
    // Check game state
    EXPECT_EQ(game.getState(), Game::GameState::Draw);
    EXPECT_TRUE(game.isGameOver());
    EXPECT_FALSE(game.getWinner().has_value());
}

// Test moves to string conversion
TEST_F(GameTest, MovesToString) {
    Game game(playerX, playerO);
    
    // Make some moves
    game.makeMove(0);
    game.makeMove(4);
    game.makeMove(8);
    
    // Convert moves to string
    std::string movesStr = Game::movesToString(game.getMoves());
    EXPECT_EQ(movesStr, "0,4,8");
    
    // Convert back
    auto moves = Game::movesFromString(movesStr);
    EXPECT_EQ(moves.size(), 3);
    EXPECT_EQ(moves[0].position, 0);
    EXPECT_EQ(moves[1].position, 4);
    EXPECT_EQ(moves[2].position, 8);
}

// Test get winner ID
TEST_F(GameTest, GetWinnerId) {
    Game game(playerX, playerO);
    
    // Neither player has an ID yet
    EXPECT_EQ(Game::getWinnerID(game), 0);
    
    // Set player IDs
    playerX.setId(1);
    playerO.setId(2);
    
    Game gameWithIds(playerX, playerO);
    
    // X wins
    gameWithIds.makeMove(0);
    gameWithIds.makeMove(3);
    gameWithIds.makeMove(1);
    gameWithIds.makeMove(4);
    gameWithIds.makeMove(2);
    
    EXPECT_EQ(Game::getWinnerID(gameWithIds), 1);
    
    // Create a game with AI
    Player humanPlayer("Human");
    humanPlayer.setId(5);
    Player aiPlayer = Player::createAI();
    
    Game humanVsAI(humanPlayer, aiPlayer);
    
    // No winner yet
    EXPECT_EQ(Game::getWinnerID(humanVsAI), 0);
    
    // AI wins
    humanVsAI.makeMove(0);
    humanVsAI.makeMove(3);
    humanVsAI.makeMove(1);
    humanVsAI.makeMove(4);
    humanVsAI.makeMove(8);
    humanVsAI.makeMove(5);
    
    // AI is O and won, so winner should be None (0)
    EXPECT_EQ(Game::getWinnerID(humanVsAI), 0);
    
    // Test draw
    Game drawGame(playerX, playerO);
    
    // Force a draw
    drawGame.makeMove(0);
    drawGame.makeMove(1);
    drawGame.makeMove(2);
    drawGame.makeMove(4);
    drawGame.makeMove(3);
    drawGame.makeMove(5);
    drawGame.makeMove(7);
    drawGame.makeMove(6);
    drawGame.makeMove(8);
    
    EXPECT_EQ(Game::getWinnerID(drawGame), -1);
}