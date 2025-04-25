#include "GameWindow.h"
#include <QVBoxLayout>
#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QFrame>

GameWindow::GameWindow(QWidget* parent) : QMainWindow(parent), gameActive(false) {
    setupUI();
    choosePlayer();
}

void GameWindow::setupUI() {
    // Set window properties
    setWindowTitle("Tic-Tac-Toe");
    setStyleSheet("QMainWindow { background-color: #e8eff1; }"); // Slightly cooler background

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    
    // Create and style the status label
    statusLabel = new QLabel("Welcome to Tic-Tac-Toe!");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;" // Updated font
        "    font-size: 22px;" // Slightly smaller
        "    color: #34495e;" // Darker blue-grey
        "    padding: 12px 20px;" // Adjust padding
        "    border-radius: 8px;" // Slightly smaller radius
        "    background-color: white;"
        "    border: 1px solid #dce4e8;" // Lighter border
        "    qproperty-alignment: 'AlignCenter';" // Ensure alignment via stylesheet too
        "}"
    );
    mainLayout->addWidget(statusLabel);

    // Create a frame for the game board
    QFrame* boardFrame = new QFrame();
    boardFrame->setFrameStyle(QFrame::NoFrame); // Remove default frame style
    boardFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #ffffff;" // Keep white background
        "    border-radius: 10px;" // Slightly smaller radius
        "    border: 1px solid #dce4e8;" // Lighter border matching label
        "}"
    );

    QGridLayout* gridLayout = new QGridLayout(boardFrame);
    gridLayout->setSpacing(10);
    gridLayout->setContentsMargins(15, 15, 15, 15);

    QString cellStyle = 
        "qpushbutton {"
        "    font-family: 'segoe ui', sans-serif;" // updated font
        "    font-size: 52px;" // slightly larger font
        "    font-weight: bold;"
        "    color: #34495e;" // match status label text
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #fdfefe, stop:1 #e8eff1);" // subtle gradient
        "    border: 1px solid #dce4e8;" // lighter border
        "    border-radius: 8px;" // match label radius
        "}"
        "qpushbutton:hover {"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #ffffff, stop:1 #e0e9ec);" // lighter hover
        "    border-color: #c8d0d4;"
        "}"
        "qpushbutton:pressed {"
        "    background-color: #e0e9ec;" // simple pressed state
        "    border-color: #b0b8bc;"
        "}"
        "qpushbutton:disabled {"
        "    color: #95a5a6;" // greyed out color
        "    background-color: #f4f6f7;" // flat disabled background
        "}";

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cells[i][j] = new QPushButton();
            cells[i][j]->setFixedSize(120, 120);
            cells[i][j]->setStyleSheet(cellStyle);
            cells[i][j]->setCursor(Qt::PointingHandCursor);
            
            // Add fade-in animation
            QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(cells[i][j]);
            cells[i][j]->setGraphicsEffect(effect);
            
            QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
            animation->setDuration(500);
            animation->setStartValue(0.0);
            animation->setEndValue(1.0);
            animation->start();

            connect(cells[i][j], &QPushButton::clicked, this, &GameWindow::handleCellClick);
            gridLayout->addWidget(cells[i][j], i, j);
        }
    }

    mainLayout->addWidget(boardFrame);

    // Style the New Game button
    newGameButton = new QPushButton("New Game");
    newGameButton->setStyleSheet(
        "QPushButton {"
        "    font-family: 'Segoe UI', sans-serif;" // Updated font
        "    font-size: 16px;" // Slightly smaller
        "    font-weight: bold;"
        "    color: white;"
        "    background-color: #5dade2;" // Slightly softer blue
        "    border: none;"
        "    border-radius: 8px;" // Match other elements
        "    padding: 12px 25px;" // Adjust padding
        "    min-width: 140px;" // Adjust min-width
        "    /* Subtle shadow effect using border */"
        "    border-bottom: 3px solid #4a9fcc;"
        "}"
        "QPushButton:hover {"
        "    background-color: #4a9fcc;" // Darker shade for hover
        "    border-bottom-color: #3a8db8;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3a8db8;" // Even darker for pressed
        "    border-bottom-width: 1px;" // Make shadow smaller when pressed
        "    margin-top: 2px;" // Simulate button moving down
        "}"
    );
    newGameButton->setCursor(Qt::PointingHandCursor);
    connect(newGameButton, &QPushButton::clicked, this, &GameWindow::startNewGame);
    
    mainLayout->addWidget(newGameButton, 0, Qt::AlignCenter);

    // Set fixed window size
    setFixedSize(500, 650);
}

