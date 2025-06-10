#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include "auth/user_auth.h"

enum class LoginPageMode {
    InitialLogin,
    SecondPlayerLogin
};

class LoginPage : public QMainWindow {
    Q_OBJECT

public:
    LoginPage(UserAuth *auth, QWidget *parent = nullptr);
    void clearFields();
    void setMode(LoginPageMode mode, const QString &firstPlayerName = "");

signals:
    void loginSuccessful(const QString &username);
    void secondPlayerLoginSuccessful(const QString &username);
    void backRequested();

private slots:
    void onLoginClicked();
    void onRegisterClicked();
    void onBackClicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void setupUI();
    void updateUIForMode();
    QWidget* createFormContainer();
    QString getBaseButtonStyle(const QString &bgColor, const QString &borderColor) const;

    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QPushButton *m_registerButton;
    QPushButton *m_backButton;
    QLabel *m_statusLabel;
    QLabel *m_titleLabel;
    UserAuth *m_auth;
    QWidget *m_centralWidget;
    LoginPageMode m_mode;
    QString m_firstPlayerName;
};

#endif
