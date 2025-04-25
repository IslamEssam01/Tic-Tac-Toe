#include "game_history_gui.h"
#include <QHeaderView>
#include <QMessageBox>
#include <chrono>
#include <sstream>
#include <iomanip>

GameHistoryGUI::GameHistoryGUI(const std::string& db_path, QWidget *parent)
    : QMainWindow(parent) {
    // Initialize game history with the provided database path
    history = new GameHistory(db_path);
    
    // Setup the user interface
    setupUI();
    
    // Load the latest games by default (limit of 10)
    limitSpinBox->setValue(10);
    loadLatestGames();
}

GameHistoryGUI::~GameHistoryGUI() {
    delete history;
}

void GameHistoryGUI::setupUI() {
    // Main widget and layout
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    
    // Control group for player selection
    QGroupBox* playerControlGroup = new QGroupBox("Player Selection", this);
    QHBoxLayout* playerControlLayout = new QHBoxLayout(playerControlGroup);
    
    // Player ID selection
    QLabel* playerIdLabel = new QLabel("Player ID:", this);
    playerIdSpinBox = new QSpinBox(this);
    playerIdSpinBox->setMinimum(1);
    playerIdSpinBox->setMaximum(999);
    
    // Load player games button
    loadPlayerGamesBtn = new QPushButton("Load Player Games", this);
    connect(loadPlayerGamesBtn, &QPushButton::clicked, this, &GameHistoryGUI::loadPlayerGames);
    
    // Add player controls to layout
    playerControlLayout->addWidget(playerIdLabel);
    playerControlLayout->addWidget(playerIdSpinBox);
    playerControlLayout->addWidget(loadPlayerGamesBtn);
    
    // Control group for latest games
    QGroupBox* latestControlGroup = new QGroupBox("Latest Games", this);
    QHBoxLayout* latestControlLayout = new QHBoxLayout(latestControlGroup);
    
    // Limit selection
    QLabel* limitLabel = new QLabel("Limit:", this);
    limitSpinBox = new QSpinBox(this);
    limitSpinBox->setMinimum(1);
    limitSpinBox->setMaximum(100);
    limitSpinBox->setValue(10);
    
    // Load latest games button
    loadLatestGamesBtn = new QPushButton("Load Latest Games", this);
    connect(loadLatestGamesBtn, &QPushButton::clicked, this, &GameHistoryGUI::loadLatestGames);
    
    // Add latest controls to layout
    latestControlLayout->addWidget(limitLabel);
    latestControlLayout->addWidget(limitSpinBox);
    latestControlLayout->addWidget(loadLatestGamesBtn);
    
    // Create games tree view
    gamesTree = new QTreeWidget(this);
    gamesTree->setHeaderLabels({"Game", "Symbol", "Opponent", "Time", "Winner"});
    gamesTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    gamesTree->setSelectionMode(QAbstractItemView::SingleSelection);
    connect(gamesTree, &QTreeWidget::itemClicked, this, &GameHistoryGUI::onItemClicked);
    
    // Create moves tree view for detailed view
    QGroupBox* movesGroup = new QGroupBox("Game Moves", this);
    QVBoxLayout* movesLayout = new QVBoxLayout(movesGroup);
    
    movesTree = new QTreeWidget(this);
    movesTree->setHeaderLabels({"Move #", "Position"});
    movesTree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    
    movesLayout->addWidget(movesTree);
    
    // Add all widgets to main layout
    mainLayout->addWidget(playerControlGroup);
    mainLayout->addWidget(latestControlGroup);
    mainLayout->addWidget(gamesTree);
    mainLayout->addWidget(movesGroup);
    
    // Set the central widget
    setCentralWidget(centralWidget);
    
    // Set window properties
    setWindowTitle("Tic-Tac-Toe Game History");
    resize(800, 600);
}

void GameHistoryGUI::loadPlayerGames() {
    int playerId = playerIdSpinBox->value();
    
    try {
        std::vector<GameHistory::GameRecord> games = history->getPlayerGames(playerId);
        
        if (games.empty()) {
            QMessageBox::information(this, "No Games Found", 
                                   QString("No games found for player ID %1.").arg(playerId));
        } else {
            populateGamesList(games);
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                             QString("Failed to load player games: %1").arg(e.what()));
    }
}

void GameHistoryGUI::loadLatestGames() {
    int limit = limitSpinBox->value();
    
    try {
        std::vector<GameHistory::GameRecord> games = history->getLatestGames(limit);
        
        if (games.empty()) {
            QMessageBox::information(this, "No Games Found", "No games found in the database.");
        } else {
            populateGamesList(games);
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                             QString("Failed to load latest games: %1").arg(e.what()));
    }
}

