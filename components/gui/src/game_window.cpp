#include "game_window.h"
#include "ui_constants.h"
#include <QVBoxLayout>
#include <QHBoxLayout> // Added for horizontal button layouts
#include <QWidget>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QFrame>
#include <QTimer> // Ensure QTimer is included

GameWindow::GameWindow(QWidget* parent) : QMainWindow(parent), gameActive(false), gameHistory(nullptr), currentGameId(-1) {
    setupUI();
    // Don't call chooseGameMode here, show setup UI instead
    showGameSetupUI(); 
}

void GameWindow::setupUI() {
    // Set window properties
    setWindowTitle("Tic-Tac-Toe");
    setStyleSheet("QMainWindow { background-color: #e8eff1; }"); // Slightly cooler background

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    // Create a logout button with an elegant style
    QPushButton* logoutButton = new QPushButton("Logout", this);
    logoutButton->setStyleSheet(
        "QPushButton {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background-color: #e74c3c;"  // Red color
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    margin: 10px;"
        "    border-bottom: 2px solid #c0392b;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "    border-bottom-color: #a93226;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #a93226;"
        "    border-bottom-width: 1px;"
        "    margin-top: 11px;"
        "}"
    );
    logoutButton->setCursor(Qt::PointingHandCursor);
    logoutButton->setFixedWidth(100);
    
    // Create a View History button with an elegant style
    QPushButton* historyButton = new QPushButton("View History", this);
    historyButton->setStyleSheet(
        "QPushButton {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background-color: #3498db;"  // Blue color
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    margin: 10px;"
        "    border-bottom: 2px solid #2980b9;"
        "}"
        "QPushButton:hover {"
        "    background-color: #2980b9;"
        "    border-bottom-color: #2471a3;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #2471a3;"
        "    border-bottom-width: 1px;"
        "    margin-top: 11px;"
        "}"
    );
    historyButton->setCursor(Qt::PointingHandCursor);
    historyButton->setFixedWidth(150);
    
    // Create a container for the buttons that spans the whole width
    QWidget* topBar = new QWidget(this);
    QHBoxLayout* topBarLayout = new QHBoxLayout(topBar);
    topBarLayout->setContentsMargins(0, 0, 0, 0);
    topBarLayout->addStretch(); // Push buttons to the right
    topBarLayout->addWidget(historyButton);
    topBarLayout->addWidget(logoutButton);

    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(UIConstants::Spacing::GAME_ELEMENT_SPACING);
    mainLayout->setContentsMargins(UIConstants::Spacing::GAME_ELEMENT_SPACING, 
                                  UIConstants::Spacing::GAME_TOP_MARGIN, 
                                  UIConstants::Spacing::GAME_ELEMENT_SPACING, 
                                  UIConstants::Spacing::GAME_BOTTOM_MARGIN); // Reduced top margin
    
    // Add the top bar first
    mainLayout->addWidget(topBar);
    
    // Connect logout button
    connect(logoutButton, &QPushButton::clicked, this, [this]() {
        emit logoutRequested();
    });
    
    // Connect history button
    connect(historyButton, &QPushButton::clicked, this, [this]() {
        emit viewHistoryRequested();
    });
    
    // Create and style the status label
    statusLabel = new QLabel("Welcome to Tic-Tac-Toe!");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;" // Updated font
        "    font-size: " + QString::number(UIConstants::Font::STATUS_LABEL_SIZE) + "px;" // Slightly smaller
        "    color: #34495e;" // Darker blue-grey
        "    padding: " + QString::number(UIConstants::Padding::STATUS_VERTICAL) + "px " + QString::number(UIConstants::Padding::STATUS_HORIZONTAL) + "px;" // Adjust padding
        "    border-radius: " + QString::number(UIConstants::Style::BORDER_RADIUS) + "px;" // Slightly smaller radius
        "    background-color: white;"
        "    border: 1px solid #dce4e8;" // Lighter border
        "    qproperty-alignment: 'AlignCenter';" // Ensure alignment via stylesheet too
        "}"
    );
    // mainLayout->addWidget(statusLabel); // Removed from main layout

    // Create a container widget for setup buttons
    setupWidget = new QWidget();
    QVBoxLayout* setupLayout = new QVBoxLayout(setupWidget);
    setupLayout->setSpacing(UIConstants::Spacing::GAME_SETUP_SPACING);
    setupLayout->setAlignment(Qt::AlignCenter);

    // --- Game Mode Buttons ---
    QLabel* modeLabel = new QLabel("Select Game Mode:");
    modeLabel->setAlignment(Qt::AlignCenter);
    modeLabel->setStyleSheet("font-size: 18px; color: #34495e; margin-bottom: 10px;");
    setupLayout->addWidget(modeLabel);

    QHBoxLayout* modeButtonLayout = new QHBoxLayout();
    modeButtonLayout->setSpacing(UIConstants::Spacing::GAME_BUTTON_SPACING);
    pvpButton = new QPushButton("Player vs Player");
    pvaiButton = new QPushButton("Player vs AI");
    // Apply styling similar to newGameButton but maybe different colors
    QString setupButtonStyle = 
        "QPushButton {"
        "    font-family: 'Segoe UI', sans-serif; font-size: " + QString::number(UIConstants::Font::BUTTON_SIZE) + "px; font-weight: bold;"
        "    color: white; background-color: #3498db; border: none;"
        "    border-radius: " + QString::number(UIConstants::Style::BORDER_RADIUS) + "px; padding: " + QString::number(UIConstants::Padding::BUTTON_VERTICAL) + "px " + QString::number(UIConstants::Padding::BUTTON_HORIZONTAL) + "px; min-width: " + QString::number(UIConstants::Button::SYMBOL_BUTTON_MIN_WIDTH) + "px;"
        "    border-bottom: " + QString::number(UIConstants::Style::BUTTON_BORDER_BOTTOM) + "px solid #2980b9;"
        "}"
        "QPushButton:hover { background-color: #2980b9; border-bottom-color: #2471a3; }"
        "QPushButton:pressed { background-color: #2471a3; border-bottom-width: 1px; margin-top: 2px; }";
    pvpButton->setStyleSheet(setupButtonStyle);
    pvaiButton->setStyleSheet(setupButtonStyle);
    pvpButton->setCursor(Qt::PointingHandCursor);
    pvaiButton->setCursor(Qt::PointingHandCursor);
    modeButtonLayout->addWidget(pvpButton);
    modeButtonLayout->addWidget(pvaiButton);
    setupLayout->addLayout(modeButtonLayout);

    // --- Player Choice Buttons (Initially Hidden) ---
     QLabel* playerLabel = new QLabel("Play As:");
    playerLabel->setAlignment(Qt::AlignCenter);
    playerLabel->setStyleSheet("font-size: 18px; color: #34495e; margin-top: 20px; margin-bottom: 10px;");
    setupLayout->addWidget(playerLabel);

    QHBoxLayout* playerButtonLayout = new QHBoxLayout();
    playerButtonLayout->setSpacing(UIConstants::Spacing::GAME_BUTTON_SPACING);
    playXButton = new QPushButton("Play as X");
    playOButton = new QPushButton("Play as O");
    // Need separate strings for styling as replace modifies the original
    QString playXStyle = setupButtonStyle; 
    QString playOStyle = setupButtonStyle;
    playXButton->setStyleSheet(playXStyle.replace("#3498db", "#e67e22").replace("#2980b9", "#d35400").replace("#2471a3", "#b84c00")); // Orange theme for player choice
    playOButton->setStyleSheet(playOStyle.replace("#3498db", "#e67e22").replace("#2980b9", "#d35400").replace("#2471a3", "#b84c00")); // Orange theme for player choice
    playXButton->setCursor(Qt::PointingHandCursor);
    playOButton->setCursor(Qt::PointingHandCursor);
    playerButtonLayout->addWidget(playXButton);
    playerButtonLayout->addWidget(playOButton);
    setupLayout->addLayout(playerButtonLayout);

    mainLayout->addWidget(setupWidget);

    // Create symbol selection widget for PvP
    symbolSelectionWidget = new QWidget();
    QVBoxLayout* symbolLayout = new QVBoxLayout(symbolSelectionWidget);
    symbolLayout->setSpacing(15);
    symbolLayout->setAlignment(Qt::AlignCenter);

    QLabel* symbolLabel = new QLabel("Symbol Choice:");
    symbolLabel->setAlignment(Qt::AlignCenter);
    symbolLabel->setStyleSheet("font-size: 18px; color: #34495e; margin-bottom: 10px;");
    symbolLayout->addWidget(symbolLabel);

    QHBoxLayout* symbolButtonLayout = new QHBoxLayout();
    symbolButtonLayout->setSpacing(10);
    player1XButton = new QPushButton("Play as X");
    player1OButton = new QPushButton("Play as O");
    QString symbolButtonStyle = 
        "QPushButton {"
        "    font-family: 'Segoe UI', sans-serif; font-size: 16px; font-weight: bold;"
        "    color: white; background-color: #27ae60; border: none;"
        "    border-radius: 8px; padding: 10px 20px; min-width: 150px;"
        "    border-bottom: 3px solid #229954;"
        "}"
        "QPushButton:hover { background-color: #229954; border-bottom-color: #1e8449; }"
        "QPushButton:pressed { background-color: #1e8449; border-bottom-width: 1px; margin-top: 2px; }";
    player1XButton->setStyleSheet(symbolButtonStyle);
    player1OButton->setStyleSheet(symbolButtonStyle);
    player1XButton->setCursor(Qt::PointingHandCursor);
    player1OButton->setCursor(Qt::PointingHandCursor);
    symbolButtonLayout->addWidget(player1XButton);
    symbolButtonLayout->addWidget(player1OButton);
    symbolLayout->addLayout(symbolButtonLayout);

    mainLayout->addWidget(symbolSelectionWidget);
    symbolSelectionWidget->setVisible(false); // Initially hidden


    // Create a frame for the game board (initially hidden)
    boardWidget = new QFrame(); // Use boardWidget instead of boardFrame directly
    boardWidget->setFrameStyle(QFrame::NoFrame); // Remove default frame style
    boardWidget->setStyleSheet(
        "QFrame {"
        "    background-color: #ffffff;" // Keep white background
        "    border-radius: 10px;" // Slightly smaller radius
        "    border: 1px solid #dce4e8;" // Lighter border matching label
        "}"
    );

    QGridLayout* gridLayout = new QGridLayout(boardWidget); // Add gridLayout to boardWidget
    gridLayout->setSpacing(UIConstants::GameBoard::BOARD_SPACING);
    gridLayout->setContentsMargins(UIConstants::GameBoard::BOARD_MARGIN, UIConstants::GameBoard::BOARD_MARGIN, UIConstants::GameBoard::BOARD_MARGIN, UIConstants::GameBoard::BOARD_MARGIN);

    QString cellStyle = 
        "QPushButton {" // Corrected from qpushbutton
        "    font-family: 'segoe ui', sans-serif;" // updated font
        "    font-size: " + QString::number(UIConstants::Font::CELL_TEXT_SIZE) + "px;" // slightly larger font
        "    font-weight: bold;"
        "    color: #34495e;" // match status label text
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #fdfefe, stop:1 #e8eff1);" // subtle gradient
        "    border: 1px solid #dce4e8;" // lighter border
        "    border-radius: " + QString::number(UIConstants::Style::BORDER_RADIUS) + "px;" // match label radius
        "    outline: none;" // Remove focus outline
        "}"
        "QPushButton:focus {" // Remove focus rectangle/border
        "    outline: none;"
        "    border: 1px solid #dce4e8;" // Keep the same border when focused
        "}"
        "QPushButton:hover {" // Corrected from qpushbutton
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #ffffff, stop:1 #e0e9ec);" // lighter hover
        "    border-color: #c8d0d4;"
        "}"
        "QPushButton:pressed {" // Corrected from qpushbutton
        "    background-color: #e0e9ec;" // simple pressed state
        "    border-color: #b0b8bc;"
        "}"
        "QPushButton:disabled {" // Corrected from qpushbutton
        "    color: #95a5a6;" // greyed out color
        "    background-color: #f4f6f7;" // flat disabled background
        "}";

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cells[i][j] = new QPushButton();
            cells[i][j]->setFixedSize(UIConstants::GameBoard::CELL_SIZE, UIConstants::GameBoard::CELL_SIZE);
            cells[i][j]->setStyleSheet(cellStyle);
            cells[i][j]->setCursor(Qt::PointingHandCursor);
            
            // Disable cells initially until game setup is complete
            cells[i][j]->setEnabled(false); 
            
            // Remove initial fade-in animation here; it will be done in startNewGame

            connect(cells[i][j], &QPushButton::clicked, this, &GameWindow::handleCellClick);
            gridLayout->addWidget(cells[i][j], i, j);
        }
    }

    // Create a container widget for the game view (status + board)
    gameWidget = new QWidget();
    QVBoxLayout* gameLayout = new QVBoxLayout(gameWidget);
    gameLayout->setSpacing(15); // Spacing between status and board
    gameLayout->setContentsMargins(0, 0, 0, 0); // No extra margins for this container
    gameLayout->addWidget(statusLabel); // Add status label here
    gameLayout->addWidget(boardWidget); // Add the board container widget

    mainLayout->addWidget(gameWidget); // Add the game container to the main layout

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
    connect(newGameButton, &QPushButton::clicked, this, &GameWindow::showGameSetupUI); // New Game restarts setup

    // Connect new setup buttons
    connect(pvpButton, &QPushButton::clicked, this, &GameWindow::handlePvpButtonClick);
    connect(pvaiButton, &QPushButton::clicked, this, &GameWindow::handlePvaiButtonClick);
    connect(playXButton, &QPushButton::clicked, this, &GameWindow::handlePlayXButtonClick);
    connect(playOButton, &QPushButton::clicked, this, &GameWindow::handlePlayOButtonClick);
    connect(player1XButton, &QPushButton::clicked, this, &GameWindow::handlePlayer1XButtonClick);
    connect(player1OButton, &QPushButton::clicked, this, &GameWindow::handlePlayer1OButtonClick);

    mainLayout->addWidget(newGameButton, 0, Qt::AlignCenter);

    // Set fixed window size
    setFixedSize(UIConstants::WindowSize::GAME_WIDTH, UIConstants::WindowSize::GAME_HEIGHT); // Adjusted size slightly for setup buttons
}

