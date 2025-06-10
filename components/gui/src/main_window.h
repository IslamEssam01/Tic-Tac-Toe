#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "auth/user_auth.h"
#include "game_window.h"

class LoginPage;
class WelcomePage;
class MainWindowTest; // Forward declaration for friend class

class MainWindow : public QMainWindow {
    Q_OBJECT
    friend class MainWindowTest; // Grant test class access to protected members

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void setupGameWindowConnections();
    void centerWindow();

protected:
    QStackedWidget *m_stackedWidget;
    LoginPage *m_loginPage;
    GameWindow *m_gameWindow; // Add a GameWindow instance
    UserAuth m_auth;
    QString m_currentUser; // Store current user name
};

#endif
