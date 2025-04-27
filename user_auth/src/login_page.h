#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include <QMainWindow>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QResizeEvent>
#include "auth/user_auth.h"
#include "background_widget.h"

class LoginPage : public QMainWindow {
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
    void setupUI();
    QWidget* createFormContainer();
    QString getBaseButtonStyle(const QString &bgColor, const QString &borderColor) const;

    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QPushButton *m_registerButton;
    QLabel *m_statusLabel;
    UserAuth *m_auth;
    QWidget *m_centralWidget;
    BackgroundWidget *m_backgroundWidget;
};

#endif
