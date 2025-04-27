#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include "Board.h"
#include "AI.h"

// Define game modes
enum class GameMode { PvP, PvAI };

class GameWindow : public QMainWindow {
    Q_OBJECT

public:
    GameWindow(QWidget* parent = nullptr);

signals:
    void setupUIShown();
    void gameBoardUIShown();
    void logoutRequested(); // New signal for logout

private slots:
    void handleCellClick();
    void startNewGame();
    // Remove choosePlayer and chooseGameMode slots
    void handlePvpButtonClick();
    void handlePvaiButtonClick();
    void handlePlayXButtonClick();
    void handlePlayOButtonClick();


private:
    void setupUI();
    void updateBoard();
    void makeAIMove();
    void gameOver(const WinInfo& result);
    void highlightWinningCells(const std::vector<std::pair<int, int>>& cells);
    void enableBoard(bool enable);
    void animateCell(QPushButton* cell, const QString& symbol);
    void showGameSetupUI(); // Helper to show initial setup
    void showPlayerChoiceUI(); // Helper to show player choice for PvAI
    void showGameBoardUI(); // Helper to show the main game board

    QPushButton* cells[3][3];
    QPushButton* newGameButton;
    QLabel* statusLabel;
    // Add buttons for setup
    QPushButton* pvpButton;
    QPushButton* pvaiButton;
    QPushButton* playXButton;
    QPushButton* playOButton;
    QFrame* boardWidget; // Container for the board grid (Changed from QWidget*)
    QWidget* setupWidget; // Container for setup buttons
    QWidget* gameWidget; // Container for status label and board widget

    Board board;
    Player humanPlayer;
    Player aiPlayer;
    Player currentPlayer;
    bool gameActive;
    GameMode gameMode;
};

#endif
