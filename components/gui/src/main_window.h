#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "auth/user_auth.h"
#include "game_window.h"
#include "game_history.h"
#include "game_history_gui.h"

class LoginPage;
class WelcomePage;
class GameHistoryGUI;
class MainWindowTest; // Forward declaration for friend class

class MainWindow : public QMainWindow {
    Q_OBJECT
    friend class MainWindowTest; // Grant test class access to protected members

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void showGameHistory();

private:
    void setupGameWindowConnections();
    void centerWindow();

protected:
    QStackedWidget *m_stackedWidget;
    LoginPage *m_loginPage;
    GameWindow *m_gameWindow; // Add a GameWindow instance
    GameHistory *m_gameHistory; // Game history backend
    GameHistoryGUI *m_gameHistoryWindow; // Game history GUI window
    UserAuth m_auth;
    QString m_currentUser; // Store current user name
};

#endif
