#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "auth/user_auth.h"
#include "../../Game/GUI/src/GameWindow.h" // Include the GameWindow header

class LoginPage;
class WelcomePage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    QStackedWidget *m_stackedWidget;
    LoginPage *m_loginPage;
    WelcomePage *m_welcomePage;
    GameWindow *m_gameWindow; // Add a GameWindow instance
    UserAuth m_auth;
};

#ifdef TESTING
class TestableMainWindow : public MainWindow {
public:
    TestableMainWindow() : MainWindow() {}
    
    QStackedWidget* getStackedWidget() const {
        return m_stackedWidget;
    }
    
    LoginPage* getLoginPage() const {
        return m_loginPage;
    }
    
    WelcomePage* getWelcomePage() const {
        return m_welcomePage;
    }
    
    GameWindow* getGameWindow() const { // Add getter for GameWindow
        return m_gameWindow;
    }
};
#endif

#endif