// Add implementations for new slots and helper functions

void GameWindow::showGameSetupUI() {
    statusLabel->setText("Choose a game mode:");
    // Reset styles if coming from a finished game
    statusLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;" 
        "    font-size: 22px;" 
        "    color: #34495e;" 
        "    padding: 12px 20px;" 
        "    border-radius: 8px;" 
        "    background-color: white;"
        "    border: 1px solid #dce4e8;" 
        "    qproperty-alignment: 'AlignCenter';" 
        "}"
    );
    setupWidget->setVisible(true);
    gameWidget->setVisible(false); // Hide the game container (status + board)
    symbolSelectionWidget->setVisible(false); // Hide symbol selection
    // boardWidget->setVisible(false); // No longer needed, handled by gameWidget
    pvpButton->setVisible(true);
    pvaiButton->setVisible(true);
    playXButton->setVisible(false); // Hide player choice initially
    playOButton->setVisible(false); // Hide player choice initially
    newGameButton->setVisible(false); // Hide New Game button during setup
    // Enable board cells are disabled when not visible
    enableBoard(false); 
    gameActive = false; // Ensure game is not active during setup
    setFixedSize(UIConstants::WindowSize::SETUP_WIDTH, UIConstants::WindowSize::SETUP_HEIGHT); // Set smaller fixed size for setup
    
    // Emit signal that setup UI is shown
    emit setupUIShown();
}

