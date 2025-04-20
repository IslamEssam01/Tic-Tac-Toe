#include <gtest/gtest.h>
#include "../src/game_history.h"
#include <filesystem>
#include <thread>

// Use a test-specific database file to avoid conflicts
const std::string TEST_DB_PATH = "test_game_history.db";

class GameHistoryTest : public ::testing::Test {
protected:
    GameHistory* history;
    
    void SetUp() override {
        // Remove test database if it exists
        if (std::filesystem::exists(TEST_DB_PATH)) {
            std::filesystem::remove(TEST_DB_PATH);
        }
        
        // Create a new game history object for each test
        history = new GameHistory(TEST_DB_PATH);
        ASSERT_TRUE(history->initialize());
    }
    
    void TearDown() override {
        delete history;
        
        // Clean up test database
        if (std::filesystem::exists(TEST_DB_PATH)) {
            std::filesystem::remove(TEST_DB_PATH);
        }
    }
};

TEST_F(GameHistoryTest, SaveAndRetrieveGame) {
    // Arrange
    std::vector<int> moves = {0, 4, 1, 5, 2}; // X wins with top row
    std::optional<int> playerX_ID = 1;
    std::optional<int> playerO_ID = 2;
    std::optional<int> winner_ID = 1;
    time_t timestamp = std::time(nullptr);
    
    // Act
    int gameId = history->saveGame(moves, playerX_ID, playerO_ID, winner_ID, timestamp);
    
    // Assert
    ASSERT_GT(gameId, 0) << "Game should be saved with a positive ID";
    
    auto retrievedGame = history->getGame(gameId);
    ASSERT_TRUE(retrievedGame.has_value()) << "Should be able to retrieve saved game";
    
    // Verify game data
    EXPECT_EQ(retrievedGame->id, gameId);
    EXPECT_EQ(retrievedGame->moves, moves);
    EXPECT_EQ(retrievedGame->playerX_ID, playerX_ID);
    EXPECT_EQ(retrievedGame->playerO_ID, playerO_ID);
    EXPECT_EQ(retrievedGame->winner_ID, winner_ID);
    EXPECT_EQ(retrievedGame->timestamp, timestamp);
}

TEST_F(GameHistoryTest, SaveGameWithAI) {
    // Arrange
    std::vector<int> moves = {0, 4, 1, 5, 2};
    std::optional<int> playerX_ID = 1;
    std::optional<int> playerO_ID = std::nullopt; // AI
    std::optional<int> winner_ID = 1;
    
    // Act
    int gameId = history->saveGame(moves, playerX_ID, playerO_ID, winner_ID);
    
    // Assert
    auto retrievedGame = history->getGame(gameId);
    ASSERT_TRUE(retrievedGame.has_value());
    
    EXPECT_EQ(retrievedGame->playerX_ID, playerX_ID);
    EXPECT_FALSE(retrievedGame->playerO_ID.has_value()) << "AI player should be stored as NULL";
    EXPECT_EQ(retrievedGame->winner_ID, winner_ID);
}

TEST_F(GameHistoryTest, SaveGameWithAIWinner) {
    // Arrange
    std::vector<int> moves = {0, 4, 1, 5, 2};
    std::optional<int> playerX_ID = std::nullopt; // AI
    std::optional<int> playerO_ID = 2;
    std::optional<int> winner_ID = std::nullopt; // AI wins
    
    // Act
    int gameId = history->saveGame(moves, playerX_ID, playerO_ID, winner_ID);
    
    // Assert
    auto retrievedGame = history->getGame(gameId);
    ASSERT_TRUE(retrievedGame.has_value());
    
    EXPECT_FALSE(retrievedGame->playerX_ID.has_value());
    EXPECT_EQ(retrievedGame->playerO_ID, playerO_ID);
    EXPECT_FALSE(retrievedGame->winner_ID.has_value()) << "AI winner should be stored as NULL";
}

TEST_F(GameHistoryTest, SaveGameWithDraw) {
    // Arrange
    std::vector<int> moves = {0, 1, 2, 3, 5, 4, 6, 7, 8}; // Draw game
    std::optional<int> playerX_ID = 1;
    std::optional<int> playerO_ID = 2;
    std::optional<int> winner_ID = -1; // Draw
    
    // Act
    int gameId = history->saveGame(moves, playerX_ID, playerO_ID, winner_ID);
    
    // Assert
    auto retrievedGame = history->getGame(gameId);
    ASSERT_TRUE(retrievedGame.has_value());
    
    EXPECT_EQ(retrievedGame->winner_ID, -1) << "Draw should be stored as -1";
}

TEST_F(GameHistoryTest, GetNonExistentGame) {
    // Act & Assert
    auto game = history->getGame(9999);
    EXPECT_FALSE(game.has_value()) << "Should return nullopt for non-existent game";
}

TEST_F(GameHistoryTest, GetAllGames) {
    // Arrange
    history->saveGame({0, 1, 2}, 1, 2, 1);
    history->saveGame({3, 4, 5}, 3, 4, 3);
    
    // Act
    auto games = history->getAllGames();
    
    // Assert
    EXPECT_EQ(games.size(), 2) << "Should retrieve all saved games";
}

TEST_F(GameHistoryTest, GetPlayerGames) {
    // Arrange
    // Player 1 in both games
    history->saveGame({0, 1, 2}, 1, 2, 1);
    history->saveGame({3, 4, 5}, 3, 1, 1);
    // Player 1 not in this game
    history->saveGame({6, 7, 8}, 4, 5, 4);
    
    // Act
    auto playerGames = history->getPlayerGames(1);
    
    // Assert
    EXPECT_EQ(playerGames.size(), 2) << "Should retrieve only games with player 1";
    
    // Each game should have player 1 as either X or O
    for (const auto& game : playerGames) {
        bool hasPlayer1 = (game.playerX_ID.has_value() && game.playerX_ID.value() == 1) || 
                         (game.playerO_ID.has_value() && game.playerO_ID.value() == 1);
        EXPECT_TRUE(hasPlayer1) << "Player 1 should be in each retrieved game";
    }
}

TEST_F(GameHistoryTest, DeleteGame) {
    // Arrange
    int gameId = history->saveGame({0, 1, 2}, 1, 2, 1);
    
    // Act & Assert
    EXPECT_TRUE(history->deleteGame(gameId)) << "Should successfully delete existing game";
    
    auto game = history->getGame(gameId);
    EXPECT_FALSE(game.has_value()) << "Game should no longer exist after deletion";
}

TEST_F(GameHistoryTest, UpdateTimestamp) {
    // Arrange
    time_t currentTime = std::time(nullptr);
    time_t futureTime = currentTime + 3600; // 1 hour in the future
    
    // Act
    int gameId = history->saveGame({0, 1, 2}, 1, 2, 1, futureTime);
    
    // Assert
    auto game = history->getGame(gameId);
    ASSERT_TRUE(game.has_value());
    EXPECT_EQ(game->timestamp, futureTime) << "Custom timestamp should be stored correctly";
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}