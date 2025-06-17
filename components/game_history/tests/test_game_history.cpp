#include <gtest/gtest.h>
#include <filesystem>
#include "game_history.h"
#include <thread>
#include <chrono>
#include <random>
#ifdef _WIN32
#include <process.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif

// Utility function for robust database cleanup
bool robustDatabaseCleanup(const std::string& dbPath) {
    // Try multiple deletion attempts with different methods
    for (int attempts = 0; attempts < 5; ++attempts) {
        std::error_code ec;
        
        // Method 1: std::filesystem::remove
        std::filesystem::remove(dbPath, ec);
        if (!ec && !std::filesystem::exists(dbPath)) {
            return true;
        }
        
        // Method 2: Traditional C remove
        if (std::remove(dbPath.c_str()) == 0 && !std::filesystem::exists(dbPath)) {
            return true;
        }
        
        // Wait and try again
        std::this_thread::sleep_for(std::chrono::milliseconds(50 * (attempts + 1)));
    }
    
    // Final check - if file doesn't exist, consider it success
    return !std::filesystem::exists(dbPath);
}

class GameHistoryTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Generate highly unique database filename to avoid conflicts
        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        auto thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
        auto process_id = static_cast<unsigned long>(getpid());
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100000, 999999);
        
        db_path = "test_tictactoe_" + std::to_string(timestamp) + "_" + 
                  std::to_string(process_id) + "_" + std::to_string(thread_id) + "_" + 
                  std::to_string(dis(gen)) + ".db";
        
        // Remove the database file if it exists
        if (std::filesystem::exists(db_path)) {
            std::filesystem::remove(db_path);
        }
        
        // Create a new GameHistory instance
        history = new GameHistory(db_path, nullptr);
    }

    void TearDown() override {
        // Clean up
        delete history;
        
        // Use robust cleanup utility
        if (!robustDatabaseCleanup(db_path)) {
            // Log warning but don't fail test
            std::cerr << "Warning: Failed to clean up test database: " << db_path << std::endl;
        }
    }

    std::string db_path;
    GameHistory* history;
};

// Test database initialization
TEST_F(GameHistoryTest, InitializeDatabase) {
    EXPECT_TRUE(history->initializeDatabase());
}

// Test game initialization and retrieval
TEST_F(GameHistoryTest, InitializeGame) {
    // Initialize a game between two players
    int game_id = history->initializeGame(1, 2);
    EXPECT_GT(game_id, 0);
    
    // Retrieve the game
    auto game = history->getGameById(game_id);
    EXPECT_EQ(game.id, game_id);
    EXPECT_EQ(game.playerX_id.value(), 1);
    EXPECT_EQ(game.playerO_id.value(), 2);
    EXPECT_FALSE(game.winner_id.has_value());
    EXPECT_EQ(game.moves.size(), 0);
}

// Test username mapping functionality
TEST_F(GameHistoryTest, UsernameMapping) {
    // Register some usernames
    history->registerPlayerUsername(1001, "alice");
    history->registerPlayerUsername(1002, "bob");
    
    // Retrieve usernames
    EXPECT_EQ(history->getPlayerUsername(1001), "alice");
    EXPECT_EQ(history->getPlayerUsername(1002), "bob");
    EXPECT_EQ(history->getPlayerUsername(9999), ""); // Non-existent player
    
    // Test username update (should replace)
    history->registerPlayerUsername(1001, "alice_updated");
    EXPECT_EQ(history->getPlayerUsername(1001), "alice_updated");
}

// Test recording moves
TEST_F(GameHistoryTest, RecordMoves) {
    // Initialize a game
    int game_id = history->initializeGame(1, 2);
    EXPECT_GT(game_id, 0);
    
    // Record a few moves
    EXPECT_TRUE(history->recordMove(game_id, 4)); // Center
    EXPECT_TRUE(history->recordMove(game_id, 0)); // Top-left
    EXPECT_TRUE(history->recordMove(game_id, 8)); // Bottom-right
    
    // Retrieve the game and check moves
    auto game = history->getGameById(game_id);
    EXPECT_EQ(game.moves.size(), 3);
    EXPECT_EQ(game.moves[0].position, 4);
    EXPECT_EQ(game.moves[1].position, 0);
    EXPECT_EQ(game.moves[2].position, 8);
}