void GameWindow::notifyUsernameMapping(const QString& username) {
    if (!username.isEmpty()) {
        emit playerUsernameRegistered(username);
    }
}

void GameWindow::showPlayerChoiceUI() {
    statusLabel->setText("Playing vs AI. Choose your symbol!");
    setupWidget->setVisible(true); // Keep setup container visible
    gameWidget->setVisible(false); // Hide game container
    // boardWidget->setVisible(false); // No longer needed
    pvpButton->setVisible(false); // Hide mode buttons
    pvaiButton->setVisible(false);
    playXButton->setVisible(true); // Show player choice
    playOButton->setVisible(true);
    setFixedSize(UIConstants::WindowSize::SETUP_WIDTH, UIConstants::WindowSize::SETUP_HEIGHT); // Set smaller fixed size for player choice
}

void GameWindow::setGameHistory(GameHistory* history) {
    gameHistory = history;
}

void GameWindow::setCurrentUser(const QString& username) {
    m_currentUser = username;
    
    // Notify about username mapping for game history
    emit playerUsernameRegistered(username);
}

void GameWindow::showGameBoardUI() {
    setupWidget->setVisible(false); // Hide setup buttons
    symbolSelectionWidget->setVisible(false); // Hide symbol selection
    gameWidget->setVisible(true); // Show game container (status + board)
    // boardWidget->setVisible(true); // No longer needed
    newGameButton->setVisible(true); // Show New Game button during gameplay
    startNewGame(); // Start the actual game logic
    setFixedSize(UIConstants::WindowSize::GAME_WIDTH, UIConstants::WindowSize::GAME_HEIGHT); // Set larger fixed size for game board
    
    // Emit signal that game board UI is shown
    emit gameBoardUIShown();
}

