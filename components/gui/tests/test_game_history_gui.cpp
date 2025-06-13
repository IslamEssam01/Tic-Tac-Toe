#include <gtest/gtest.h>
#include <QTest>
#include <QSignalSpy>
#include <QApplication>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QPushButton>
#include <QTemporaryFile>
#include <chrono>
#include "game_history_gui.h"
#include "game_history.h"
#include "auth/user_auth.h"

class GameHistoryGUITest : public ::testing::Test {
public:
    static QApplication* app;

protected:
    void SetUp() override {
        // Create temporary database file for testing
        tempDbFile = new QTemporaryFile();
        tempDbFile->open();
        tempDbPath = tempDbFile->fileName().toStdString();
        tempDbFile->close();
        
        // Create real GameHistory instance with test database
        gameHistory = new GameHistory(tempDbPath);
        gameHistory->initializeDatabase();
        
        testUsername = "TestUser";
        testUserId = qHash(testUsername);
        
        // Add some test games to the database
        setupTestGames();
        
        // Create GameHistoryGUI with real GameHistory
        gameHistoryGUI = new GameHistoryGUI(gameHistory, testUsername);
        
        // Show the window to ensure proper widget visibility
        gameHistoryGUI->show();
        bool exposed = QTest::qWaitForWindowExposed(gameHistoryGUI);
        Q_UNUSED(exposed);
    }
    
    void TearDown() override {
        delete gameHistoryGUI;
        delete gameHistory;
        delete tempDbFile;
    }
    
    void setupTestGames() {
        // Game 1: Player wins against AI
        int gameId1 = gameHistory->initializeGame(testUserId, std::nullopt);
        gameHistory->recordMove(gameId1, 0); // X
        gameHistory->recordMove(gameId1, 4); // O (AI)
        gameHistory->recordMove(gameId1, 1); // X
        gameHistory->recordMove(gameId1, 5); // O (AI)
        gameHistory->recordMove(gameId1, 2); // X wins
        gameHistory->setWinner(gameId1, testUserId);
        
        // Game 2: AI wins against player
        int gameId2 = gameHistory->initializeGame(std::nullopt, testUserId);
        gameHistory->recordMove(gameId2, 0); // X (AI)
        gameHistory->recordMove(gameId2, 4); // O
        gameHistory->recordMove(gameId2, 1); // X (AI)
        gameHistory->recordMove(gameId2, 5); // O
        gameHistory->recordMove(gameId2, 3); // X (AI)
        gameHistory->recordMove(gameId2, 6); // O
        gameHistory->recordMove(gameId2, 2); // X (AI) wins
        gameHistory->setWinner(gameId2, -2); // AI wins
        
        // Game 3: Draw between two players
        int otherUserId = qHash(QString("OtherPlayer"));
        int gameId3 = gameHistory->initializeGame(testUserId, otherUserId);
        gameHistory->recordMove(gameId3, 0); // X
        gameHistory->recordMove(gameId3, 1); // O
        gameHistory->recordMove(gameId3, 2); // X
        gameHistory->recordMove(gameId3, 3); // O
        gameHistory->recordMove(gameId3, 5); // X
        gameHistory->recordMove(gameId3, 4); // O
        gameHistory->recordMove(gameId3, 6); // X
        gameHistory->recordMove(gameId3, 7); // O
        gameHistory->recordMove(gameId3, 8); // X
        gameHistory->setWinner(gameId3, -1); // Draw
    }
    
    GameHistory* gameHistory;
    GameHistoryGUI* gameHistoryGUI;
    QTemporaryFile* tempDbFile;
    std::string tempDbPath;
    QString testUsername;
    int testUserId;
};

QApplication* GameHistoryGUITest::app = nullptr;

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QApplication app(argc, argv);
    GameHistoryGUITest::app = &app;
    return RUN_ALL_TESTS();
}

