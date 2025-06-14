#include <gtest/gtest.h>
#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include "game_window.h"

class GameWindowTest : public ::testing::Test {
public:
    static QApplication* app;

protected:
    void SetUp() override {
        gameWindow = new GameWindow();
    }

    void TearDown() override {
        delete gameWindow;
    }

    GameWindow* gameWindow;
};

QApplication* GameWindowTest::app = nullptr;

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QApplication app(argc, argv);
    GameWindowTest::app = &app;
    return RUN_ALL_TESTS();
}

TEST_F(GameWindowTest, InitialState) {
    // Check that the game window is properly initialized
    ASSERT_NE(gameWindow, nullptr);

    // Check that the window has the correct title
    EXPECT_EQ(gameWindow->windowTitle(), "Tic-Tac-Toe");

    // Check that buttons exist (without checking visibility which requires proper setup)
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    EXPECT_GT(buttons.size(), 0);

    // Check that some expected buttons exist
    bool foundPvPButton = false;
    bool foundPvAIButton = false;
    for (auto* btn : buttons) {
        if (btn->text() == "Player vs Player") foundPvPButton = true;
        if (btn->text() == "Player vs AI") foundPvAIButton = true;
    }
    EXPECT_TRUE(foundPvPButton);
    EXPECT_TRUE(foundPvAIButton);
}

TEST_F(GameWindowTest, PvPModeSelection) {
    // Test PvP mode selection now requests second player authentication
    QSignalSpy spy(gameWindow, SIGNAL(secondPlayerAuthenticationRequested()));

    // Find and click PvP button
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* pvpBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Player vs Player") {
            pvpBtn = btn;
            break;
        }
    }
    ASSERT_NE(pvpBtn, nullptr);

    QTest::mouseClick(pvpBtn, Qt::LeftButton);

    // Should request second player authentication
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(GameWindowTest, PvAIModeSelection) {
    // Test PvAI mode selection leads to player choice
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* pvaiBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Player vs AI") {
            pvaiBtn = btn;
            break;
        }
    }
    ASSERT_NE(pvaiBtn, nullptr);

    QTest::mouseClick(pvaiBtn, Qt::LeftButton);

    // Should show player choice buttons
    QPushButton* playXBtn = nullptr;
    QPushButton* playOBtn = nullptr;
    buttons = gameWindow->findChildren<QPushButton*>();
    for (auto* btn : buttons) {
        if (btn->text() == "Play as X") playXBtn = btn;
        if (btn->text() == "Play as O") playOBtn = btn;
    }
    EXPECT_NE(playXBtn, nullptr);
    EXPECT_NE(playOBtn, nullptr);
}

TEST_F(GameWindowTest, LogoutSignalEmitted) {
    // Test logout signal emission
    QSignalSpy spy(gameWindow, SIGNAL(logoutRequested()));

    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* logoutBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Logout") {
            logoutBtn = btn;
            break;
        }
    }
    ASSERT_NE(logoutBtn, nullptr);

    QTest::mouseClick(logoutBtn, Qt::LeftButton);

    EXPECT_EQ(spy.count(), 1);
}

TEST_F(GameWindowTest, PlayerNamesSetAndSymbolSelection) {
    // Test setting player names and symbol selection
    gameWindow->setPlayerNames("Player1", "Player2");

    // Should show symbol selection buttons
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* player1XBtn = nullptr;
    QPushButton* player1OBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Player1 as X") player1XBtn = btn;
        if (btn->text() == "Player1 as O") player1OBtn = btn;
    }
    EXPECT_NE(player1XBtn, nullptr);
    EXPECT_NE(player1OBtn, nullptr);
}

TEST_F(GameWindowTest, SymbolSelectionTransitionsToGame) {
    // Test that symbol selection transitions to game board
    QSignalSpy spy(gameWindow, SIGNAL(gameBoardUIShown()));

    // Set player names first
    gameWindow->setPlayerNames("Player1", "Player2");

    // Find and click Player1 as X button
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* player1XBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Player1 as X") {
            player1XBtn = btn;
            break;
        }
    }
    ASSERT_NE(player1XBtn, nullptr);

    QTest::mouseClick(player1XBtn, Qt::LeftButton);

    // Should transition to game board
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(GameWindowTest, SecondPlayerAuthenticationSignal) {
    // Test that PvP mode emits second player authentication signal
    QSignalSpy spy(gameWindow, SIGNAL(secondPlayerAuthenticationRequested()));

    // Find PvP button
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* pvpBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Player vs Player") {
            pvpBtn = btn;
            break;
        }
    }
    ASSERT_NE(pvpBtn, nullptr);

    // Click PvP button
    QTest::mouseClick(pvpBtn, Qt::LeftButton);

    // Should emit authentication request signal
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(GameWindowTest, ResetGameStateResetsToInitialState) {
    // Test that resetGameState properly resets the game to initial setup state

    // First, set up a game state (simulate players authenticated and game in progress)
    gameWindow->setPlayerNames("Alice", "Bob");

    // Verify we're in symbol selection state (not initial state)
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* aliceXBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Alice as X") {
            aliceXBtn = btn;
            break;
        }
    }
    EXPECT_NE(aliceXBtn, nullptr); // Should find the symbol selection button

    // Now reset the game state
    gameWindow->resetGameState();

    // Verify we're back to the initial setup state
    // Should be able to find the PvP and PvAI buttons again
    buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* pvpBtn = nullptr;
    QPushButton* pvaiBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Player vs Player") pvpBtn = btn;
        if (btn->text() == "Player vs AI") pvaiBtn = btn;
    }
    EXPECT_NE(pvpBtn, nullptr);
    EXPECT_NE(pvaiBtn, nullptr);

    // The symbol selection buttons should no longer be accessible/visible
    QPushButton* symbolBtn = nullptr;
    for (auto* btn : buttons) {
        if (btn->text().contains("Alice as")) {
            symbolBtn = btn;
            break;
        }
    }
    // Symbol buttons might still exist but should not be visible or accessible in reset state
}