void GameWindow::handlePvpButtonClick() {
    gameMode = GameMode::PvP;
    statusLabel->setText("Player vs Player mode selected!");
    // Request second player authentication
    emit secondPlayerAuthenticationRequested();
}

void GameWindow::handlePvaiButtonClick() {
    gameMode = GameMode::PvAI;
    showPlayerChoiceUI(); // Show X/O choice buttons
}

void GameWindow::handlePlayXButtonClick() {
    humanPlayer = Player::X;
    aiPlayer = Player::O;
    statusLabel->setText("You are X. AI is O.");
    showGameBoardUI();
}

void GameWindow::handlePlayOButtonClick() {
    humanPlayer = Player::O;
    aiPlayer = Player::X;
    statusLabel->setText("You are O. AI is X.");
    showGameBoardUI();
}

void GameWindow::handlePlayer1XButtonClick() {
    player1Symbol = Player::X;
    player2Symbol = Player::O;
    statusLabel->setText(QString("%1 is X, %2 is O. Starting game!").arg(player1Name, player2Name));
    showGameBoardUI();
}

void GameWindow::handlePlayer1OButtonClick() {
    player1Symbol = Player::O;
    player2Symbol = Player::X;
    statusLabel->setText(QString("%1 is O, %2 is X. Starting game!").arg(player1Name, player2Name));
    showGameBoardUI();
}

