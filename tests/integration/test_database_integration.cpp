#include <gtest/gtest.h>
#include <QCoreApplication>
#include <QTemporaryDir>
#include <QTemporaryFile>
#include <filesystem>
#include <thread>
#include <chrono>
#include <memory>

#include "auth/user_auth.h"
#include "game_history.h"

class DatabaseIntegrationTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create temporary directory for test databases
        tempDir = std::make_unique<QTemporaryDir>();
        ASSERT_TRUE(tempDir->isValid());
        
        QString basePath = tempDir->path();
        authDbPath = (basePath + "/auth_test.db").toStdString();
        historyDbPath = (basePath + "/history_test.db").toStdString();
        
        // Initialize components
        userAuth = std::make_unique<UserAuth>(authDbPath);
        gameHistory = std::make_unique<GameHistory>(historyDbPath);
    }
    
    void TearDown() override {
        // Components will be cleaned up by unique_ptr destructors
        // Temporary directory will be cleaned up automatically
    }
    
    std::unique_ptr<QTemporaryDir> tempDir;
    std::string authDbPath;
    std::string historyDbPath;
    std::unique_ptr<UserAuth> userAuth;
    std::unique_ptr<GameHistory> gameHistory;
};

// Test that both databases can be created and initialized without conflicts
TEST_F(DatabaseIntegrationTest, DatabaseCreationAndInitialization) {
    // Verify database files exist
    EXPECT_TRUE(std::filesystem::exists(authDbPath));
    EXPECT_TRUE(std::filesystem::exists(historyDbPath));
    
    // Verify databases are functional
    EXPECT_TRUE(userAuth->registerUser("testuser", "test123"));
    
    int gameId = gameHistory->initializeGame(1001, 1002);
    EXPECT_GT(gameId, 0);
    
    // Verify we can read back the data
    EXPECT_TRUE(userAuth->login("testuser", "test123"));
    
    auto game = gameHistory->getGameById(gameId);
    EXPECT_EQ(game.id, gameId);
    EXPECT_EQ(game.playerX_id.value(), 1001);
    EXPECT_EQ(game.playerO_id.value(), 1002);
}

