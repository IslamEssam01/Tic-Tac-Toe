#include "game_history_gui.h"

#include <QMessageBox>
#include <QIcon>
#include <QStyle>
#include <QScreen>
#include <QGuiApplication>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <set>

GameHistoryGUI::GameHistoryGUI(GameHistory* history, const QString& currentUser, QWidget *parent)
    : QMainWindow(parent), gameHistory(history), userAuth(nullptr), currentUser(currentUser),
      isReplaying(false), currentReplayGameId(-1), currentMoveIndex(0)
{
    // Calculate user ID for filtering
    currentUserId = qHash(currentUser);
    
    setupUI();
    
    // Connect signals and slots
    connect(gamesTreeWidget, &QTreeWidget::itemClicked, this, &GameHistoryGUI::displayGameDetails);
    
    // Connect to GameHistory signals for auto-refresh
    if (gameHistory) {
        connect(gameHistory, &GameHistory::gameInitialized, this, &GameHistoryGUI::onGameEvent);
        connect(gameHistory, &GameHistory::moveRecorded, this, &GameHistoryGUI::onGameEvent);
        connect(gameHistory, &GameHistory::gameCompleted, this, &GameHistoryGUI::onGameEvent);
    }
    
    // Initialize replay timer
    replayTimer = new QTimer(this);
    connect(replayTimer, &QTimer::timeout, this, &GameHistoryGUI::onReplayTimer);
    
    // Show user's games by default
    showMyGames();
}

GameHistoryGUI::~GameHistoryGUI() {
    // The gameHistory pointer is owned by the calling code, so we don't delete it here
}

void GameHistoryGUI::setCurrentUser(const QString& username) {
    // Update user information
    currentUser = username;
    currentUserId = qHash(username);
    
    // Update window title
    setWindowTitle(QString("Tic-Tac-Toe Game History - %1").arg(currentUser));
    
    // Clear cache when user changes
    playerIdToUsernameCache.clear();
    
    // Populate cache with new current user
    populateUsernameCache();
    
    // Refresh to show new user's games
    showMyGames();
}

void GameHistoryGUI::setUserAuth(UserAuth* auth) {
    userAuth = auth;
    // Clear cache when auth changes
    playerIdToUsernameCache.clear();
    // Populate cache with current user
    populateUsernameCache();
}

void GameHistoryGUI::setupUI() {
    // Set window properties
    setWindowTitle(QString("Tic-Tac-Toe Game History - %1").arg(currentUser));
    setFixedSize(UIConstants::WindowSize::HISTORY_WIDTH, UIConstants::WindowSize::HISTORY_HEIGHT);
    
    // Add consistent styling with other windows
    setStyleSheet(
        "QMainWindow { background-color: #e8eff1; }"
        "QPushButton:focus { outline: none; border-color: inherit; }"
    );
    
    // Center the window on screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
    
    // Create central widget and main layout
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    // Create main splitter
    mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->addWidget(mainSplitter);
    
    // Create games list section (left side)
    createGamesListSection();
    
    // Create game details section (right side)
    createGameDetailsSection();
    
    // Set initial splitter sizes (adjust for much smaller board)
    QList<int> sizes;
    sizes << 900 << 450;
    mainSplitter->setSizes(sizes);
}