void GameWindow::showSymbolSelectionUI() {
    statusLabel->setText(QString("%1, choose your symbol:").arg(player1Name));
    
    // Update button text to show actual player name
    player1XButton->setText(QString("%1 as X").arg(player1Name));
    player1OButton->setText(QString("%1 as O").arg(player1Name));
    
    setupWidget->setVisible(false);
    gameWidget->setVisible(false);
    symbolSelectionWidget->setVisible(true);
    newGameButton->setVisible(false);
    enableBoard(false);
    gameActive = false;
    setFixedSize(UIConstants::WindowSize::SETUP_WIDTH, UIConstants::WindowSize::SETUP_HEIGHT);
}

void GameWindow::setPlayerNames(const QString& player1, const QString& player2) {
    player1Name = player1;
    player2Name = player2;
    
    // Notify about username mappings for game history
    emit playerUsernameRegistered(player1);
    emit playerUsernameRegistered(player2);
    
    showSymbolSelectionUI();
}

void GameWindow::resetGameState() {
    // Reset all game state variables
    gameActive = false;
    currentPlayer = Player::X;
    gameMode = GameMode::PvP; // Default mode
    currentGameId = -1; // Reset game ID
    
    // Clear player information
    player1Name.clear();
    player2Name.clear();
    player1Symbol = Player::None;
    player2Symbol = Player::None;
    humanPlayer = Player::X;
    aiPlayer = Player::O;
    
    // Reset the board
    board.reset();
    
    // Reset all cell states
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cells[i][j]->setText("");
            cells[i][j]->setEnabled(false);
            
            // Remove any animations or effects
            if (cells[i][j]->graphicsEffect()) {
                delete cells[i][j]->graphicsEffect();
                cells[i][j]->setGraphicsEffect(nullptr);
            }
        }
    }
    
    // Show the initial setup UI
    showGameSetupUI();
}

