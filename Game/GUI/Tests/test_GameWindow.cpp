#include <gtest/gtest.h>
#include <QApplication>
#include <QTest>
#include "GameWindow.h"

// Fixture to set up QApplication and GameWindow for each test
class GameWindowTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create QApplication (needed for Qt widgets)
        int argc = 0;
        char* argv[] = {nullptr};
        app = new QApplication(argc, argv);
        window = new GameWindow();
    }

    void TearDown() override {
        delete window;
        delete app;
    }

    QApplication* app;
    GameWindow* window;
};

// Test for GameWindow constructor
TEST_F(GameWindowTest, Constructor) {
    // Check initial state
    EXPECT_FALSE(window->gameActive);
    EXPECT_NE(window->statusLabel, nullptr);
    EXPECT_NE(window->setupWidget, nullptr);
    EXPECT_NE(window->gameWidget, nullptr);
    EXPECT_NE(window->boardWidget, nullptr);
    EXPECT_NE(window->newGameButton, nullptr);
    EXPECT_NE(window->pvpButton, nullptr);
    EXPECT_NE(window->pvaiButton, nullptr);
    EXPECT_NE(window->playXButton, nullptr);
    EXPECT_NE(window->playOButton, nullptr);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_NE(window->cells[i][j], nullptr);
        }
    }
    // This test checks that the constructor initializes the game window correctly,
    // ensuring gameActive is false and all widgets are created.
}

// Test for setupUI
TEST_F(GameWindowTest, SetupUI) {
    window->setupUI();
    EXPECT_EQ(window->windowTitle(), QString("Tic-Tac-Toe"));
    EXPECT_FALSE(window->setupWidget->isHidden());
    EXPECT_FALSE(window->gameWidget->isHidden());
    EXPECT_FALSE(window->newGameButton->isHidden());
    EXPECT_EQ(window->statusLabel->text(), QString("Welcome to Tic-Tac-Toe!"));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_FALSE(window->cells[i][j]->isEnabled()); // Cells should be disabled initially
        }
    }
    // This test verifies that setupUI sets up the UI correctly, including window title,
    // initial widget visibility, and disabled board cells.
}

// Test for showGameSetupUI
TEST_F(GameWindowTest, ShowGameSetupUI) {
    window->showGameSetupUI();
    EXPECT_EQ(window->statusLabel->text(), QString("Choose a game mode:"));
    EXPECT_TRUE(window->setupWidget->isVisible());
    EXPECT_FALSE(window->gameWidget->isVisible());
    EXPECT_TRUE(window->pvpButton->isVisible());
    EXPECT_TRUE(window->pvaiButton->isVisible());
    EXPECT_FALSE(window->playXButton->isVisible());
    EXPECT_FALSE(window->playOButton->isVisible());
    EXPECT_FALSE(window->newGameButton->isVisible());
    EXPECT_FALSE(window->gameActive);
    EXPECT_EQ(window->size(), QSize(500, 350));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_FALSE(window->cells[i][j]->isEnabled());
        }
    }
    // This test checks that showGameSetupUI displays the game mode selection UI,
    // hides the game board, and disables the board cells.
}

// Test for showPlayerChoiceUI
TEST_F(GameWindowTest, ShowPlayerChoiceUI) {
    window->showPlayerChoiceUI();
    EXPECT_EQ(window->statusLabel->text(), QString("Playing vs AI. Choose your symbol!"));
    EXPECT_TRUE(window->setupWidget->isVisible());
    EXPECT_FALSE(window->gameWidget->isVisible());
    EXPECT_FALSE(window->pvpButton->isVisible());
    EXPECT_FALSE(window->pvaiButton->isVisible());
    EXPECT_TRUE(window->playXButton->isVisible());
    EXPECT_TRUE(window->playOButton->isVisible());
    EXPECT_EQ(window->size(), QSize(500, 350));
    // This test ensures that showPlayerChoiceUI shows the X/O selection buttons
    // for PvAI mode and hides other widgets.
}