void GameHistoryGUI::populateGamesList(const std::vector<GameHistory::GameRecord>& games) {
    // Clear existing items
    gamesTree->clear();
    clearMovesList();
    
    // Determine the player ID we're filtering for
    int currentPlayerId = playerIdSpinBox->value();
    
    for (const auto& game : games) {
        QTreeWidgetItem* item = new QTreeWidgetItem(gamesTree);
        
        // Store game ID in the first column (hidden data)
        item->setData(0, Qt::UserRole, game.id);
        
        // Set game number
        item->setText(0, QString("Game #%1").arg(game.id));
        
        // Set symbol (X or O)
        QString symbol = getSymbolForPlayer(game, currentPlayerId);
        item->setText(1, symbol);
        
        // Set opponent info
        QString opponent = getOpponentText(game, currentPlayerId);
        item->setText(2, opponent);
        
        // Set time
        QString time = getTimeText(game);
        item->setText(3, time);
        
        // Set winner
        QString winner = getWinnerText(game);
        item->setText(4, winner);
        
        // Add to the tree
        gamesTree->addTopLevelItem(item);
    }
}

void GameHistoryGUI::clearMovesList() {
    movesTree->clear();
}

void GameHistoryGUI::onItemClicked(QTreeWidgetItem *item, int column) {
    if (!item) return;
    
    // Get the game ID from the item's data
    int gameId = item->data(0, Qt::UserRole).toInt();
    
    // Show the move details for this game
    showMoveDetails(gameId);
}

void GameHistoryGUI::showMoveDetails(int gameId) {
    // Clear existing items
    movesTree->clear();
    
    try {
        // Get the game record
        GameHistory::GameRecord game = history->getGameById(gameId);
        
        // Add each move to the tree
        int moveNumber = 1;
        char currentSymbol = 'X'; // X always starts
        
        for (const auto& move : game.moves) {
            QTreeWidgetItem* item = new QTreeWidgetItem(movesTree);
            
            // Set move number
            QString moveText = QString("%1 (%2)").arg(moveNumber).arg(currentSymbol);
            item->setText(0, moveText);
            
            // Convert position (0-8) to board coordinates
            int row = move.position / 3;
            int col = move.position % 3;
            QString posText = QString("Row %1, Col %2 (Pos %3)").arg(row).arg(col).arg(move.position);
            item->setText(1, posText);
            
            // Add to the tree
            movesTree->addTopLevelItem(item);
            
            // Update for next move
            moveNumber++;
            currentSymbol = (currentSymbol == 'X') ? 'O' : 'X';
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", 
                             QString("Failed to load game details: %1").arg(e.what()));
    }
}

QString GameHistoryGUI::getSymbolForPlayer(const GameHistory::GameRecord& game, int playerId) {
    if (game.playerX_id.has_value() && game.playerX_id.value() == playerId) {
        return "X";
    } else if (game.playerO_id.has_value() && game.playerO_id.value() == playerId) {
        return "O";
    } else {
        return "N/A";
    }
}

QString GameHistoryGUI::getOpponentText(const GameHistory::GameRecord& game, int playerId) {
    // Check if player was X or O
    bool playerIsX = game.playerX_id.has_value() && game.playerX_id.value() == playerId;
    bool playerIsO = game.playerO_id.has_value() && game.playerO_id.value() == playerId;
    
    if (playerIsX) {
        // Player was X, opponent was O
        if (game.playerO_id.has_value()) {
            return QString("Player #%1").arg(game.playerO_id.value());
        } else {
            return "AI";
        }
    } else if (playerIsO) {
        // Player was O, opponent was X
        if (game.playerX_id.has_value()) {
            return QString("Player #%1").arg(game.playerX_id.value());
        } else {
            return "AI";
        }
    } else {
        // Player not in this game - show both players
        QString xPlayer = game.playerX_id.has_value() ? 
            QString("Player #%1").arg(game.playerX_id.value()) : "AI";
        QString oPlayer = game.playerO_id.has_value() ? 
            QString("Player #%1").arg(game.playerO_id.value()) : "AI";
        return QString("%1 vs %2").arg(xPlayer).arg(oPlayer);
    }
}

QString GameHistoryGUI::getTimeText(const GameHistory::GameRecord& game) {
    auto time_t = std::chrono::system_clock::to_time_t(game.timestamp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M");
    return QString::fromStdString(ss.str());
}

QString GameHistoryGUI::getWinnerText(const GameHistory::GameRecord& game) {
    if (!game.winner_id.has_value()) {
        return "In Progress";
    } else if (game.winner_id.value() == -1) {
        return "Draw";
    } else if (game.winner_id.value() == -2) {
        return "AI Won";
    } else {
        return QString("Player #%1 Won").arg(game.winner_id.value());
    }
}