// Test concurrent access to different databases
TEST_F(DatabaseIntegrationTest, ConcurrentDatabaseAccess) {
    // Register initial users
    EXPECT_TRUE(userAuth->registerUser("user1", "pass123"));
    EXPECT_TRUE(userAuth->registerUser("user2", "pass456"));
    
    // Create games concurrently while doing auth operations
    std::vector<std::thread> threads;
    std::vector<int> gameIds(10, -1);
    std::vector<bool> authResults(10, false);
    
    // Start concurrent operations
    for (int i = 0; i < 10; i++) {
        threads.emplace_back([&, i]() {
            // Alternate between auth and game history operations
            if (i % 2 == 0) {
                // Auth operations
                std::string username = "concurrent_user_" + std::to_string(i);
                std::string password = "pass_" + std::to_string(i) + "123";
                
                if (userAuth->registerUser(username, password)) {
                    authResults[i] = userAuth->login(username, password);
                }
            } else {
                // Game history operations
                int playerId1 = 2000 + i;
                int playerId2 = 3000 + i;
                gameIds[i] = gameHistory->initializeGame(playerId1, playerId2);
                
                if (gameIds[i] > 0) {
                    gameHistory->recordMove(gameIds[i], i % 9);
                    gameHistory->setWinner(gameIds[i], playerId1);
                }
            }
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    // Verify results
    for (int i = 0; i < 10; i++) {
        if (i % 2 == 0) {
            EXPECT_TRUE(authResults[i]) << "Auth operation failed for thread " << i;
        } else {
            EXPECT_GT(gameIds[i], 0) << "Game creation failed for thread " << i;
        }
    }
}

// Test database persistence across component recreation
TEST_F(DatabaseIntegrationTest, DatabasePersistenceAcrossRecreation) {
    // Create initial data
    EXPECT_TRUE(userAuth->registerUser("persistent_user", "persist123"));
    
    int gameId = gameHistory->initializeGame(5001, 5002);
    EXPECT_GT(gameId, 0);
    EXPECT_TRUE(gameHistory->recordMove(gameId, 4));
    EXPECT_TRUE(gameHistory->recordMove(gameId, 0));
    EXPECT_TRUE(gameHistory->setWinner(gameId, 5001));
    
    // Destroy and recreate components
    userAuth.reset();
    gameHistory.reset();
    
    userAuth = std::make_unique<UserAuth>(authDbPath);
    gameHistory = std::make_unique<GameHistory>(historyDbPath);
    
    // Verify data persisted
    EXPECT_TRUE(userAuth->login("persistent_user", "persist123"));
    
    auto game = gameHistory->getGameById(gameId);
    EXPECT_EQ(game.id, gameId);
    EXPECT_EQ(game.playerX_id.value(), 5001);
    EXPECT_EQ(game.playerO_id.value(), 5002);
    EXPECT_EQ(game.winner_id.value(), 5001);
    EXPECT_EQ(game.moves.size(), 2);
    EXPECT_EQ(game.moves[0].position, 4);
    EXPECT_EQ(game.moves[1].position, 0);
}

// Test large dataset handling
TEST_F(DatabaseIntegrationTest, LargeDatasetHandling) {
    const int NUM_USERS = 1000;
    const int NUM_GAMES = 500;
    
    // Create many users
    for (int i = 0; i < NUM_USERS; i++) {
        std::string username = "user_" + std::to_string(i);
        std::string password = "pass_" + std::to_string(i) + "123";
        EXPECT_TRUE(userAuth->registerUser(username, password));
    }
    
    // Create many games
    std::vector<int> gameIds;
    for (int i = 0; i < NUM_GAMES; i++) {
        int playerX = 10000 + (i * 2);
        int playerO = 10000 + (i * 2) + 1;
        
        int gameId = gameHistory->initializeGame(playerX, playerO);
        EXPECT_GT(gameId, 0);
        gameIds.push_back(gameId);
        
        // Add some moves to each game
        gameHistory->recordMove(gameId, i % 9);
        gameHistory->recordMove(gameId, (i + 1) % 9);
        gameHistory->recordMove(gameId, (i + 2) % 9);
        
        // Set winner for some games
        if (i % 3 == 0) {
            gameHistory->setWinner(gameId, playerX);
        } else if (i % 3 == 1) {
            gameHistory->setWinner(gameId, playerO);
        }
        // Leave some games unfinished
    }
    
    // Verify we can retrieve all games
    auto allGames = gameHistory->getAllGames();
    EXPECT_EQ(allGames.size(), NUM_GAMES);
    
    // Verify we can still do auth operations
    EXPECT_TRUE(userAuth->login("user_500", "pass_500123"));
    EXPECT_FALSE(userAuth->login("user_500", "wrong_password"));
    
    // Test querying specific player games
    auto playerGames = gameHistory->getPlayerGames(10500); // Player from game 250
    EXPECT_GT(playerGames.size(), 0);
}

// Test database schema integrity
TEST_F(DatabaseIntegrationTest, DatabaseSchemaIntegrity) {
    // Test auth database schema
    EXPECT_TRUE(userAuth->registerUser("schema_test", "test123"));
    
    // Test duplicate username prevention
    EXPECT_FALSE(userAuth->registerUser("schema_test", "different_pass"));
    
    // Test login with correct credentials
    EXPECT_TRUE(userAuth->login("schema_test", "test123"));
    
    // Test game history database schema
    int gameId = gameHistory->initializeGame(7001, std::nullopt); // Human vs AI
    EXPECT_GT(gameId, 0);
    
    // Test game state tracking
    EXPECT_TRUE(gameHistory->isGameActive(gameId));
    
    EXPECT_TRUE(gameHistory->recordMove(gameId, 0));
    EXPECT_TRUE(gameHistory->recordMove(gameId, 1));
    EXPECT_TRUE(gameHistory->recordMove(gameId, 2));
    
    EXPECT_TRUE(gameHistory->setWinner(gameId, 7001));
    EXPECT_FALSE(gameHistory->isGameActive(gameId));
    
    // Test retrieving game data
    auto game = gameHistory->getGameById(gameId);
    EXPECT_EQ(game.playerX_id.value(), 7001);
    EXPECT_FALSE(game.playerO_id.has_value()); // AI player
    EXPECT_EQ(game.winner_id.value(), 7001);
    EXPECT_EQ(game.moves.size(), 3);
}

// Test error handling with database operations
TEST_F(DatabaseIntegrationTest, DatabaseErrorHandling) {
    // Test invalid game operations
    EXPECT_FALSE(gameHistory->recordMove(99999, 0)); // Non-existent game
    EXPECT_FALSE(gameHistory->setWinner(99999, 1001)); // Non-existent game
    
    // Test retrieving non-existent game
    auto nonExistentGame = gameHistory->getGameById(99999);
    EXPECT_EQ(nonExistentGame.id, 99999); // ID is set but other fields are default
    EXPECT_EQ(nonExistentGame.moves.size(), 0);
    
    // Test querying non-existent player
    auto noGames = gameHistory->getPlayerGames(99999);
    EXPECT_EQ(noGames.size(), 0);
    
    // Test auth error scenarios
    EXPECT_FALSE(userAuth->login("nonexistent", "password"));
    EXPECT_FALSE(userAuth->registerUser("", "")); // Empty credentials
    
    // Test password validation
    EXPECT_FALSE(userAuth->registerUser("user", "123")); // Too short
    EXPECT_FALSE(userAuth->registerUser("user", "12345")); // No letters
    EXPECT_FALSE(userAuth->registerUser("user", "abcde")); // No digits
}

// Test database file permissions and access
TEST_F(DatabaseIntegrationTest, DatabaseFileAccess) {
    // Verify database files are readable and writable
    EXPECT_TRUE(std::filesystem::exists(authDbPath));
    EXPECT_TRUE(std::filesystem::exists(historyDbPath));
    
    // Count initial records instead of relying on file size
    auto initialHistoryGames = gameHistory->getAllGames().size();
    
    // Perform operations that should add records
    EXPECT_TRUE(userAuth->registerUser("filetest1", "test123"));
    EXPECT_TRUE(userAuth->registerUser("filetest2", "test456"));
    
    int gameId1 = gameHistory->initializeGame(8001, 8002);
    int gameId2 = gameHistory->initializeGame(8003, std::nullopt);
    
    gameHistory->recordMove(gameId1, 0);
    gameHistory->recordMove(gameId1, 1);
    gameHistory->setWinner(gameId1, 8001);
    
    gameHistory->recordMove(gameId2, 4);
    gameHistory->recordMove(gameId2, 3);
    gameHistory->setWinner(gameId2, -2); // AI wins
    
    // Verify operations actually added data
    auto finalHistoryGames = gameHistory->getAllGames().size();
    EXPECT_EQ(finalHistoryGames, initialHistoryGames + 2);
    
    // Verify auth operations worked
    EXPECT_TRUE(userAuth->login("filetest1", "test123"));
    EXPECT_TRUE(userAuth->login("filetest2", "test456"));
}

// Test cross-component data consistency
TEST_F(DatabaseIntegrationTest, CrossComponentDataConsistency) {
    // Register users in auth system
    EXPECT_TRUE(userAuth->registerUser("alice", "alice123"));
    EXPECT_TRUE(userAuth->registerUser("bob", "bob123"));
    
    // Use consistent player IDs for game history
    int aliceId = qHash(QString("alice"));
    int bobId = qHash(QString("bob"));
    
    // Create game with these player IDs
    int gameId = gameHistory->initializeGame(aliceId, bobId);
    EXPECT_GT(gameId, 0);
    
    // Simulate a complete game
    gameHistory->recordMove(gameId, 4); // X (Alice)
    gameHistory->recordMove(gameId, 0); // O (Bob)
    gameHistory->recordMove(gameId, 1); // X (Alice)
    gameHistory->recordMove(gameId, 3); // O (Bob)
    gameHistory->recordMove(gameId, 7); // X (Alice) wins
    
    gameHistory->setWinner(gameId, aliceId);
    
    // Verify consistency
    EXPECT_TRUE(userAuth->login("alice", "alice123"));
    EXPECT_TRUE(userAuth->login("bob", "bob123"));
    
    auto aliceGames = gameHistory->getPlayerGames(aliceId);
    auto bobGames = gameHistory->getPlayerGames(bobId);
    
    EXPECT_EQ(aliceGames.size(), 1);
    EXPECT_EQ(bobGames.size(), 1);
    EXPECT_EQ(aliceGames[0].id, gameId);
    EXPECT_EQ(bobGames[0].id, gameId);
    EXPECT_EQ(aliceGames[0].winner_id.value(), aliceId);
    EXPECT_EQ(bobGames[0].winner_id.value(), aliceId);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QCoreApplication app(argc, argv);
    return RUN_ALL_TESTS();
}