// Test for showGameBoardUI
TEST_F(GameWindowTest, ShowGameBoardUI) {
    window->showGameBoardUI();
    EXPECT_FALSE(window->setupWidget->isVisible());
    EXPECT_TRUE(window->gameWidget->isVisible());
    EXPECT_TRUE(window->newGameButton->isVisible());
    EXPECT_EQ(window->size(), QSize(500, 700));
    EXPECT_TRUE(window->gameActive); // startNewGame sets gameActive to true
    // This test verifies that showGameBoardUI displays the game board,
    // hides setup widgets, and starts a new game.
}

// Test for handlePvpButtonClick
TEST_F(GameWindowTest, HandlePvpButtonClick) {
    window->handlePvpButtonClick();
    EXPECT_EQ(window->gameMode, GameMode::PvP);
    EXPECT_EQ(window->statusLabel->text(), QString("Player vs Player mode selected!"));
    EXPECT_FALSE(window->setupWidget->isVisible());
    EXPECT_TRUE(window->gameWidget->isVisible());
    EXPECT_TRUE(window->gameActive);
    // This test checks that handlePvpButtonClick sets PvP mode and switches to the game board UI.
}

// Test for handlePvaiButtonClick
TEST_F(GameWindowTest, HandlePvaiButtonClick) {
    window->handlePvaiButtonClick();
    EXPECT_EQ(window->gameMode, GameMode::PvAI);
    EXPECT_EQ(window->statusLabel->text(), QString("Playing vs AI. Choose your symbol!"));
    EXPECT_TRUE(window->setupWidget->isVisible());
    EXPECT_FALSE(window->gameWidget->isVisible());
    // This test ensures that handlePvaiButtonClick sets PvAI mode and shows the player choice UI.
}

// Test for handlePlayXButtonClick
TEST_F(GameWindowTest, HandlePlayXButtonClick) {
    window->handlePlayXButtonClick();
    EXPECT_EQ(window->humanPlayer, Player::X);
    EXPECT_EQ(window->aiPlayer, Player::O);
    EXPECT_EQ(window->statusLabel->text(), QString("You are X. AI is O."));
    EXPECT_FALSE(window->setupWidget->isVisible());
    EXPECT_TRUE(window->gameWidget->isVisible());
    EXPECT_TRUE(window->gameActive);
    // This test verifies that handlePlayXButtonClick sets the human as X, AI as O,
    // and starts the game.
}

// Test for handlePlayOButtonClick
TEST_F(GameWindowTest, HandlePlayOButtonClick) {
    window->handlePlayOButtonClick();
    EXPECT_EQ(window->humanPlayer, Player::O);
    EXPECT_EQ(window->aiPlayer, Player::X);
    EXPECT_EQ(window->statusLabel->text(), QString("You are O. AI is X."));
    EXPECT_FALSE(window->setupWidget->isVisible());
    EXPECT_TRUE(window->gameWidget->isVisible());
    EXPECT_TRUE(window->gameActive);
    // This test checks that handlePlayOButtonClick sets the human as O, AI as X,
    // and starts the game.
}

// Test for highlightWinningCells
TEST_F(GameWindowTest, HighlightWinningCells) {
    std::vector<std::pair<int, int>> winCells = {{0, 0}, {0, 1}, {0, 2}};
    window->highlightWinningCells(winCells);
    for (const auto& [row, col] : winCells) {
        QString style = window->cells[row][col]->styleSheet();
        EXPECT_TRUE(style.contains("background-color: #58d68d"));
        EXPECT_TRUE(style.contains("color: white"));
        EXPECT_TRUE(style.contains("border: 1px solid #48c97d"));
        EXPECT_NE(window->cells[row][col]->graphicsEffect(), nullptr); // Should have an effect
    }
    // Check that non-winning cells are unchanged
    EXPECT_FALSE(window->cells[1][0]->styleSheet().contains("#58d68d"));
    // This test ensures that highlightWinningCells applies the correct style and effect
    // to winning cells only.
}

