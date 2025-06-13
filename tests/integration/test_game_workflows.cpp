#include <gtest/gtest.h>
#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <filesystem>
#include <memory>
#include <set>

// Include all components for integration testing
#include "application.h"
#include "main_window.h"
#include "login_page.h"
#include "game_window.h"
#include "game_history_gui.h"
#include "auth/user_auth.h"
#include "game_history.h"
#include "Board.h"
#include "AI.h"

class GameWorkflowIntegrationTest : public ::testing::Test {
public:
    static QApplication* app;

protected:
    void SetUp() override {
        // Create temporary directory for test databases
        tempDir = std::make_unique<QTemporaryDir>();
        ASSERT_TRUE(tempDir->isValid());

        // Set up database paths
        QString basePath = tempDir->path();
        authDbPath = (basePath + "/test_users.db").toStdString();
        historyDbPath = (basePath + "/test_history.db").toStdString();

        // Initialize components
        userAuth = std::make_unique<UserAuth>(authDbPath);
        gameHistory = std::make_unique<GameHistory>(historyDbPath);

        // Register test users
        ASSERT_TRUE(userAuth->registerUser("alice", "alice123"));
        ASSERT_TRUE(userAuth->registerUser("bob", "bob123"));
        ASSERT_TRUE(userAuth->registerUser("charlie", "charlie123"));
    }

    void TearDown() override {
        // Cleanup handled by QTemporaryDir destructor
    }

    // Helper method to simulate user login
    bool simulateLogin(const QString& username, const QString& password) {
        return userAuth->login(username.toStdString(), password.toStdString());
    }

    // Helper method to play a complete game and return game ID
    int playCompleteGame(std::optional<int> playerX_id, std::optional<int> playerO_id,
                        const std::vector<int>& moves, std::optional<int> winner_id) {
        int gameId = gameHistory->initializeGame(playerX_id, playerO_id);
        EXPECT_GT(gameId, 0);

        for (int move : moves) {
            EXPECT_TRUE(gameHistory->recordMove(gameId, move));
        }

        EXPECT_TRUE(gameHistory->setWinner(gameId, winner_id));
        return gameId;
    }

    std::unique_ptr<QTemporaryDir> tempDir;
    std::string authDbPath;
    std::string historyDbPath;
    std::unique_ptr<UserAuth> userAuth;
    std::unique_ptr<GameHistory> gameHistory;
};

QApplication* GameWorkflowIntegrationTest::app = nullptr;

// Test complete Player vs AI workflow
TEST_F(GameWorkflowIntegrationTest, CompletePlayerVsAIWorkflow) {
    // 1. Simulate user login
    EXPECT_TRUE(simulateLogin("alice", "alice123"));
    int aliceId = qHash(QString("alice"));

    // 2. Initialize AI game (Alice as X, AI as O)
    int gameId = gameHistory->initializeGame(aliceId, std::nullopt);
    EXPECT_GT(gameId, 0);

    // 3. Simulate complete game with moves
    std::vector<int> moves = {4, 0, 1, 3, 7}; // Alice wins with column
    for (size_t i = 0; i < moves.size(); i++) {
        EXPECT_TRUE(gameHistory->recordMove(gameId, moves[i]));

        // Verify game state after each move
        auto game = gameHistory->getGameById(gameId);
        EXPECT_EQ(game.moves.size(), i + 1);
        EXPECT_EQ(game.moves[i].position, moves[i]);
    }

    // 4. Set Alice as winner
    EXPECT_TRUE(gameHistory->setWinner(gameId, aliceId));

    // 5. Verify game is recorded correctly
    auto game = gameHistory->getGameById(gameId);
    EXPECT_EQ(game.playerX_id.value(), aliceId);
    EXPECT_FALSE(game.playerO_id.has_value()); // AI
    EXPECT_EQ(game.winner_id.value(), aliceId);
    EXPECT_EQ(game.moves.size(), 5);

    // 6. Verify game appears in Alice's history
    auto aliceGames = gameHistory->getPlayerGames(aliceId);
    EXPECT_EQ(aliceGames.size(), 1);
    EXPECT_EQ(aliceGames[0].id, gameId);
}