void GameWindow::choosePlayer() {
    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Choose Player");
    msgBox.setText("Choose your player:");
    msgBox.setIcon(QMessageBox::Question);
    
    QPushButton* xButton = msgBox.addButton("Play as X", QMessageBox::YesRole);
    QPushButton* oButton = msgBox.addButton("Play as O", QMessageBox::NoRole);
    
    // Style the message box
    msgBox.setStyleSheet(
        "QMessageBox {"
        "    background-color: white;"
        "}"
        "QMessageBox QLabel {"
        "    color: #2c3e50;"
        "    font-size: 16px;"
        "    min-width: 200px;"
        "}"
        "QPushButton {"
        "    font-size: 14px;"
        "    padding: 8px 16px;"
        "    border-radius: 5px;"
        "    background-color: #3498db;"
        "    color: white;"
        "    border: none;"
        "    min-width: 100px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "}"
    );
    
    msgBox.exec();

    if (msgBox.clickedButton() == xButton) {
        humanPlayer = Player::X;
        aiPlayer = Player::O;
    } else {
        humanPlayer = Player::O;
        aiPlayer = Player::X;
    }

    startNewGame();
}

// Update the highlightWinningCells method to add animation
void GameWindow::highlightWinningCells(const std::vector<std::pair<int, int>>& winCells) {
    QString winningStyle =
        "QPushButton {"
        "    background-color: #58d68d;" // Vibrant green
        "    color: white;"
        "    border: 1px solid #48c97d;" // Matching border
        "}"; // Keep other properties like font, radius from base style

    for (const auto& [row, col] : winCells) {
        cells[row][col]->setStyleSheet(winningStyle);
        
        // Add pulsing animation
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(cells[row][col]);
        cells[row][col]->setGraphicsEffect(effect);
        
        QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
        animation->setDuration(1000);
        animation->setStartValue(1.0);
        animation->setEndValue(0.5);
        animation->setLoopCount(-1); // Infinite loop
        animation->start();
    }
}

// Modified gameOver method to include animations
void GameWindow::gameOver(const WinInfo& result) {
    gameActive = false;
    
    QString resultStyle;
    if (result.winner != Player::None) {
        highlightWinningCells(result.winCells);
        if (result.winner == humanPlayer) {
            statusLabel->setStyleSheet(
                "QLabel {"
                "    font-family: 'Segoe UI', sans-serif;"
                "    font-size: 22px;"
                "    color: white;"
                "    padding: 12px 20px;"
                "    border-radius: 8px;"
                "    background-color: #58d68d;" // Match winning cells
                "    border: 1px solid #48c97d;"
                "    qproperty-alignment: 'AlignCenter';"
                "}"
            );
            statusLabel->setText("🏆 Congratulations! You won! 🎉");
        } else {
            statusLabel->setStyleSheet(
                "QLabel {"
                "    font-family: 'Segoe UI', sans-serif;"
                "    font-size: 22px;"
                "    color: white;"
                "    padding: 12px 20px;"
                "    border-radius: 8px;"
                "    background-color: #e74c3c;" // Keep red for loss
                "    border: 1px solid #c0392b;"
                "    qproperty-alignment: 'AlignCenter';"
                "}"
            );
            statusLabel->setText("AI wins! Better luck next time!");
        }
    } else {
        statusLabel->setStyleSheet(
            "QLabel {"
            "    font-family: 'Segoe UI', sans-serif;"
            "    font-size: 22px;"
            "    color: #34495e;" // Dark text for yellow background
            "    padding: 12px 20px;"
            "    border-radius: 8px;"
            "    background-color: #f4d03f;" // Brighter yellow
            "    border: 1px solid #f1c40f;"
            "    qproperty-alignment: 'AlignCenter';"
            "}"
        );
        statusLabel->setText("It's a draw! 🤝");
    }
}

