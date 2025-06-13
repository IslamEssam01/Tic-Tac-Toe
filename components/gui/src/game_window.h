#ifndef GAME_WINDOW_H
#define GAME_WINDOW_H

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
#include "game_history.h"

// Define game modes
enum class GameMode { PvP, PvAI };

class GameWindow : public QMainWindow {
    Q_OBJECT

public:
    GameWindow(QWidget* parent = nullptr);
    void setGameHistory(GameHistory* history); // Set the game history instance
    void setCurrentUser(const QString& username); // Set current user for history tracking

signals:
    void setupUIShown();
    void gameBoardUIShown();
    void logoutRequested(); // New signal for logout
    void secondPlayerAuthenticationRequested();
    void symbolSelectionRequested();
    void viewHistoryRequested(); // New signal for viewing game history
    void playerUsernameRegistered(const QString& username); // Signal for username mapping

public slots:
    void setPlayerNames(const QString& player1, const QString& player2);
    void resetGameState(); // Reset all game state to initial setup

private slots:
    void handleCellClick();
    void startNewGame();
    void handlePvpButtonClick();
    void handlePvaiButtonClick();
    void handlePlayXButtonClick();
    void handlePlayOButtonClick();
    void handlePlayer1XButtonClick();
    void handlePlayer1OButtonClick();


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
    void showSymbolSelectionUI(); // Helper to show symbol selection for PvP
    void showGameBoardUI(); // Helper to show the main game board
    void notifyUsernameMapping(const QString& username); // Helper to notify about username mappings

    QPushButton* cells[3][3];
    QPushButton* newGameButton;
    QLabel* statusLabel;
    // Add buttons for setup
    QPushButton* pvpButton;
    QPushButton* pvaiButton;
    QPushButton* playXButton;
    QPushButton* playOButton;
    QPushButton* player1XButton;
    QPushButton* player1OButton;
    QFrame* boardWidget; // Container for the board grid (Changed from QWidget*)
    QWidget* setupWidget; // Container for setup buttons
    QWidget* gameWidget; // Container for status label and board widget
    QWidget* symbolSelectionWidget; // Container for PvP symbol selection

    Board board;
    GameHistory* gameHistory; // Game history backend
    int currentGameId; // Current game ID being played
    Player humanPlayer;
    Player aiPlayer;
    Player currentPlayer;
    bool gameActive;
    GameMode gameMode;
    QString player1Name;
    QString player2Name;
    Player player1Symbol;
    Player player2Symbol;
    QString m_currentUser; // Current user name for history tracking
};

#endif