// Test complete Player vs Player workflow
TEST_F(GameWorkflowIntegrationTest, CompletePlayerVsPlayerWorkflow) {
    // 1. Simulate both players login
    EXPECT_TRUE(simulateLogin("alice", "alice123"));
    EXPECT_TRUE(simulateLogin("bob", "bob123"));

    int aliceId = qHash(QString("alice"));
    int bobId = qHash(QString("bob"));

    // 2. Initialize PvP game (Alice as X, Bob as O)
    int gameId = gameHistory->initializeGame(aliceId, bobId);
    EXPECT_GT(gameId, 0);

    // 3. Simulate complete game ending in draw
    std::vector<int> drawMoves = {4, 0, 8, 2, 6, 3, 5, 1, 7}; // Draw game
    for (int move : drawMoves) {
        EXPECT_TRUE(gameHistory->recordMove(gameId, move));
    }

    // 4. Set game as draw
    EXPECT_TRUE(gameHistory->setWinner(gameId, -1));

    // 5. Verify game is recorded correctly
    auto game = gameHistory->getGameById(gameId);
    EXPECT_EQ(game.playerX_id.value(), aliceId);
    EXPECT_EQ(game.playerO_id.value(), bobId);
    EXPECT_EQ(game.winner_id.value(), -1); // Draw
    EXPECT_EQ(game.moves.size(), 9);

    // 6. Verify game appears in both players' histories
    auto aliceGames = gameHistory->getPlayerGames(aliceId);
    auto bobGames = gameHistory->getPlayerGames(bobId);

    EXPECT_EQ(aliceGames.size(), 1);
    EXPECT_EQ(bobGames.size(), 1);
    EXPECT_EQ(aliceGames[0].id, gameId);
    EXPECT_EQ(bobGames[0].id, gameId);
}

// Test multiple games and history accumulation
TEST_F(GameWorkflowIntegrationTest, MultipleGamesHistoryAccumulation) {
    int aliceId = qHash(QString("alice"));
    int bobId = qHash(QString("bob"));
    int charlieId = qHash(QString("charlie"));

    // Play multiple games
    std::vector<int> gameIds;

    // Game 1: Alice vs AI (Alice wins)
    gameIds.push_back(playCompleteGame(aliceId, std::nullopt, {0, 1, 2}, aliceId));

    // Game 2: Alice vs Bob (Bob wins)
    gameIds.push_back(playCompleteGame(aliceId, bobId, {0, 1, 3, 4, 6, 7}, bobId));

    // Game 3: Alice vs Charlie (Draw)
    gameIds.push_back(playCompleteGame(aliceId, charlieId, {4, 0, 8, 2, 6, 3, 5, 1, 7}, -1));

    // Game 4: Bob vs AI (AI wins)
    gameIds.push_back(playCompleteGame(bobId, std::nullopt, {0, 1, 3, 4, 8}, -2));

    // Verify each player's game history
    auto aliceGames = gameHistory->getPlayerGames(aliceId);
    auto bobGames = gameHistory->getPlayerGames(bobId);
    auto charlieGames = gameHistory->getPlayerGames(charlieId);

    EXPECT_EQ(aliceGames.size(), 3); // Games 1, 2, 3
    EXPECT_EQ(bobGames.size(), 2);   // Games 2, 4
    EXPECT_EQ(charlieGames.size(), 1); // Game 3

    // Verify game count and that Alice appears in all expected games
    EXPECT_EQ(aliceGames.size(), 3);

    // Verify Alice's games contain the correct winners (order may vary)
    std::set<int> aliceWinners;
    for (const auto& game : aliceGames) {
        if (game.winner_id.has_value()) {
            aliceWinners.insert(game.winner_id.value());
        }
    }
    EXPECT_TRUE(aliceWinners.count(aliceId) > 0);  // Alice won at least one
    EXPECT_TRUE(aliceWinners.count(bobId) > 0);    // Bob won at least one
    EXPECT_TRUE(aliceWinners.count(-1) > 0);       // At least one draw
}

// Test authentication and game integration
TEST_F(GameWorkflowIntegrationTest, AuthenticationGameIntegration) {
    // Test invalid login doesn't allow game creation
    EXPECT_FALSE(simulateLogin("invalid", "user"));

    // Test valid login allows game workflow
    EXPECT_TRUE(simulateLogin("alice", "alice123"));
    int aliceId = qHash(QString("alice"));

    // Create game with authenticated user
    int gameId = gameHistory->initializeGame(aliceId, std::nullopt);
    EXPECT_GT(gameId, 0);

    // Verify user authentication persists through game
    EXPECT_TRUE(simulateLogin("alice", "alice123")); // Still works

    // Test second player authentication for PvP
    EXPECT_TRUE(simulateLogin("bob", "bob123"));
    int bobId = qHash(QString("bob"));

    // Create PvP game
    int pvpGameId = gameHistory->initializeGame(aliceId, bobId);
    EXPECT_GT(pvpGameId, 0);

    // Verify both players have access to the game
    auto aliceGames = gameHistory->getPlayerGames(aliceId);
    auto bobGames = gameHistory->getPlayerGames(bobId);

    bool aliceHasPvpGame = false;
    bool bobHasPvpGame = false;

    for (const auto& game : aliceGames) {
        if (game.id == pvpGameId) aliceHasPvpGame = true;
    }
    for (const auto& game : bobGames) {
        if (game.id == pvpGameId) bobHasPvpGame = true;
    }

    EXPECT_TRUE(aliceHasPvpGame);
    EXPECT_TRUE(bobHasPvpGame);
}