// Update the highlightWinningCells method to add animation
void GameWindow::highlightWinningCells(const std::vector<std::pair<int, int>>& winCells) {
    QString winningStyle =
        "QPushButton {" // Corrected from qpushbutton
        "    background-color: #58d68d;" // Vibrant green
        "    color: white;"
        "    border: 1px solid #48c97d;" // Matching border
        "}"; // Keep other properties like font, radius from base style

    for (const auto& [row, col] : winCells) {
        // Combine base style with winning style (ensure font etc. are kept)
        QString currentStyle = cells[row][col]->styleSheet();
        // A simple approach: append winning style specifics. More robust might involve parsing.
        cells[row][col]->setStyleSheet(currentStyle + winningStyle); 
        
        // Ensure previous effects/animations are cleaned up if any
        if (cells[row][col]->graphicsEffect()) {
            delete cells[row][col]->graphicsEffect();
            cells[row][col]->setGraphicsEffect(nullptr); // Important to nullify pointer
        }

        // Add pulsing animation
        QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(cells[row][col]);
        cells[row][col]->setGraphicsEffect(effect);
        
        QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
        animation->setDuration(1000);
        animation->setStartValue(1.0);
        animation->setEndValue(0.5);
        animation->setLoopCount(-1); // Infinite loop
        // Ensure animation cleans itself up if the widget is destroyed or effect replaced
        animation->start(QAbstractAnimation::DeleteWhenStopped); 
    }
}

// Modified gameOver method to include animations
void GameWindow::gameOver(const WinInfo& result) {
    gameActive = false;
    enableBoard(false); // Disable board on game over
    
    // Record game result in history if available
    if (gameHistory && currentGameId > 0) {
        std::optional<int> winnerId = std::nullopt;
        
        if (result.winner != Player::None) {
            if (gameMode == GameMode::PvP) {
                // For PvP, determine winner based on player symbols
                if (result.winner == player1Symbol) {
                    winnerId = qHash(player1Name);
                } else {
                    winnerId = qHash(player2Name);
                }
            } else {
                // For PvAI
                if (result.winner == humanPlayer) {
                    winnerId = qHash(m_currentUser);
                } else {
                    winnerId = -2; // AI wins
                }
            }
        } else {
            winnerId = -1; // Draw
        }
        
        gameHistory->setWinner(currentGameId, winnerId);
    }
    
    QString resultStyleBase = 
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 22px;"
        "    padding: 12px 20px;"
        "    border-radius: 8px;"
        "    qproperty-alignment: 'AlignCenter';"
        "}";
    QString specificStyle;

    if (result.winner != Player::None) {
    highlightWinningCells(result.winCells);
    if (gameMode == GameMode::PvAI && result.winner == humanPlayer) {
        // Win style (Green) - Player wins against AI
        specificStyle = 
            "QLabel {"
            "    color: white;"
            "    background-color: #58d68d;" // Match winning cells
            "    border: 1px solid #48c97d;"
            "}";
        statusLabel->setText("🏆 Congratulations! You won! 🎉");
    } else if (gameMode == GameMode::PvAI && result.winner == aiPlayer) {
        // Loss style (Red) - AI wins
        specificStyle = 
            "QLabel {"
            "    color: white;"
            "    background-color: #e74c3c;" // Keep red for loss
            "    border: 1px solid #c0392b;"
            "}";
        statusLabel->setText("AI wins! Better luck next time!");
    } else if (gameMode == GameMode::PvP) {
        // PvP mode - show winner name
        QString winnerName = (result.winner == player1Symbol) ? player1Name : player2Name;
        specificStyle = 
            "QLabel {"
            "    color: white;"
            "    background-color: #58d68d;" // Green for any PvP win
            "    border: 1px solid #48c97d;"
            "}";
        statusLabel->setText(QString("🏆 %1 wins! 🎉").arg(winnerName));
    }
    } else {
        // Draw style (Yellow)
        specificStyle = 
            "QLabel {"
            "    color: #34495e;" // Dark text for yellow background
            "    background-color: #f4d03f;" // Brighter yellow
            "    border: 1px solid #f1c40f;"
            "}";
        statusLabel->setText("It's a draw! 🤝");
    }
    statusLabel->setStyleSheet(resultStyleBase + specificStyle);
}

