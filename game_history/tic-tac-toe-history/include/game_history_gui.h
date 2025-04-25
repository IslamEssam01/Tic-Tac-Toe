#ifndef GAME_HISTORY_GUI_H
#define GAME_HISTORY_GUI_H

#include <QMainWindow>
#include <QTreeWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QGroupBox>
#include <QSpinBox>
#include "game_history.h"

class GameHistoryGUI : public QMainWindow {
    Q_OBJECT

public:
    GameHistoryGUI(const std::string& db_path, QWidget *parent = nullptr);
    ~GameHistoryGUI();

private slots:
    void loadPlayerGames();
    void loadLatestGames();
    void onItemClicked(QTreeWidgetItem *item, int column);
    void showMoveDetails(int gameId);

private:
    GameHistory* history;
    QTreeWidget* gamesTree;
    QTreeWidget* movesTree;
    QComboBox* playerSelector;
    QSpinBox* playerIdSpinBox;
    QPushButton* loadPlayerGamesBtn;
    QPushButton* loadLatestGamesBtn;
    QSpinBox* limitSpinBox;
    
    void setupUI();
    void populateGamesList(const std::vector<GameHistory::GameRecord>& games);
    void clearMovesList();
    QString getSymbolForPlayer(const GameHistory::GameRecord& game, int playerId);
    QString getWinnerText(const GameHistory::GameRecord& game);
    QString getOpponentText(const GameHistory::GameRecord& game, int playerId);
    QString getTimeText(const GameHistory::GameRecord& game);
};

#endif // GAME_HISTORY_GUI_H