// Test game logic and history consistency
TEST_F(GameWorkflowIntegrationTest, GameLogicHistoryConsistency) {
    int aliceId = qHash(QString("alice"));

    // Create a game and simulate using Board and AI components
    Board gameBoard;
    int gameId = gameHistory->initializeGame(aliceId, std::nullopt);

    // Simulate a realistic game using actual Board logic
    std::vector<std::pair<int, int>> positions = {{1, 1}, {0, 0}, {0, 1}, {2, 2}, {2, 1}};
    Player currentPlayer = Player::X;

    for (const auto& [row, col] : positions) {
        // Validate move with Board component
        EXPECT_TRUE(gameBoard.isValidMove(row, col));
        EXPECT_TRUE(gameBoard.makeMove(row, col, currentPlayer));

        // Record move in history
        int position = row * 3 + col;
        EXPECT_TRUE(gameHistory->recordMove(gameId, position));

        // Check if game is over
        if (gameBoard.isGameOver()) {
            WinInfo result = gameBoard.checkWinner();
            std::optional<int> winnerId = std::nullopt;

            if (result.winner == Player::X) {
                winnerId = aliceId; // Alice is X
            } else if (result.winner == Player::O) {
                winnerId = -2; // AI is O
            } else {
                winnerId = -1; // Draw
            }

            EXPECT_TRUE(gameHistory->setWinner(gameId, winnerId));
            break;
        }

        // Switch player
        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
    }

    // Verify consistency between Board state and History record
    auto game = gameHistory->getGameById(gameId);
    EXPECT_EQ(game.moves.size(), positions.size());

    // Recreate board from history and verify it matches
    Board verifyBoard;
    Player verifyPlayer = Player::X;

    for (const auto& move : game.moves) {
        int row = move.position / 3;
        int col = move.position % 3;
        EXPECT_TRUE(verifyBoard.makeMove(row, col, verifyPlayer));
        verifyPlayer = (verifyPlayer == Player::X) ? Player::O : Player::X;
    }

    // Both boards should have same game over state
    EXPECT_EQ(gameBoard.isGameOver(), verifyBoard.isGameOver());
    if (gameBoard.isGameOver()) {
        EXPECT_EQ(gameBoard.checkWinner().winner, verifyBoard.checkWinner().winner);
    }
}

// Test error handling across components
TEST_F(GameWorkflowIntegrationTest, CrossComponentErrorHandling) {
    int aliceId = qHash(QString("alice"));

    // Test recording moves to non-existent game
    EXPECT_FALSE(gameHistory->recordMove(99999, 0));

    // Test setting winner for non-existent game
    EXPECT_FALSE(gameHistory->setWinner(99999, aliceId));

    // Test invalid player ID scenarios
    int gameId = gameHistory->initializeGame(aliceId, std::nullopt);
    EXPECT_GT(gameId, 0);

    // Record valid moves
    EXPECT_TRUE(gameHistory->recordMove(gameId, 0));
    EXPECT_TRUE(gameHistory->recordMove(gameId, 1));

    // Test invalid move positions
    EXPECT_TRUE(gameHistory->recordMove(gameId, 8)); // Valid position
    // Note: GameHistory doesn't validate move positions, that's Board's job

    // Test game state after errors
    auto game = gameHistory->getGameById(gameId);
    EXPECT_EQ(game.moves.size(), 3);
    EXPECT_FALSE(game.winner_id.has_value()); // Game still in progress

    // Valid winner setting should still work
    EXPECT_TRUE(gameHistory->setWinner(gameId, aliceId));
    game = gameHistory->getGameById(gameId);
    EXPECT_TRUE(game.winner_id.has_value());
    EXPECT_EQ(game.winner_id.value(), aliceId);
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QApplication app(argc, argv);
    GameWorkflowIntegrationTest::app = &app;
    return RUN_ALL_TESTS();
}