void GameWindow::startNewGame() {
    // Reset the game board
    board.reset();
    // gameActive will be set after animations potentially
    currentPlayer = Player::X;  // X always starts first
    
    // Initialize game in history if available
    if (gameHistory) {
        std::optional<int> playerXId = std::nullopt;
        std::optional<int> playerOId = std::nullopt;
        
        if (gameMode == GameMode::PvP) {
            // For PvP, we need to map player names to IDs
            // For now, use simple hash of username as ID
            if (player1Symbol == Player::X) {
                playerXId = qHash(player1Name);
                playerOId = qHash(player2Name);
            } else {
                playerXId = qHash(player2Name);
                playerOId = qHash(player1Name);
            }
        } else {
            // For PvAI, one player is human, other is AI (nullopt)
            if (humanPlayer == Player::X) {
                playerXId = qHash(m_currentUser);
                playerOId = std::nullopt; // AI
            } else {
                playerXId = std::nullopt; // AI
                playerOId = qHash(m_currentUser);
            }
        }
        
        currentGameId = gameHistory->initializeGame(playerXId, playerOId);
    }

    // Reset all cells (use the base cell style defined in setupUI)
    QString cellStyle = 
        "QPushButton {" // Corrected from qpushbutton
        "    font-family: 'segoe ui', sans-serif;" 
        "    font-size: 52px;" 
        "    font-weight: bold;"
        "    color: #34495e;" 
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #fdfefe, stop:1 #e8eff1);" 
        "    border: 1px solid #dce4e8;" 
        "    border-radius: 8px;" 
        "    outline: none;" // Remove focus outline
        "}"
        "QPushButton:focus {" // Remove focus rectangle/border
        "    outline: none;"
        "    border: 1px solid #dce4e8;" // Keep the same border when focused
        "}"
        "QPushButton:hover {" // Corrected from qpushbutton
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #ffffff, stop:1 #e0e9ec);" 
        "    border-color: #c8d0d4;"
        "}"
        "QPushButton:pressed {" // Corrected from qpushbutton
        "    background-color: #e0e9ec;" 
        "    border-color: #b0b8bc;"
        "}"
        "QPushButton:disabled {" // Corrected from qpushbutton
        "    color: #95a5a6;" 
        "    background-color: #f4f6f7;" 
        "}";

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            cells[i][j]->setText("");
            cells[i][j]->setEnabled(true); // Enable cells for the new game
            cells[i][j]->setStyleSheet(cellStyle); // Apply base style
            
            // Ensure previous effects/animations are cleaned up if any
            if (cells[i][j]->graphicsEffect()) {
                delete cells[i][j]->graphicsEffect();
                cells[i][j]->setGraphicsEffect(nullptr); // Important to nullify pointer
            }

            // Add fade-in animation for new game
            QGraphicsOpacityEffect* effect = new QGraphicsOpacityEffect(cells[i][j]);
            cells[i][j]->setGraphicsEffect(effect);
            
            QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
            animation->setDuration(500);
            animation->setStartValue(0.0);
            animation->setEndValue(1.0);
            // Ensure animation cleans itself up
            animation->start(QAbstractAnimation::DeleteWhenStopped); 
        }
    }
    gameActive = true; // Activate game after setup and animations start

    // Reset status label to default style
    statusLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;" 
        "    font-size: 22px;" 
        "    color: #34495e;" 
        "    padding: 12px 20px;" 
        "    border-radius: 8px;" 
        "    background-color: white;"
        "    border: 1px solid #dce4e8;" 
        "    qproperty-alignment: 'AlignCenter';" 
        "}"
    );
    
    if (gameMode == GameMode::PvP) {
        QString currentPlayerName = (currentPlayer == player1Symbol) ? player1Name : player2Name;
        statusLabel->setText(QString("Game started - %1's turn (%2)!").arg(currentPlayerName, playerToChar(currentPlayer)));
        enableBoard(true); // Ensure board is enabled for PvP start
    } else { // PvAI mode
        // Determine who starts based on player choice
        statusLabel->setText("Game started - " + QString(currentPlayer == humanPlayer ? "Your" : "AI's") + " turn!");
        
        // If AI starts first (is X), make its move
        if (currentPlayer == aiPlayer) {
            enableBoard(false); // Disable board while AI thinks
            statusLabel->setText("AI (X) is thinking...");
            QTimer::singleShot(500, this, &GameWindow::makeAIMove);
        } else {
             enableBoard(true); // Ensure board is enabled if human starts
        }
    }
}

