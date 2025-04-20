#ifndef WELCOME_PAGE_H
#define WELCOME_PAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class WelcomePage : public QWidget {
    Q_OBJECT

public:
    WelcomePage(QWidget *parent = nullptr);
    void setUsername(const QString &username);

signals:
    void logout();

private slots:
    void onLogoutClicked();

private:
    QLabel *m_welcomeLabel;
    QPushButton *m_logoutButton;
};

#endif