#include <gtest/gtest.h>
#include <filesystem>
#include "game_history.h"

class GameHistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use a temporary test database
        db_path = "test_tictactoe.db";
        
        // Remove the database file if it exists
        if (std::filesystem::exists(db_path)) {
            std::filesystem::remove(db_path);
        }
        
        // Create a new GameHistory instance
        history = new GameHistory(db_path);
    }

    void TearDown() override {
        // Clean up
        delete history;
        
        // Remove the test database
        if (std::filesystem::exists(db_path)) {
            std::filesystem::remove(db_path);
        }
    }

    std::string db_path;
    GameHistory* history;
};

// Test database initialization
TEST_F(GameHistoryTest, InitializeDatabase) {
    EXPECT_TRUE(history->initializeDatabase());
}

// Test saving and retrieving a game
TEST_F(GameHistoryTest, SaveAndRetrieveGame) {
    // Create a test game
    GameHistory::GameRecord game;
    game.moves = {{0}, {4}, {1}, {7}, {2}}; // X wins with top row
    game.playerX_id = 1;
    game.playerO_id = 2;
    game.winner_id = 1;
    game.timestamp = std::chrono::system_clock::now();
    
    // Save the game
    EXPECT_TRUE(history->saveGame(game));
    
    // Retrieve all games
    auto games = history->getAllGames();
    EXPECT_EQ(games.size(), 1);
    
    // Check the retrieved game details
    const auto& retrieved_game = games[0];
    EXPECT_EQ(retrieved_game.moves.size(), game.moves.size());
    for (size_t i = 0; i < game.moves.size(); ++i) {
        EXPECT_EQ(retrieved_game.moves[i].position, game.moves[i].position);
    }
    EXPECT_EQ(retrieved_game.playerX_id.value(), game.playerX_id.value());
    EXPECT_EQ(retrieved_game.playerO_id.value(), game.playerO_id.value());
    EXPECT_EQ(retrieved_game.winner_id.value(), game.winner_id.value());
}
TEST_F(GameHistoryTest, EmptyMoves) {
    // Create a test game with no moves (e.g., game started but no moves made)
    GameHistory::GameRecord game;
    game.moves = {}; // Empty moves list
    game.playerX_id = 1;
    game.playerO_id = 2;
    game.winner_id = std::nullopt; // No winner yet
    game.timestamp = std::chrono::system_clock::now();
    
    // Save the game
    EXPECT_TRUE(history->saveGame(game));
    
    // Retrieve all games
    auto games = history->getAllGames();
    EXPECT_EQ(games.size(), 1);
    
    // Check that the empty moves list is correctly stored and retrieved
    const auto& retrieved_game = games[0];
    EXPECT_EQ(retrieved_game.moves.size(), 0);
}
// Test AI game (player vs AI)
TEST_F(GameHistoryTest, AIGame) {
    // Create a test game with AI as opponent
    GameHistory::GameRecord game;
    game.moves = {{0}, {4}, {1}, {7}, {2}}; // X wins with top row
    game.playerX_id = 1;
    game.playerO_id = std::nullopt; // AI plays O
    game.winner_id = 1;
    game.timestamp = std::chrono::system_clock::now();
    
    // Save the game
    EXPECT_TRUE(history->saveGame(game));
    
    // Retrieve player's games
    auto games = history->getPlayerGames(1);
    EXPECT_EQ(games.size(), 1);
    
    // Check that AI is correctly stored as null
    const auto& retrieved_game = games[0];
    EXPECT_TRUE(retrieved_game.playerX_id.has_value());
    EXPECT_FALSE(retrieved_game.playerO_id.has_value());
}

// Test AI winning
TEST_F(GameHistoryTest, AIWins) {
    // Create a test game where AI wins
    GameHistory::GameRecord game;
    game.moves = {{0}, {3}, {1}, {4}, {6}, {5}}; // O wins with middle column
    game.playerX_id = 1;
    game.playerO_id = std::nullopt; // AI plays O
    game.winner_id = std::nullopt; // AI wins
    game.timestamp = std::chrono::system_clock::now();
    
    // Save the game
    EXPECT_TRUE(history->saveGame(game));
    
    // Retrieve all games
    auto games = history->getAllGames();
    EXPECT_EQ(games.size(), 1);
    
    // Check that winner is correctly stored as null
    const auto& retrieved_game = games[0];
    EXPECT_FALSE(retrieved_game.winner_id.has_value());
}

// Test draw game
TEST_F(GameHistoryTest, DrawGame) {
    // Create a test draw game
    GameHistory::GameRecord game;
    game.moves = {{0}, {4}, {8}, {2}, {6}, {3}, {5}, {1}, {7}}; // Full board, draw
    game.playerX_id = 1;
    game.playerO_id = 2;
    game.winner_id = -1; // Draw
    game.timestamp = std::chrono::system_clock::now();
    
    // Save the game
    EXPECT_TRUE(history->saveGame(game));
    
    // Retrieve all games
    auto games = history->getAllGames();
    EXPECT_EQ(games.size(), 1);
    
    // Check that draw is correctly stored as -1
    const auto& retrieved_game = games[0];
    EXPECT_TRUE(retrieved_game.winner_id.has_value());
    EXPECT_EQ(retrieved_game.winner_id.value(), -1);
}

// Test multiple games and retrieval order
TEST_F(GameHistoryTest, MultipleGamesAndOrder) {
    // Create test games at different times
    GameHistory::GameRecord game1;
    game1.moves = {{0}, {4}, {1}, {7}, {2}}; // X wins with top row
    game1.playerX_id = 1;
    game1.playerO_id = 2;
    game1.winner_id = 1;
    game1.timestamp = std::chrono::system_clock::now() - std::chrono::hours(2);
    
    GameHistory::GameRecord game2;
    game2.moves = {{4}, {0}, {8}, {2}, {6}, {3}, {5}, {1}, {7}}; // Draw
    game2.playerX_id = 1;
    game2.playerO_id = 3;
    game2.winner_id = -1;
    game2.timestamp = std::chrono::system_clock::now() - std::chrono::hours(1);
    
    GameHistory::GameRecord game3;
    game3.moves = {{8}, {4}, {2}, {0}, {5}}; // X wins
    game3.playerX_id = 2;
    game3.playerO_id = 3;
    game3.winner_id = 2;
    game3.timestamp = std::chrono::system_clock::now();
    
    // Save games
    EXPECT_TRUE(history->saveGame(game1));
    EXPECT_TRUE(history->saveGame(game2));
    EXPECT_TRUE(history->saveGame(game3));
    
    // Test getAllGames (should be ordered by timestamp, newest first)
    auto allGames = history->getAllGames();
    EXPECT_EQ(allGames.size(), 3);
    EXPECT_EQ(allGames[0].playerX_id.value(), 2); // game3
    EXPECT_EQ(allGames[1].winner_id.value(), -1); // game2
    EXPECT_EQ(allGames[2].playerX_id.value(), 1); // game1
    
    // Test getPlayerGames for player 1
    auto player1Games = history->getPlayerGames(1);
    EXPECT_EQ(player1Games.size(), 2);
    
    // Test getLatestGames
    auto latestGames = history->getLatestGames(2);
    EXPECT_EQ(latestGames.size(), 2);
    EXPECT_EQ(latestGames[0].playerX_id.value(), 2); // game3
    EXPECT_EQ(latestGames[1].winner_id.value(), -1); // game2
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}