void GameWindow::handleCellClick() {
    if (!gameActive) return;

    // Get the clicked button
    QPushButton* clickedButton = qobject_cast<QPushButton*>(sender());
    // Check if button is valid, enabled, and empty
    if (!clickedButton || !clickedButton->isEnabled() || !clickedButton->text().isEmpty()) return; 

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

    // Determine the player making the move
    Player movePlayer = Player::None;
    if (gameMode == GameMode::PvP) {
        movePlayer = currentPlayer;
    } else { // PvAI
        if (currentPlayer == humanPlayer) {
            movePlayer = humanPlayer;
        } else {
            // Should not happen if board is correctly disabled during AI turn
            return; 
        }
    }

    // Make the move
    if (board.makeMove(row, col, movePlayer)) {
        animateCell(clickedButton, QString(playerToChar(movePlayer)));
        clickedButton->setEnabled(false);

        // Record move in history if available
        if (gameHistory && currentGameId > 0) {
            gameHistory->recordMove(currentGameId, row * 3 + col);
        }
        
        // Check if game is over after the move
        if (board.isGameOver()) {
            gameOver(board.checkWinner());
            return;
        }

        // Handle turn switch based on game mode
        if (gameMode == GameMode::PvP) {
            // Switch to other player in PvP mode
            currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
            QString currentPlayerName = (currentPlayer == player1Symbol) ? player1Name : player2Name;
            statusLabel->setText(QString("%1's turn (%2)").arg(currentPlayerName, playerToChar(currentPlayer)));
        } else {
            // Switch to AI's turn in PvAI mode
            currentPlayer = aiPlayer;
            statusLabel->setText("AI is thinking...");
            enableBoard(false); // Disable board while AI thinks

            // Make AI move after a short delay
            QTimer::singleShot(500, this, &GameWindow::makeAIMove);
        }
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
            
        // Record AI move in history if available
        if (gameHistory && currentGameId > 0) {
            gameHistory->recordMove(currentGameId, row * 3 + col);
        }

        // Check if game is over after AI move
        if (board.isGameOver()) {
            gameOver(board.checkWinner());
            return;
        }

        // Switch back to human's turn
        currentPlayer = humanPlayer;
        statusLabel->setText("Your turn!");
        enableBoard(true); // Re-enable board for human
    } else {
        // Handle error case: AI couldn't make a valid move (shouldn't happen in normal play)
        statusLabel->setText("Error: AI move failed. Your turn.");
        currentPlayer = humanPlayer;
        enableBoard(true);
    }
}

void GameWindow::animateCell(QPushButton* cell, const QString& symbol) {
    // First, make the cell invisible using opacity effect
    QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(cell->graphicsEffect());
    if (!effect) { // Create effect if it doesn't exist
        effect = new QGraphicsOpacityEffect(cell);
        cell->setGraphicsEffect(effect);
    }
    effect->setOpacity(0); // Make it transparent before setting text

    // Set the symbol
    cell->setText(symbol);
    // Ensure text color matches the base style (might be overridden by win/loss)
    // This requires parsing the stylesheet or having a known base text color.
    // For simplicity, we assume the stylesheet handles the default color.

    // Create and start the fade-in animation
    QPropertyAnimation* animation = new QPropertyAnimation(effect, "opacity");
    animation->setDuration(300); // Faster fade-in
    animation->setStartValue(0.0);
    animation->setEndValue(1.0);
    animation->setEasingCurve(QEasingCurve::InOutQuad); // Smoother animation
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

void GameWindow::enableBoard(bool enable) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            // Only enable/disable if the cell is empty
            if (cells[i][j]->text().isEmpty()) {
                cells[i][j]->setEnabled(enable);
            } else {
                 // Ensure already played cells remain disabled
                 cells[i][j]->setEnabled(false);
            }
        }
    }
}