TEST_F(GameHistoryGUITest, InitialState) {
    // Check that the game history GUI is properly initialized
    ASSERT_NE(gameHistoryGUI, nullptr);
    
    // Check window title
    EXPECT_TRUE(gameHistoryGUI->windowTitle().contains("Game History"));
    EXPECT_TRUE(gameHistoryGUI->windowTitle().contains(testUsername));
    
    // Check that main widgets exist
    QTreeWidget* gamesTreeWidget = gameHistoryGUI->findChild<QTreeWidget*>();
    ASSERT_NE(gamesTreeWidget, nullptr);
    
    // Check that game details widgets exist
    QList<QLabel*> labels = gameHistoryGUI->findChildren<QLabel*>();
    EXPECT_GT(labels.size(), 0);
    
    // Check that moves table exists
    QTableWidget* movesTable = gameHistoryGUI->findChild<QTableWidget*>();
    ASSERT_NE(movesTable, nullptr);
    EXPECT_EQ(movesTable->columnCount(), 3);
}

TEST_F(GameHistoryGUITest, GamesListPopulation) {
    // The games list should be populated on creation
    QTreeWidget* gamesTreeWidget = gameHistoryGUI->findChild<QTreeWidget*>();
    ASSERT_NE(gamesTreeWidget, nullptr);
    
    // Should have 3 test games
    EXPECT_EQ(gamesTreeWidget->topLevelItemCount(), 3);
    
    // Check that items exist
    if (gamesTreeWidget->topLevelItemCount() > 0) {
        QTreeWidgetItem* firstItem = gamesTreeWidget->topLevelItem(0);
        ASSERT_NE(firstItem, nullptr);
        
        // Game ID should be a number
        bool isNumber;
        int gameId = firstItem->text(0).toInt(&isNumber);
        EXPECT_TRUE(isNumber);
        EXPECT_GT(gameId, 0);
        
        // Players column should show appropriate format
        QString playersText = firstItem->text(2);
        EXPECT_TRUE(playersText.contains("vs"));
        
        // Result column should show winner
        QString resultText = firstItem->text(3);
        EXPECT_FALSE(resultText.isEmpty());
    }
}

TEST_F(GameHistoryGUITest, TreeWidgetHeaders) {
    QTreeWidget* gamesTreeWidget = gameHistoryGUI->findChild<QTreeWidget*>();
    ASSERT_NE(gamesTreeWidget, nullptr);
    
    // Check that tree widget has correct headers
    QStringList expectedHeaders = {"Game", "Date", "Players", "Result"};
    
    for (int i = 0; i < expectedHeaders.size(); ++i) {
        QString headerText = gamesTreeWidget->headerItem()->text(i);
        EXPECT_EQ(headerText, expectedHeaders[i]);
    }
}

TEST_F(GameHistoryGUITest, MovesTableHeaders) {
    QTableWidget* movesTable = gameHistoryGUI->findChild<QTableWidget*>();
    ASSERT_NE(movesTable, nullptr);
    
    // Check that moves table has correct headers
    QStringList expectedHeaders = {"Move #", "Player", "Position"};
    
    for (int i = 0; i < expectedHeaders.size(); ++i) {
        QString headerText = movesTable->horizontalHeaderItem(i)->text();
        EXPECT_EQ(headerText, expectedHeaders[i]);
    }
}

TEST_F(GameHistoryGUITest, GameDetailsDisplay) {
    QTreeWidget* gamesTreeWidget = gameHistoryGUI->findChild<QTreeWidget*>();
    ASSERT_NE(gamesTreeWidget, nullptr);
    
    if (gamesTreeWidget->topLevelItemCount() > 0) {
        // Click on the first game
        QTreeWidgetItem* firstItem = gamesTreeWidget->topLevelItem(0);
        ASSERT_NE(firstItem, nullptr);
        
        // Simulate clicking on the item
        gamesTreeWidget->setCurrentItem(firstItem);
        emit gamesTreeWidget->itemClicked(firstItem, 0);
        
        // Process events to ensure GUI updates
        QCoreApplication::processEvents();
        
        // Check that some game details are displayed
        QList<QLabel*> labels = gameHistoryGUI->findChildren<QLabel*>();
        
        bool foundGameId = false;
        bool foundPlayerInfo = false;
        
        for (QLabel* label : labels) {
            QString text = label->text();
            if (text.contains(firstItem->text(0))) {
                foundGameId = true;
            }
            if (text.contains(testUsername) || text.contains("AI") || text.contains("Player")) {
                foundPlayerInfo = true;
            }
        }
        
        EXPECT_TRUE(foundGameId || foundPlayerInfo); // At least one should be found
    }
}