void GameWindow::startNewGame() {
    // Reset the game board
    board.reset();
    gameActive = true;
    currentPlayer = Player::X;  // X always starts first

    // Reset all cells
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cells[i][j]->setText("");
            cells[i][j]->setEnabled(true);
            cells[i][j]->setStyleSheet(
                "QPushButton {"
                "    font-family: 'Arial';"
                "    font-size: 48px;"
                "    font-weight: bold;"
                "    color: #2c3e50;"
                "    background-color: #ecf0f1;"
                "    border: 2px solid #bdc3c7;"
                "    border-radius: 10px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #d5dbdb;"
                "}"
                "QPushButton:pressed {"
                "    background-color: #bdc3c7;"
                "}"
                "QPushButton:disabled {"
                "    color: #7f8c8d;"
                "    background-color: #ecf0f1;"
                "}"
            );
            
            // Add fade-in animation for new game
            QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(cells[i][j]);
            cells[i][j]->setGraphicsEffect(effect);
            
            QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
            animation->setDuration(500);
            animation->setStartValue(0.0);
            animation->setEndValue(1.0);
            animation->start();
        }
    }

    // Reset status label
    statusLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Arial';"
        "    font-size: 24px;"
        "    color: #2c3e50;"
        "    padding: 10px;"
        "    border-radius: 10px;"
        "    background-color: white;"
        "    border: 2px solid #bdc3c7;"
        "}"
    );
    statusLabel->setText("Game started - " + QString(humanPlayer == Player::X ? "Your" : "AI's") + " turn!");

    // If AI starts first (is X), make its move
    if (currentPlayer == aiPlayer) {
        QTimer::singleShot(500, this, &GameWindow::makeAIMove);
    }
}

void GameWindow::handleCellClick() {
    if (!gameActive) return;

    // Get the clicked button
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    if (!clickedButton || !clickedButton->text().isEmpty()) return;

    // Find the position of the clicked button
    int row = -1, col = -1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (cells[i][j] == clickedButton) {
                row = i;
                col = j;
                break;
            }
        }
        if (row != -1) break;
    }

    // Make the human move
    if (board.makeMove(row, col, humanPlayer)) {
        animateCell(clickedButton, QString(playerToChar(humanPlayer)));
        clickedButton->setEnabled(false);

        // Check if game is over after human move
        if (board.isGameOver()) {
            gameOver(board.checkWinner());
            return;
        }

        // Switch to AI's turn
        currentPlayer = aiPlayer;
        statusLabel->setText("AI is thinking...");

        // Make AI move after a short delay
        QTimer::singleShot(500, this, &GameWindow::makeAIMove);
    }
}

// Helper methods that need to be implemented as well:

void GameWindow::makeAIMove() {
    if (!gameActive) return;

    // Get AI's move
    auto [row, col] = findBestMove(board, aiPlayer);
    
    // Make the move
    if (board.makeMove(row, col, aiPlayer)) {
        animateCell(cells[row][col], QString(playerToChar(aiPlayer)));
        cells[row][col]->setEnabled(false);

        // Check if game is over after AI move
        if (board.isGameOver()) {
            gameOver(board.checkWinner());
            return;
        }

        // Switch back to human's turn
        currentPlayer = humanPlayer;
        statusLabel->setText("Your turn!");
    }
}

void GameWindow::animateCell(QPushButton* cell, const QString& symbol) {
    // First, make the cell invisible
    QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(cell);
    cell->setGraphicsEffect(effect);
    effect->setOpacity(0);

    // Set the symbol
    cell->setText(symbol);

    // Create and start the fade-in animation
    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(300);
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameWindow::enableBoard(bool enable) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (cells[i][j]->text().isEmpty()) {
                cells[i][j]->setEnabled(enable);
            }
        }
    }
}