void GameHistoryGUI::createGamesListSection() {
    // Create games list widget
    gamesListWidget = new QWidget();
    gamesListLayout = new QVBoxLayout(gamesListWidget);
    

    
    // Create games tree widget
    gamesTreeWidget = new QTreeWidget();
    gamesTreeWidget->setHeaderLabels(QStringList() << "Game" << "Date" << "Players" << "Result");
    gamesTreeWidget->setColumnWidth(0, 70);   // Game ID column
    gamesTreeWidget->setColumnWidth(1, 150);  // Date column
    gamesTreeWidget->setColumnWidth(2, 100);  // Players column
    gamesTreeWidget->setAlternatingRowColors(true);
    gamesTreeWidget->setSortingEnabled(true);
    gamesTreeWidget->sortByColumn(1, Qt::DescendingOrder); // Sort by date descending
    gamesTreeWidget->setStyleSheet(
        "QTreeWidget {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    color: #2c3e50;"
        "    background-color: white;"
        "    alternate-background-color: #f8f9fa;"
        "    border: 1px solid #dce4e8;"
        "    border-radius: 6px;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
        "QTreeWidget::item {"
        "    color: #2c3e50;"
        "    padding: 4px;"
        "}"
        "QHeaderView::section {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    background-color: #e8eff1;"
        "    border: 1px solid #dce4e8;"
        "    padding: 8px;"
        "}"
        "QTreeWidget::item:hover {"
        "    background-color: #ecf0f1;"
        "}"
    );
    
    // Add widgets to layout
    gamesListLayout->addWidget(gamesTreeWidget);
    
    // Add to main splitter
    mainSplitter->addWidget(gamesListWidget);
}