// Test for gameOver
TEST_F(GameWindowTest, GameOver) {
    // Case: Human wins in PvAI
    WinInfo winInfo = {Player::X, {{0, 0}, {0, 1}, {0, 2}}};
    window->gameMode = GameMode::PvAI;
    window->humanPlayer = Player::X;
    window->gameOver(winInfo);
    EXPECT_FALSE(window->gameActive);
    EXPECT_EQ(window->statusLabel->text(), QString("🏆 Congratulations! You won! 🎉"));
    QString style = window->statusLabel->styleSheet();
    EXPECT_TRUE(style.contains("background-color: #58d68d"));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_FALSE(window->cells[i][j]->isEnabled()); // Board should be disabled
        }
    }

    // Case: AI wins in PvAI
    winInfo.winner = Player::O;
    window->humanPlayer = Player::X;
    window->gameOver(winInfo);
    EXPECT_EQ(window->statusLabel->text(), QString("AI wins! Better luck next time!"));
    style = window->statusLabel->styleSheet();
    EXPECT_TRUE(style.contains("background-color: #e74c3c"));

    // Case: Draw
    winInfo.winner = Player::None;
    winInfo.winCells.clear();
    window->gameOver(winInfo);
    EXPECT_EQ(window->statusLabel->text(), QString("It's a draw! 🤝"));
    style = window->statusLabel->styleSheet();
    EXPECT_TRUE(style.contains("background-color: #f4d03f"));
    // This test verifies that gameOver handles win, loss, and draw cases correctly,
    // updating the status label and disabling the board.
}

// Test for startNewGame
TEST_F(GameWindowTest, StartNewGame) {
    // Case: PvP mode
    window->gameMode = GameMode::PvP;
    window->startNewGame();
    EXPECT_TRUE(window->gameActive);
    EXPECT_EQ(window->currentPlayer, Player::X);
    EXPECT_EQ(window->statusLabel->text(), QString("Game started - Player X's turn!"));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_TRUE(window->cells[i][j]->isEnabled());
            EXPECT_TRUE(window->cells[i][j]->text().isEmpty());
            EXPECT_FALSE(window->cells[i][j]->styleSheet().contains("#58d68d")); // No winning cells
        }
    }

    // Case: PvAI with AI starting
    window->gameMode = GameMode::PvAI;
    window->humanPlayer = Player::O;
    window->aiPlayer = Player::X;
    window->startNewGame();
    EXPECT_TRUE(window->gameActive);
    EXPECT_EQ(window->statusLabel->text(), QString("AI (X) is thinking..."));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_FALSE(window->cells[i][j]->isEnabled()); // Board disabled during AI's turn
        }
    }

    // Case: PvAI with human starting
    window->humanPlayer = Player::X;
    window->aiPlayer = Player::O;
    window->startNewGame();
    EXPECT_TRUE(window->gameActive);
    EXPECT_EQ(window->statusLabel->text(), QString("Game started - Your turn!"));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            EXPECT_TRUE(window->cells[i][j]->isEnabled());
        }
    }
    // This test checks that startNewGame resets the game, sets the correct player,
    // and enables/disables the board based on the game mode.
}