// Test setting winner
TEST_F(GameHistoryTest, SetWinner) {
    // Initialize a game
    int game_id = history->initializeGame(1, 2);
    
    // Record some moves
    history->recordMove(game_id, 4); // Center
    history->recordMove(game_id, 0); // Top-left
    history->recordMove(game_id, 1); // Top-middle
    history->recordMove(game_id, 3); // Middle-left
    history->recordMove(game_id, 7); // X wins with 1-4-7 column
    
    // Check game is active before setting winner
    EXPECT_TRUE(history->isGameActive(game_id));
    
    // Set the winner as player 1
    EXPECT_TRUE(history->setWinner(game_id, 1));
    
    // Verify winner was set
    auto game = history->getGameById(game_id);
    EXPECT_TRUE(game.winner_id.has_value());
    EXPECT_EQ(game.winner_id.value(), 1);
    
    // Check game is no longer active
    EXPECT_FALSE(history->isGameActive(game_id));
}

// Test draw game
TEST_F(GameHistoryTest, DrawGame) {
    // Initialize a game
    int game_id = history->initializeGame(1, 2);
    
    // Record moves for a draw
    history->recordMove(game_id, 4); // Center
    history->recordMove(game_id, 0); // Top-left
    history->recordMove(game_id, 8); // Bottom-right
    history->recordMove(game_id, 2); // Top-right
    history->recordMove(game_id, 6); // Bottom-left
    history->recordMove(game_id, 3); // Middle-left
    history->recordMove(game_id, 5); // Middle-right
    history->recordMove(game_id, 1); // Top-middle
    history->recordMove(game_id, 7); // Bottom-middle
    
    // Set as draw (-1)
    EXPECT_TRUE(history->setWinner(game_id, -1));
    
    // Verify draw was set
    auto game = history->getGameById(game_id);
    EXPECT_TRUE(game.winner_id.has_value());
    EXPECT_EQ(game.winner_id.value(), -1);
}

// Test AI game
TEST_F(GameHistoryTest, AIGame) {
    // Initialize a game with AI as opponent
    int game_id = history->initializeGame(1, std::nullopt);
    
    // Record some moves
    history->recordMove(game_id, 0); // Player plays top-left
    history->recordMove(game_id, 4); // AI plays center
    history->recordMove(game_id, 1); // Player plays top-middle
    history->recordMove(game_id, 7); // AI plays bottom-middle
    history->recordMove(game_id, 6); // Player plays bottom-left
    history->recordMove(game_id, 3); // AI plays middle-left and wins
    
    // Set AI as winner (-2)
    EXPECT_TRUE(history->setWinner(game_id, -2));
    
    // Verify AI win was set correctly
    auto game = history->getGameById(game_id);
    EXPECT_TRUE(game.winner_id.has_value());
    EXPECT_EQ(game.winner_id.value(), -2);
    EXPECT_FALSE(game.playerO_id.has_value());
}