void GameHistoryGUI::createGameDetailsSection() {
    // Create game details widget
    gameDetailsWidget = new QWidget();
    gameDetailsLayout = new QVBoxLayout(gameDetailsWidget);
    
    // Game info group box
    gameInfoGroupBox = new QGroupBox("Game Information");
    gameInfoGroupBox->setStyleSheet(
        "QGroupBox {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    color: #34495e;"
        "    background-color: white;"
        "    border: 1px solid #dce4e8;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
    );
    gameInfoLayout = new QGridLayout();
    
    // Create label styling
    QString labelStyle = 
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    color: #34495e;"
        "    font-weight: bold;"
        "}";
    
    QString valueStyle = 
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    color: #2c3e50;"
        "    padding: 2px 4px;"
        "}";
    
    // Game ID, Date, Players, Winner
    QLabel* gameIdLabel_text = new QLabel("Game ID:");
    gameIdLabel_text->setStyleSheet(labelStyle);
    gameInfoLayout->addWidget(gameIdLabel_text, 0, 0);
    gameIdLabel = new QLabel("-");
    gameIdLabel->setStyleSheet(valueStyle);
    gameInfoLayout->addWidget(gameIdLabel, 0, 1);
    
    QLabel* dateLabel_text = new QLabel("Date:");
    dateLabel_text->setStyleSheet(labelStyle);
    gameInfoLayout->addWidget(dateLabel_text, 1, 0);
    gameDateLabel = new QLabel("-");
    gameDateLabel->setStyleSheet(valueStyle);
    gameInfoLayout->addWidget(gameDateLabel, 1, 1);
    
    QLabel* playerXLabel_text = new QLabel("Player X:");
    playerXLabel_text->setStyleSheet(labelStyle);
    gameInfoLayout->addWidget(playerXLabel_text, 2, 0);
    playerXLabel = new QLabel("-");
    playerXLabel->setStyleSheet(valueStyle);
    gameInfoLayout->addWidget(playerXLabel, 2, 1);
    
    QLabel* playerOLabel_text = new QLabel("Player O:");
    playerOLabel_text->setStyleSheet(labelStyle);
    gameInfoLayout->addWidget(playerOLabel_text, 3, 0);
    playerOLabel = new QLabel("-");
    playerOLabel->setStyleSheet(valueStyle);
    gameInfoLayout->addWidget(playerOLabel, 3, 1);
    
    QLabel* winnerLabel_text = new QLabel("Winner:");
    winnerLabel_text->setStyleSheet(labelStyle);
    gameInfoLayout->addWidget(winnerLabel_text, 4, 0);
    winnerLabel = new QLabel("-");
    winnerLabel->setStyleSheet(valueStyle);
    gameInfoLayout->addWidget(winnerLabel, 4, 1);
    
    gameInfoGroupBox->setLayout(gameInfoLayout);
    
    // Game board visualization with frame to match game window
    gameBoardGroupBox = new QGroupBox("Game Board");
    gameBoardGroupBox->setStyleSheet(
        "QGroupBox {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    color: #34495e;"
        "    background-color: white;"
        "    border: 1px solid #dce4e8;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
    );
    
    // Create a frame container for the board to match game window styling
    QFrame* boardFrame = new QFrame();
    boardFrame->setFrameStyle(QFrame::NoFrame);
    boardFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #ffffff;"
        "    border-radius: 6px;"
        "    border: 1px solid #dce4e8;"
        "}"
    );
    
    gameBoardLayout = new QGridLayout(boardFrame);
    gameBoardLayout->setSpacing(UIConstants::GameBoard::HISTORY_BOARD_SPACING);
    gameBoardLayout->setContentsMargins(UIConstants::GameBoard::HISTORY_BOARD_MARGIN, UIConstants::GameBoard::HISTORY_BOARD_MARGIN, UIConstants::GameBoard::HISTORY_BOARD_MARGIN, UIConstants::GameBoard::HISTORY_BOARD_MARGIN);
    
    // Create cell style to match game window (but smaller)
    QString cellStyle = 
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: " + QString::number(UIConstants::Font::HISTORY_CELL_TEXT_SIZE) + "px;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    background-color: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                                      stop:0 #fdfefe, stop:1 #e8eff1);"
        "    border: 1px solid #dce4e8;"
        "    border-radius: 4px;"
        "    qproperty-alignment: 'AlignCenter';"
        "}";
    
    // Create 3x3 board
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int cellIndex = row * 3 + col;
            boardCells[cellIndex] = new QLabel();
            boardCells[cellIndex]->setAlignment(Qt::AlignCenter);
            boardCells[cellIndex]->setFixedSize(UIConstants::GameBoard::HISTORY_CELL_SIZE, UIConstants::GameBoard::HISTORY_CELL_SIZE);
            boardCells[cellIndex]->setStyleSheet(cellStyle);
            gameBoardLayout->addWidget(boardCells[cellIndex], row, col);
        }
    }
    
    // Add the board frame to a layout in the group box
    QVBoxLayout* groupBoxLayout = new QVBoxLayout(gameBoardGroupBox);
    groupBoxLayout->setContentsMargins(5, 15, 5, 5);
    groupBoxLayout->addWidget(boardFrame);
    
    // Moves table
    movesGroupBox = new QGroupBox("Moves");
    movesGroupBox->setStyleSheet(
        "QGroupBox {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-weight: bold;"
        "    font-size: 14px;"
        "    color: #34495e;"
        "    background-color: white;"
        "    border: 1px solid #dce4e8;"
        "    border-radius: 8px;"
        "    margin-top: 10px;"
        "    padding-top: 15px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
    );
    movesLayout = new QVBoxLayout();
    
    movesTable = new QTableWidget(0, 3); // 0 rows initially, 3 columns
    movesTable->setHorizontalHeaderLabels(QStringList() << "Move #" << "Player" << "Position");
    movesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    movesTable->verticalHeader()->setVisible(false);
    movesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    movesTable->setStyleSheet(
        "QTableWidget {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    color: #2c3e50;"
        "    background-color: #fdfefe;"
        "    alternate-background-color: #f8f9fa;"
        "    border: none;"
        "    gridline-color: #dce4e8;"
        "    selection-background-color: #3498db;"
        "    selection-color: white;"
        "}"
        "QTableWidget::item {"
        "    color: #2c3e50;"
        "    padding: 6px;"
        "}"
        "QHeaderView::section {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-weight: bold;"
        "    color: #34495e;"
        "    background-color: #e8eff1;"
        "    border: 1px solid #dce4e8;"
        "    padding: 8px;"
        "}"
    );
    
    movesLayout->addWidget(movesTable);
    movesGroupBox->setLayout(movesLayout);
    
    // Create simple replay button
    replayButton = new QPushButton("Replay Game");
    replayButton->setStyleSheet(
        "QPushButton {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background-color: #5dade2;"
        "    border: none;"
        "    border-radius: 6px;"
        "    padding: 8px 16px;"
        "    min-width: 100px;"
        "    border-bottom: 3px solid #4a9fcc;"
        "}"
        "QPushButton:hover {"
        "    background-color: #4a9fcc;"
        "    border-bottom-color: #3a8db8;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3a8db8;"
        "    border-bottom-width: 1px;"
        "    margin-top: 2px;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #95a5a6;"
        "    color: #7f8c8d;"
        "}"
    );
    replayButton->setEnabled(false);
    connect(replayButton, &QPushButton::clicked, this, &GameHistoryGUI::startReplay);
    
    // Add all widgets to the layout
    gameDetailsLayout->addWidget(gameInfoGroupBox);
    gameDetailsLayout->addWidget(gameBoardGroupBox);
    gameDetailsLayout->addWidget(replayButton, 0, Qt::AlignCenter);
    gameDetailsLayout->addWidget(movesGroupBox);
    
    // Add to main splitter
    mainSplitter->addWidget(gameDetailsWidget);
    
    // Initialize with empty state
    clearGameDetails();
}