// Test for handleCellClick
TEST_F(GameWindowTest, HandleCellClick) {
    // Case: PvP, successful move
    window->gameMode = GameMode::PvP;
    window->currentPlayer = Player::X;
    window->gameActive = true;
    window->cells[0][0]->setEnabled(true);
    QTest::mouseClick(window->cells[0][0], Qt::LeftButton);
    EXPECT_EQ(window->cells[0][0]->text(), QString("X"));
    EXPECT_FALSE(window->cells[0][0]->isEnabled());
    EXPECT_EQ(window->currentPlayer, Player::O);
    EXPECT_EQ(window->statusLabel->text(), QString("Player O's turn"));

    // Case: Game not active
    window->gameActive = false;
    window->cells[0][1]->setEnabled(true);
    QTest::mouseClick(window->cells[0][1], Qt::LeftButton);
    EXPECT_TRUE(window->cells[0][1]->text().isEmpty()); // No symbol should be set

    // Case: Disabled cell
    window->gameActive = true;
    window->cells[0][2]->setEnabled(false);
    QTest::mouseClick(window->cells[0][2], Qt::LeftButton);
    EXPECT_TRUE(window->cells[0][2]->text().isEmpty()); // No symbol should be set

    // Case: PvAI, human move
    window->gameMode = GameMode::PvAI;
    window->humanPlayer = Player::X;
    window->currentPlayer = Player::X;
    window->gameActive = true;
    window->cells[1][0]->setEnabled(true);
    QTest::mouseClick(window->cells[1][0], Qt::LeftButton);
    EXPECT_EQ(window->cells[1][0]->text(), QString("X"));
    EXPECT_FALSE(window->cells[1][0]->isEnabled());
    EXPECT_EQ(window->currentPlayer, Player::O);
    EXPECT_EQ(window->statusLabel->text(), QString("AI is thinking..."));
    // This test verifies that handleCellClick handles player moves in PvP and PvAI modes,
    // ignores invalid clicks, and updates the game state correctly.
}

// Test for makeAIMove
TEST_F(GameWindowTest, MakeAIMove) {
    window->gameMode = GameMode::PvAI;
    window->humanPlayer = Player::O;
    window->aiPlayer = Player::X;
    window->currentPlayer = Player::X;
    window->gameActive = true;
    window->cells[0][0]->setEnabled(true);
    window->makeAIMove();
    EXPECT_EQ(window->cells[0][0]->text(), QString("X"));
    EXPECT_FALSE(window->cells[0][0]->isEnabled());
    EXPECT_EQ(window->currentPlayer, Player::O);
    EXPECT_EQ(window->statusLabel->text(), QString("Your turn!"));
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (i == 0 && j == 0) continue;
            EXPECT_TRUE(window->cells[i][j]->isEnabled()); // Other cells should be enabled
        }
    }

    // Case: Game not active
    window->gameActive = false;
    window->cells[0][1]->setEnabled(true);
    window->makeAIMove();
    EXPECT_TRUE(window->cells[0][1]->text().isEmpty()); // No symbol should be set
    // This test checks that makeAIMove places the AI's symbol, updates the game state,
    // and ignores moves when the game is not active.
}

// Test for animateCell
TEST_F(GameWindowTest, AnimateCell) {
    QPushButton* cell = window->cells[0][0];
    window->animateCell(cell, "X");
    EXPECT_EQ(cell->text(), QString("X"));
    QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(cell->graphicsEffect());
    ASSERT_NE(effect, nullptr);
    EXPECT_EQ(effect->opacity(), 0.0); // Starts transparent
    QTest::qWait(350); // Wait for animation to finish (300ms + margin)
    EXPECT_EQ(effect->opacity(), 1.0); // Ends fully opaque
    // This test verifies that animateCell sets the cell's text and applies a fade-in animation.
}

// Test for enableBoard
TEST_F(GameWindowTest, EnableBoard) {
    // Enable the board
    window->cells[0][0]->setText("");
    window->cells[0][1]->setText("X");
    window->enableBoard(true);
    EXPECT_TRUE(window->cells[0][0]->isEnabled()); // Empty cell should be enabled
    EXPECT_FALSE(window->cells[0][1]->isEnabled()); // Used cell should be disabled

    // Disable the board
    window->enableBoard(false);
    EXPECT_FALSE(window->cells[0][0]->isEnabled());
    EXPECT_FALSE(window->cells[0][1]->isEnabled());
    // This test checks that enableBoard enables empty cells and disables used cells
    // when enabling, and disables all cells when disabling.
}