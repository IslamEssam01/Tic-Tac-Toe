#include "game_history_gui.h"

#include <QMessageBox>
#include <QIcon>
#include <ctime>
#include <iomanip>
#include <sstream>

GameHistoryGUI::GameHistoryGUI(GameHistory* history, QWidget *parent)
    : QMainWindow(parent), gameHistory(history)
{
    setupUI();
    
    // Connect signals and slots
    connect(loadButton, &QPushButton::clicked, this, &GameHistoryGUI::loadPlayerGames);
    connect(gamesTreeWidget, &QTreeWidget::itemClicked, this, &GameHistoryGUI::displayGameDetails);
    connect(refreshButton, &QPushButton::clicked, this, &GameHistoryGUI::refreshGamesList);
    connect(showAllButton, &QPushButton::clicked, this, &GameHistoryGUI::showAllGames);
    
    // Show all games by default
    showAllGames();
}

GameHistoryGUI::~GameHistoryGUI() {
    // The gameHistory pointer is owned by the calling code, so we don't delete it here
}

void GameHistoryGUI::setupUI() {
    // Set window properties
    setWindowTitle("Tic-Tac-Toe Game History");
    resize(900, 600);
    
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
    
    // Set initial splitter sizes
    QList<int> sizes;
    sizes << 350 << 550;
    mainSplitter->setSizes(sizes);
}

void GameHistoryGUI::createGamesListSection() {
    // Create games list widget
    gamesListWidget = new QWidget();
    gamesListLayout = new QVBoxLayout(gamesListWidget);
    
    // Create player filter controls
    QHBoxLayout* filterLayout = new QHBoxLayout();
    
    playerFilterComboBox = new QComboBox();
    playerFilterComboBox->addItem("Filter by Player ID");
    
    playerIdInput = new QLineEdit();
    playerIdInput->setPlaceholderText("Enter Player ID");
    
    loadButton = new QPushButton("Load");
    
    showAllButton = new QPushButton("Show All");
    
    refreshButton = new QPushButton("Refresh");
    
    filterLayout->addWidget(playerFilterComboBox);
    filterLayout->addWidget(playerIdInput);
    filterLayout->addWidget(loadButton);
    filterLayout->addWidget(showAllButton);
    filterLayout->addWidget(refreshButton);
    
    // Create games tree widget
    gamesTreeWidget = new QTreeWidget();
    gamesTreeWidget->setHeaderLabels(QStringList() << "Game" << "Date" << "Players" << "Result");
    gamesTreeWidget->setColumnWidth(0, 60);   // Game ID column
    gamesTreeWidget->setColumnWidth(1, 150);  // Date column
    gamesTreeWidget->setColumnWidth(2, 100);  // Players column
    gamesTreeWidget->setAlternatingRowColors(true);
    gamesTreeWidget->setSortingEnabled(true);
    gamesTreeWidget->sortByColumn(1, Qt::DescendingOrder); // Sort by date descending
    
    // Add widgets to layout
    gamesListLayout->addLayout(filterLayout);
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
    gameInfoLayout = new QGridLayout();
    
    // Game ID, Date, Players, Winner
    gameInfoLayout->addWidget(new QLabel("Game ID:"), 0, 0);
    gameIdLabel = new QLabel("-");
    gameInfoLayout->addWidget(gameIdLabel, 0, 1);
    
    gameInfoLayout->addWidget(new QLabel("Date:"), 1, 0);
    gameDateLabel = new QLabel("-");
    gameInfoLayout->addWidget(gameDateLabel, 1, 1);
    
    gameInfoLayout->addWidget(new QLabel("Player X:"), 2, 0);
    playerXLabel = new QLabel("-");
    gameInfoLayout->addWidget(playerXLabel, 2, 1);
    
    gameInfoLayout->addWidget(new QLabel("Player O:"), 3, 0);
    playerOLabel = new QLabel("-");
    gameInfoLayout->addWidget(playerOLabel, 3, 1);
    
    gameInfoLayout->addWidget(new QLabel("Winner:"), 4, 0);
    winnerLabel = new QLabel("-");
    gameInfoLayout->addWidget(winnerLabel, 4, 1);
    
    gameInfoGroupBox->setLayout(gameInfoLayout);
    
    // Game board visualization
    gameBoardGroupBox = new QGroupBox("Game Board");
    gameBoardLayout = new QGridLayout();
    
    // Create 3x3 board
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 3; col++) {
            int cellIndex = row * 3 + col;
            boardCells[cellIndex] = new QLabel();
            boardCells[cellIndex]->setAlignment(Qt::AlignCenter);
            boardCells[cellIndex]->setMinimumSize(60, 60);
            boardCells[cellIndex]->setFrameShape(QFrame::Box);
            boardCells[cellIndex]->setFrameShadow(QFrame::Raised);
            boardCells[cellIndex]->setLineWidth(1);
            boardCells[cellIndex]->setMidLineWidth(1);
            boardCells[cellIndex]->setStyleSheet("font-size: 24px;");
            gameBoardLayout->addWidget(boardCells[cellIndex], row, col);
        }
    }
    
    gameBoardGroupBox->setLayout(gameBoardLayout);
    
    // Moves table
    movesGroupBox = new QGroupBox("Moves");
    movesLayout = new QVBoxLayout();
    
    movesTable = new QTableWidget(0, 3); // 0 rows initially, 3 columns
    movesTable->setHorizontalHeaderLabels(QStringList() << "Move #" << "Player" << "Position");
    movesTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    movesTable->verticalHeader()->setVisible(false);
    movesTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    
    movesLayout->addWidget(movesTable);
    movesGroupBox->setLayout(movesLayout);
    
    // Add all widgets to the layout
    gameDetailsLayout->addWidget(gameInfoGroupBox);
    gameDetailsLayout->addWidget(gameBoardGroupBox);
    gameDetailsLayout->addWidget(movesGroupBox);
    
    // Add to main splitter
    mainSplitter->addWidget(gameDetailsWidget);
    
    // Initialize with empty state
    clearGameDetails();
}

