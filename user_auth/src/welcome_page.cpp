#include "welcome_page.h"
#include <QVBoxLayout>

WelcomePage::WelcomePage(QWidget *parent)
    : QWidget(parent) {
    
    // Set fixed window size
    setFixedSize(400, 300);
    
    // Create background widget with repeating pattern
    m_backgroundWidget = new BackgroundWidget(BackgroundWidget::Mode::RepeatingPattern, this);
    m_backgroundWidget->setGeometry(0, 0, width(), height());
    
    // Create widgets
    m_welcomeLabel = new QLabel(this);
    m_welcomeLabel->setObjectName("m_welcomeLabel");
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setFamilies(QStringList() << "Comic Sans MS" << "Arial");
    font.setPointSize(24);
    font.setBold(true);
    m_welcomeLabel->setFont(font);
    m_welcomeLabel->setStyleSheet("color: #FF0000; background: transparent;"); // Red text
    
    m_logoutButton = new QPushButton("Logout", this);
    m_logoutButton->setObjectName("m_logoutButton");
    m_logoutButton->setStyleSheet("background-color: red; color: white; padding: 5px; border-radius: 5px;");
    m_logoutButton->setFixedWidth(100);
    
    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(m_welcomeLabel);
    layout->addWidget(m_logoutButton, 0, Qt::AlignCenter);
    layout->addStretch();
    
    // Ensure background widget is behind other widgets
    m_backgroundWidget->lower();
    
    // Connect signals
    connect(m_logoutButton, &QPushButton::clicked, this, &WelcomePage::onLogoutClicked);
}

void WelcomePage::setUsername(const QString &username) {
    m_welcomeLabel->setText(QString("Hello, %1!").arg(username));
}

void WelcomePage::onLogoutClicked() {
    emit logout();
}

void WelcomePage::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    m_backgroundWidget->setGeometry(0, 0, width(), height());
}