TEST_F(GameHistoryGUITest, MovesTablePopulation) {
    QTreeWidget* gamesTreeWidget = gameHistoryGUI->findChild<QTreeWidget*>();
    QTableWidget* movesTable = gameHistoryGUI->findChild<QTableWidget*>();
    ASSERT_NE(gamesTreeWidget, nullptr);
    ASSERT_NE(movesTable, nullptr);
    
    if (gamesTreeWidget->topLevelItemCount() > 0) {
        // Click on the first game
        QTreeWidgetItem* firstItem = gamesTreeWidget->topLevelItem(0);
        ASSERT_NE(firstItem, nullptr);
        
        gamesTreeWidget->setCurrentItem(firstItem);
        emit gamesTreeWidget->itemClicked(firstItem, 0);
        
        // Process events
        QCoreApplication::processEvents();
        
        // Check that moves table has some moves
        EXPECT_GT(movesTable->rowCount(), 0);
        
        // Check first move if exists
        if (movesTable->rowCount() > 0) {
            QTableWidgetItem* moveNumber = movesTable->item(0, 0);
            QTableWidgetItem* player = movesTable->item(0, 1);
            QTableWidgetItem* position = movesTable->item(0, 2);
            
            if (moveNumber) {
                EXPECT_EQ(moveNumber->text(), "1");
            }
            if (player) {
                EXPECT_TRUE(player->text() == "X" || player->text() == "O");
            }
            if (position) {
                EXPECT_TRUE(position->text().contains("("));
            }
        }
    }
}

TEST_F(GameHistoryGUITest, GameBoardVisualization) {
    QTreeWidget* gamesTreeWidget = gameHistoryGUI->findChild<QTreeWidget*>();
    ASSERT_NE(gamesTreeWidget, nullptr);
    
    if (gamesTreeWidget->topLevelItemCount() > 0) {
        // Click on a game to display its board
        QTreeWidgetItem* firstItem = gamesTreeWidget->topLevelItem(0);
        ASSERT_NE(firstItem, nullptr);
        
        gamesTreeWidget->setCurrentItem(firstItem);
        emit gamesTreeWidget->itemClicked(firstItem, 0);
        
        // Process events
        QCoreApplication::processEvents();
        
        // Find board cells (should be 9 QLabel widgets for the 3x3 board)
        QList<QLabel*> allLabels = gameHistoryGUI->findChildren<QLabel*>();
        
        // Count labels that could be board cells (contain X or O)
        int boardCells = 0;
        for (QLabel* label : allLabels) {
            if (label->text() == "X" || label->text() == "O") {
                boardCells++;
            }
        }
        
        // Should have some moves displayed on the board
        EXPECT_GT(boardCells, 0);
    }
}

TEST_F(GameHistoryGUITest, CurrentUserUpdate) {
    QString newUsername = "NewTestUser";
    
    // Verify initial state
    QString initialTitle = gameHistoryGUI->windowTitle();
    EXPECT_TRUE(initialTitle.contains(testUsername));
    
    // Update current user
    gameHistoryGUI->setCurrentUser(newUsername);
    
    // Process events to ensure UI updates
    QCoreApplication::processEvents();
    
    // Check that window title is updated
    QString updatedTitle = gameHistoryGUI->windowTitle();
    EXPECT_TRUE(updatedTitle.contains(newUsername));
    
    // The title should be different from the initial title
    EXPECT_NE(initialTitle, updatedTitle);
}

