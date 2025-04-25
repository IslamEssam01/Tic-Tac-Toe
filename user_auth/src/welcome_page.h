#ifndef WELCOME_PAGE_H
#define WELCOME_PAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QResizeEvent>
#include "background_widget.h"

class WelcomePage : public QWidget {
    Q_OBJECT

public:
    WelcomePage(QWidget *parent = nullptr);
    void setUsername(const QString &username);

signals:
    void logout();

private slots:
    void onLogoutClicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *m_welcomeLabel;
    QPushButton *m_logoutButton;
    BackgroundWidget *m_backgroundWidget;
};

#endif