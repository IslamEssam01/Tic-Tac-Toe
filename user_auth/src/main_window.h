#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "auth/user_auth.h"

class LoginPage;
class WelcomePage;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QStackedWidget *m_stackedWidget;
    LoginPage *m_loginPage;
    WelcomePage *m_welcomePage;
    UserAuth m_auth;
};

#endif