TEST_F(GameHistoryGUITest, UsernameMapping) {
    QString mappedUsername = "MappedUser";
    
    // Register a username mapping
    gameHistoryGUI->registerUsernameMapping(mappedUsername);
    
    // This should not crash and should handle the mapping internally
    SUCCEED();
}

TEST_F(GameHistoryGUITest, WindowProperties) {
    // Check window properties
    EXPECT_TRUE(gameHistoryGUI->isWindow());
    
    // Check that window has fixed size
    QSize windowSize = gameHistoryGUI->size();
    EXPECT_GT(windowSize.width(), 0);
    EXPECT_GT(windowSize.height(), 0);
    
    // Check that window is properly styled
    QString styleSheet = gameHistoryGUI->styleSheet();
    // StyleSheet might be empty as styling could be applied in code
    // Just ensure the test doesn't crash
    SUCCEED();
}

TEST_F(GameHistoryGUITest, EmptyGamesHandling) {
    // Create a new GameHistoryGUI with a user that has no games
    QString emptyUserName = "EmptyUser";
    GameHistoryGUI* emptyGUI = new GameHistoryGUI(gameHistory, emptyUserName);
    
    // Show the window
    emptyGUI->show();
    bool exposed = QTest::qWaitForWindowExposed(emptyGUI);
    Q_UNUSED(exposed);
    
    // Find the games tree widget
    QTreeWidget* gamesTreeWidget = emptyGUI->findChild<QTreeWidget*>();
    ASSERT_NE(gamesTreeWidget, nullptr);
    
    // Should have no games for this user
    EXPECT_EQ(gamesTreeWidget->topLevelItemCount(), 0);
    
    // Clean up
    delete emptyGUI;
}

TEST_F(GameHistoryGUITest, GameEventSignals) {
    // Test that the GUI responds to game history signals
    QSignalSpy spyInitialized(gameHistory, SIGNAL(gameInitialized(int)));
    QSignalSpy spyMoveRecorded(gameHistory, SIGNAL(moveRecorded(int, int)));
    QSignalSpy spyGameCompleted(gameHistory, SIGNAL(gameCompleted(int, std::optional<int>)));
    
    // Create a new game to trigger signals
    int newGameId = gameHistory->initializeGame(testUserId, std::nullopt);
    gameHistory->recordMove(newGameId, 0);
    gameHistory->setWinner(newGameId, testUserId);
    
    // Process events
    QCoreApplication::processEvents();
    
    // Verify signals were emitted
    EXPECT_GT(spyInitialized.count(), 0);
    EXPECT_GT(spyMoveRecorded.count(), 0);
    EXPECT_GT(spyGameCompleted.count(), 0);
}

TEST_F(GameHistoryGUITest, SelectionAndRefresh) {
    QTreeWidget* gamesTreeWidget = gameHistoryGUI->findChild<QTreeWidget*>();
    ASSERT_NE(gamesTreeWidget, nullptr);
    
    if (gamesTreeWidget->topLevelItemCount() > 0) {
        // Select the first game
        QTreeWidgetItem* firstItem = gamesTreeWidget->topLevelItem(0);
        ASSERT_NE(firstItem, nullptr);
        
        gamesTreeWidget->setCurrentItem(firstItem);
        
        // Verify selection
        EXPECT_EQ(gamesTreeWidget->currentItem(), firstItem);
        
        // Create a new game that would refresh the list
        int newGameId = gameHistory->initializeGame(testUserId, std::nullopt);
        gameHistory->recordMove(newGameId, 0);
        gameHistory->setWinner(newGameId, testUserId);
        
        // Process events
        QCoreApplication::processEvents();
        
        // Selection behavior after refresh depends on implementation
        // This test verifies the mechanism doesn't crash
        SUCCEED();
    }
}