#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "auth/user_auth.h"

class LoginPage : public QWidget {
    Q_OBJECT

public:
    LoginPage(UserAuth *auth, QWidget *parent = nullptr);
    void clearFields();

signals:
    void loginSuccessful(const QString &username);

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QPushButton *m_registerButton;
    QLabel *m_statusLabel;
    UserAuth *m_auth;
};

#endif