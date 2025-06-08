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
    // Test PvP mode selection
    QPushButton* pvpButton = gameWindow->findChild<QPushButton*>();
    ASSERT_NE(pvpButton, nullptr);
    
    QSignalSpy spy(gameWindow, SIGNAL(gameBoardUIShown()));
    
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
    
    // Should transition to game board
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