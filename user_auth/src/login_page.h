#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include "auth/user_auth.h"
#include "background_widget.h"

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

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QPushButton *m_registerButton;
    QLabel *m_statusLabel;
    UserAuth *m_auth;
    BackgroundWidget *m_backgroundWidget;
};

#endif