// Test multiple games and retrieval order
TEST_F(GameHistoryTest, MultipleGamesAndOrder) {
    // Initialize multiple games with small delays to ensure different timestamps
    int game1_id = history->initializeGame(1, 2);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    int game2_id = history->initializeGame(1, 3);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    int game3_id = history->initializeGame(2, 3);
    
    // Add moves to the games
    history->recordMove(game1_id, 4);
    history->recordMove(game2_id, 0);
    history->recordMove(game3_id, 8);
    
    // Set winners for games
    history->setWinner(game1_id, 1);
    history->setWinner(game2_id, -1); // Draw
    // Leave game3 as active (no winner)
    
    // In order of ID (newest first): game3_id, game2_id, game1_id
    
    // Test getAllGames (should be ordered by ID, newest first)
    auto allGames = history->getAllGames();
    EXPECT_EQ(allGames.size(), 3);
    EXPECT_EQ(allGames[0].id, game3_id);
    EXPECT_EQ(allGames[1].id, game2_id);
    EXPECT_EQ(allGames[2].id, game1_id);
    
    // Test getPlayerGames for player 1
    auto player1Games = history->getPlayerGames(1);
    EXPECT_EQ(player1Games.size(), 2);
    
    // Test getLatestGames
    auto latestGames = history->getLatestGames(2);
    EXPECT_EQ(latestGames.size(), 2);
    EXPECT_EQ(latestGames[0].id, game3_id);
    EXPECT_EQ(latestGames[1].id, game2_id);
}

// Test empty moves serialization and deserialization
TEST_F(GameHistoryTest, EmptyMoves) {
    // Initialize a game but don't add any moves
    int game_id = history->initializeGame(1, 2);
    
    // Verify the game was created with empty moves list
    auto game = history->getGameById(game_id);
    EXPECT_EQ(game.moves.size(), 0);
}

// Test game active status checking
TEST_F(GameHistoryTest, GameActiveStatus) {
    // Initialize a game
    int game_id = history->initializeGame(1, 2);
    
    // Check that new game is active
    EXPECT_TRUE(history->isGameActive(game_id));
    
    // Add some moves
    history->recordMove(game_id, 0);
    history->recordMove(game_id, 4);
    
    // Game should still be active
    EXPECT_TRUE(history->isGameActive(game_id));
    
    // Set a winner
    history->setWinner(game_id, 1);
    
    // Game should no longer be active
    EXPECT_FALSE(history->isGameActive(game_id));
}

// Test updating an existing game
TEST_F(GameHistoryTest, UpdateGame) {
    // Initialize a game
    int game_id = history->initializeGame(1, 2);
    
    // Get the initial game
    auto game = history->getGameById(game_id);
    
    // Modify the game
    game.moves.push_back({4});
    game.moves.push_back({0});
    game.moves.push_back({8});
    
    // Update the game
    EXPECT_TRUE(history->updateGame(game_id, game));
    
    // Get the updated game
    auto updated_game = history->getGameById(game_id);
    
    // Verify the changes
    EXPECT_EQ(updated_game.moves.size(), 3);
    EXPECT_EQ(updated_game.moves[0].position, 4);
    EXPECT_EQ(updated_game.moves[1].position, 0);
    EXPECT_EQ(updated_game.moves[2].position, 8);
}

// Test username mapping with actual game workflow
TEST_F(GameHistoryTest, UsernameGameIntegration) {
    // Register usernames for players
    int alice_id = qHash(QString("alice"));
    int bob_id = qHash(QString("bob"));
    
    history->registerPlayerUsername(alice_id, "alice");
    history->registerPlayerUsername(bob_id, "bob");
    
    // Create a game between alice and bob
    int game_id = history->initializeGame(alice_id, bob_id);
    EXPECT_GT(game_id, 0);
    
    // Add some moves and complete the game
    history->recordMove(game_id, 4);
    history->recordMove(game_id, 0);
    history->recordMove(game_id, 1);
    history->setWinner(game_id, alice_id);
    
    // Verify we can retrieve usernames for the players in this game
    EXPECT_EQ(history->getPlayerUsername(alice_id), "alice");
    EXPECT_EQ(history->getPlayerUsername(bob_id), "bob");
    
    // Verify the game data
    auto game = history->getGameById(game_id);
    EXPECT_EQ(game.playerX_id.value(), alice_id);
    EXPECT_EQ(game.playerO_id.value(), bob_id);
    EXPECT_EQ(game.winner_id.value(), alice_id);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}