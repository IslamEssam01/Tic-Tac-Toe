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

class GameWindow : public QMainWindow {
    Q_OBJECT

public:
    GameWindow(QWidget* parent = nullptr);

private slots:
    void handleCellClick();
    void startNewGame();
    void choosePlayer();

private:
    void setupUI();
    void updateBoard();
    void makeAIMove();
    void gameOver(const WinInfo& result);
    void highlightWinningCells(const std::vector<std::pair<int, int>>& cells);
    void enableBoard(bool enable);
    void animateCell(QPushButton* cell, const QString& symbol);

    QPushButton* cells[3][3];
    QPushButton* newGameButton;
    QLabel* statusLabel;
    Board board;
    Player humanPlayer;
    Player aiPlayer;
    Player currentPlayer;
    bool gameActive;
};

#endif