void GameHistoryGUI::showMyGames() {
    // Clear the tree widget
    gamesTreeWidget->clear();
    
    // Get user's games
    std::vector<GameHistory::GameRecord> games = gameHistory->getPlayerGames(currentUserId);
    
    if (games.empty()) {
        clearGameDetails();
        return;
    }
    
    // Populate the tree widget
    for (const auto& game : games) {
        QTreeWidgetItem* item = createGameListItem(game);
        gamesTreeWidget->addTopLevelItem(item);
    }

    
    // Clear game details
    clearGameDetails();
}



void GameHistoryGUI::displayGameDetails(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    
    bool ok;
    int gameId = item->data(0, Qt::DisplayRole).toInt(&ok);
    
    if (!ok) {
        return;
    }
    
    // Stop any ongoing replay
    if (isReplaying) {
        stopReplay();
    }
    
    // Get the game record
    GameHistory::GameRecord game = gameHistory->getGameById(gameId);
    
    // Update game information
    gameIdLabel->setText(QString::number(game.id));
    gameDateLabel->setText(formatTimestamp(game.timestamp));
    playerXLabel->setText(getPlayerDisplay(game.playerX_id));
    playerOLabel->setText(getPlayerDisplay(game.playerO_id));
    winnerLabel->setText(getWinnerDisplay(game.winner_id));
    
    // Update game board
    updateGameBoard(game.moves);
    
    // Set up replay data
    currentReplayGameId = gameId;
    currentGameMoves = game.moves;
    currentMoveIndex = 0;
    replayButton->setEnabled(!currentGameMoves.empty());
    
    // Update moves table
    movesTable->setRowCount(game.moves.size());
    
    for (size_t i = 0; i < game.moves.size(); i++) {
        // Move number (1-based)
        QTableWidgetItem* moveNumberItem = new QTableWidgetItem(QString::number(i + 1));
        moveNumberItem->setTextAlignment(Qt::AlignCenter);
        movesTable->setItem(i, 0, moveNumberItem);
        
        // Player (X or O)
        QString player = (i % 2 == 0) ? "X" : "O";
        QTableWidgetItem* playerItem = new QTableWidgetItem(player);
        playerItem->setTextAlignment(Qt::AlignCenter);
        movesTable->setItem(i, 1, playerItem);
        
        // Position (0-8)
        int position = game.moves[i].position;
        int row = position / 3;
        int col = position % 3;
        QString positionText = QString("(%1,%2)").arg(row).arg(col);
        
        QTableWidgetItem* positionItem = new QTableWidgetItem(positionText);
        positionItem->setTextAlignment(Qt::AlignCenter);
        movesTable->setItem(i, 2, positionItem);
    }
}

