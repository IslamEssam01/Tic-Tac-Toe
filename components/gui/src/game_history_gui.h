#ifndef GAME_HISTORY_GUI_H
#define GAME_HISTORY_GUI_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QGroupBox>
#include <QComboBox>
#include <QLineEdit>
#include <QStringList>
#include <QDateTime>
#include <QGridLayout>
#include <QSplitter>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>

#include "game_history.h"
#include "ui_constants.h"
#include "auth/user_auth.h"

class GameHistoryGUI : public QMainWindow {
    Q_OBJECT

public:
    explicit GameHistoryGUI(GameHistory* history, const QString& currentUser, QWidget *parent = nullptr);
    ~GameHistoryGUI();
    
    void setCurrentUser(const QString& username);
    void setUserAuth(UserAuth* auth);
    void registerUsernameMapping(const QString& username);

private slots:
    void displayGameDetails(QTreeWidgetItem* item, int column);
    void showMyGames();
    void onGameEvent(); // Auto-refresh when game events occur
    void restoreSelection(int gameId); // Restore selection to specific game ID

private:
    GameHistory* gameHistory;
    UserAuth* userAuth;
    QString currentUser;
    int currentUserId;
    
    // Username mapping cache
    mutable QMap<int, QString> playerIdToUsernameCache;
    
    // Main widgets
    QWidget* centralWidget;
    QSplitter* mainSplitter;
    
    // Left side - Games list
    QWidget* gamesListWidget;
    QVBoxLayout* gamesListLayout;
    QTreeWidget* gamesTreeWidget;
    
    // Right side - Game details
    QWidget* gameDetailsWidget;
    QVBoxLayout* gameDetailsLayout;
    QGroupBox* gameInfoGroupBox;
    QGridLayout* gameInfoLayout;
    QLabel* gameIdLabel;
    QLabel* gameDateLabel;
    QLabel* playerXLabel;
    QLabel* playerOLabel;
    QLabel* winnerLabel;
    
    // Game board visualization
    QGroupBox* gameBoardGroupBox;
    QGridLayout* gameBoardLayout;
    QLabel* boardCells[9];
    
    // Moves table
    QGroupBox* movesGroupBox;
    QVBoxLayout* movesLayout;
    QTableWidget* movesTable;
    
    // Helper functions
    void setupUI();
    void createGamesListSection();
    void createGameDetailsSection();
    QString formatTimestamp(const std::chrono::system_clock::time_point& timestamp);
    void updateGameBoard(const std::vector<GameHistory::Move>& moves);
    void clearGameDetails();
    QTreeWidgetItem* createGameListItem(const GameHistory::GameRecord& game);
    QString getPlayerDisplay(const std::optional<int>& playerId);
    QString getWinnerDisplay(const std::optional<int>& winnerId);
    QString resolvePlayerIdToUsername(int playerId) const;
    void populateUsernameCache();
};

#endif // GAME_HISTORY_GUI_H