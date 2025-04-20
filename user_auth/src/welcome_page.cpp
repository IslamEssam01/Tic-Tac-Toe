#include "welcome_page.h"
#include <QVBoxLayout>

WelcomePage::WelcomePage(QWidget *parent)
    : QWidget(parent) {
    
    // Create widgets
    m_welcomeLabel = new QLabel(this);
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont font = m_welcomeLabel->font();
    font.setPointSize(16);
    m_welcomeLabel->setFont(font);
    
    m_logoutButton = new QPushButton("Logout", this);
    
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_welcomeLabel);
    layout->addWidget(m_logoutButton, 0, Qt::AlignCenter);
    layout->addStretch();
    
    // Connect signals
    connect(m_logoutButton, &QPushButton::clicked, 
            this, &WelcomePage::onLogoutClicked);
}

void WelcomePage::setUsername(const QString &username) {
    m_welcomeLabel->setText(QString("Hello, %1!").arg(username));
}

void WelcomePage::onLogoutClicked() {
    emit logout();
}