QString GameHistoryGUI::formatTimestamp(const std::chrono::system_clock::time_point& timestamp) {
    auto time_t = std::chrono::system_clock::to_time_t(timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return QString::fromStdString(ss.str());
}

void GameHistoryGUI::updateGameBoard(const std::vector<GameHistory::Move>& moves) {
    // Clear the board first
    for (int i = 0; i < 9; i++) {
        boardCells[i]->setText("");
    }
    
    // Add moves to the board
    for (size_t i = 0; i < moves.size(); i++) {
        int position = moves[i].position;
        if (position >= 0 && position < 9) {
            QString symbol = (i % 2 == 0) ? "X" : "O";
            boardCells[position]->setText(symbol);
        }
    }
}

void GameHistoryGUI::clearGameDetails() {
    // Stop any ongoing replay
    if (isReplaying) {
        stopReplay();
    }
    
    gameIdLabel->setText("-");
    gameDateLabel->setText("-");
    playerXLabel->setText("-");
    playerOLabel->setText("-");
    winnerLabel->setText("-");
    
    // Clear board
    for (int i = 0; i < 9; i++) {
        boardCells[i]->setText("");
    }
    
    // Clear replay data
    currentReplayGameId = -1;
    currentGameMoves.clear();
    currentMoveIndex = 0;
    replayButton->setEnabled(false);
    
    // Clear moves table
    movesTable->setRowCount(0);
}

QTreeWidgetItem* GameHistoryGUI::createGameListItem(const GameHistory::GameRecord& game) {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    
    // Game ID
    item->setText(0, QString::number(game.id));
    
    // Date
    item->setText(1, formatTimestamp(game.timestamp));
    
    // Players (X vs O)
    QString playersText = getPlayerDisplay(game.playerX_id) + " vs " + getPlayerDisplay(game.playerO_id);
    item->setText(2, playersText);
    
    // Winner
    item->setText(3, getWinnerDisplay(game.winner_id));
    
    return item;
}

QString GameHistoryGUI::getPlayerDisplay(const std::optional<int>& playerId) {
    if (!playerId.has_value()) {
        return "AI";
    } else {
        QString username = resolvePlayerIdToUsername(playerId.value());
        if (!username.isEmpty()) {
            return username;
        } else {
            return "Player " + QString::number(playerId.value());
        }
    }
}

QString GameHistoryGUI::getWinnerDisplay(const std::optional<int>& winnerId) {
    if (!winnerId.has_value()) {
        return "In Progress";
    } else if (winnerId.value() == -1) {
        return "Draw";
    } else if (winnerId.value() == -2) {
        return "AI Won";
    } else {
        QString username = resolvePlayerIdToUsername(winnerId.value());
        if (!username.isEmpty()) {
            return username + " Won";
        } else {
            return "Player " + QString::number(winnerId.value()) + " Won";
        }
    }
}

void GameHistoryGUI::restoreSelection(int gameId) {
    // Find the item with the matching game ID and select it
    for (int i = 0; i < gamesTreeWidget->topLevelItemCount(); ++i) {
        QTreeWidgetItem* item = gamesTreeWidget->topLevelItem(i);
        bool ok;
        int itemGameId = item->data(0, Qt::DisplayRole).toInt(&ok);
        if (ok && itemGameId == gameId) {
            // Select the item
            gamesTreeWidget->setCurrentItem(item);
            
            // Display the game details
            displayGameDetails(item, 0);
            break;
        }
    }
}



void GameHistoryGUI::onGameEvent() {
    // Only refresh if the window is visible to avoid unnecessary updates
    if (!this->isVisible()) {
        return;
    }
    
    // Auto-refresh the games list when any game event occurs
    // but preserve the current selection if possible
    
    // Save currently selected game ID
    int selectedGameId = -1;
    QTreeWidgetItem* currentItem = gamesTreeWidget->currentItem();
    if (currentItem) {
        bool ok;
        selectedGameId = currentItem->data(0, Qt::DisplayRole).toInt(&ok);
        if (!ok) {
            selectedGameId = -1;
        }
    }
    
    // Refresh the user's games
    showMyGames();
    
    // Restore selection if we had one
    if (selectedGameId != -1) {
        restoreSelection(selectedGameId);
    }
}

QString GameHistoryGUI::resolvePlayerIdToUsername(int playerId) const {
    // Check if we already have this mapping cached
    if (playerIdToUsernameCache.contains(playerId)) {
        return playerIdToUsernameCache[playerId];
    }
    
    // If it's the current user, we know the mapping
    if (playerId == currentUserId) {
        playerIdToUsernameCache[playerId] = currentUser;
        return currentUser;
    }
    
    // Try to get username from game history database
    if (gameHistory) {
        std::string username = gameHistory->getPlayerUsername(playerId);
        if (!username.empty()) {
            QString qUsername = QString::fromStdString(username);
            playerIdToUsernameCache[playerId] = qUsername;
            return qUsername;
        }
    }
    
    // For unknown player IDs, return a more user-friendly format
    QString fallbackName = QString("Player_%1").arg(QString::number(playerId).right(4));
    playerIdToUsernameCache[playerId] = fallbackName;
    return fallbackName;
}

void GameHistoryGUI::populateUsernameCache() {
    // Add current user to cache
    if (!currentUser.isEmpty()) {
        playerIdToUsernameCache[currentUserId] = currentUser;
    }
    
    // Load username mappings from the database
    if (gameHistory) {
        // Get all games to find all player IDs that have been used
        auto allGames = gameHistory->getAllGames();
        std::set<int> playerIds;
        
        for (const auto& game : allGames) {
            if (game.playerX_id.has_value()) {
                playerIds.insert(game.playerX_id.value());
            }
            if (game.playerO_id.has_value()) {
                playerIds.insert(game.playerO_id.value());
            }
        }
        
        // For each unique player ID, try to get the username from database
        for (int playerId : playerIds) {
            std::string username = gameHistory->getPlayerUsername(playerId);
            if (!username.empty()) {
                playerIdToUsernameCache[playerId] = QString::fromStdString(username);
            }
        }
    }
}



void GameHistoryGUI::registerUsernameMapping(const QString& username) {
    if (!username.isEmpty()) {
        int playerId = qHash(username);
        playerIdToUsernameCache[playerId] = username;
    }
}

void GameHistoryGUI::startReplay() {
    if (currentReplayGameId == -1 || currentGameMoves.empty()) {
        return;
    }
    
    isReplaying = true;
    currentMoveIndex = 0;
    
    // Update button
    replayButton->setText("Stop Replay");
    replayButton->disconnect();
    connect(replayButton, &QPushButton::clicked, this, &GameHistoryGUI::stopReplay);
    
    updateReplayBoard(0); // Show initial empty board
    
    // Start the timer (1 second between moves)
    replayTimer->start(1000);
}



void GameHistoryGUI::stopReplay() {
    isReplaying = false;
    replayTimer->stop();
    
    // Reset button
    replayButton->setText("Replay Game");
    replayButton->disconnect();
    connect(replayButton, &QPushButton::clicked, this, &GameHistoryGUI::startReplay);
    
    // Show the final game state
    updateGameBoard(currentGameMoves);
}



void GameHistoryGUI::onReplayTimer() {
    if (currentMoveIndex < static_cast<int>(currentGameMoves.size())) {
        currentMoveIndex++;
        updateReplayBoard(currentMoveIndex);
        
        if (currentMoveIndex >= static_cast<int>(currentGameMoves.size())) {
            // Reached the end, stop replay
            stopReplay();
        }
    }
}

void GameHistoryGUI::updateReplayBoard(int moveIndex) {
    // Clear the board first
    for (int i = 0; i < 9; i++) {
        boardCells[i]->setText("");
    }
    
    // Add moves up to the current index
    for (int i = 0; i < moveIndex && i < static_cast<int>(currentGameMoves.size()); i++) {
        int position = currentGameMoves[i].position;
        if (position >= 0 && position < 9) {
            QString symbol = (i % 2 == 0) ? "X" : "O";
            boardCells[position]->setText(symbol);
        }
    }
}