void GameHistoryGUI::loadPlayerGames() {
    bool ok;
    int playerId = playerIdInput->text().toInt(&ok);
    
    if (!ok) {
        QMessageBox::warning(this, "Invalid Input", "Please enter a valid player ID.");
        return;
    }
    
    // Clear the tree widget
    gamesTreeWidget->clear();
    
    // Get player games
    std::vector<GameHistory::GameRecord> games = gameHistory->getPlayerGames(playerId);
    
    if (games.empty()) {
        QMessageBox::information(this, "No Games Found", 
                                "No games found for player ID " + QString::number(playerId));
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

void GameHistoryGUI::showAllGames() {
    // Clear the tree widget
    gamesTreeWidget->clear();
    
    // Get all games
    std::vector<GameHistory::GameRecord> games = gameHistory->getAllGames();
    
    if (games.empty()) {
        QMessageBox::information(this, "No Games Found", "No games found in the database.");
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

void GameHistoryGUI::refreshGamesList() {
    // Check if we're filtering by player ID
    if (!playerIdInput->text().isEmpty()) {
        loadPlayerGames();
    } else {
        showAllGames();
    }
}

void GameHistoryGUI::displayGameDetails(QTreeWidgetItem* item, int column) {
    Q_UNUSED(column);
    
    bool ok;
    int gameId = item->data(0, Qt::DisplayRole).toInt(&ok);
    
    if (!ok) {
        return;
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
    gameIdLabel->setText("-");
    gameDateLabel->setText("-");
    playerXLabel->setText("-");
    playerOLabel->setText("-");
    winnerLabel->setText("-");
    
    // Clear board
    for (int i = 0; i < 9; i++) {
        boardCells[i]->setText("");
    }
    
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
        return "Player " + QString::number(playerId.value());
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
        return "Player " + QString::number(winnerId.value()) + " Won";
    }
}