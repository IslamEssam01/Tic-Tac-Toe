#include "welcome_page.h"
#include <QVBoxLayout>

WelcomePage::WelcomePage(QWidget *parent)
    : QWidget(parent) {
    
    // Set fixed window size
    setFixedSize(500, 300);
    
    // Create background widget with repeating pattern
    m_backgroundWidget = new BackgroundWidget(BackgroundWidget::Mode::RepeatingPattern, this);
    m_backgroundWidget->setGeometry(0, 0, width(), height());
    
    // Create widgets
    m_welcomeLabel = new QLabel(this);
    m_welcomeLabel->setObjectName("m_welcomeLabel");
    m_welcomeLabel->setAlignment(Qt::AlignCenter);
    QFont font;
    font.setFamilies(QStringList() << "Segoe UI" << "Arial");
    font.setPointSize(24);
    font.setBold(true);
    m_welcomeLabel->setFont(font);
    m_welcomeLabel->setStyleSheet(
        "QLabel {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 28px;"
        "    color: #5dade2;"
        "    text-shadow: 2px 2px 2px rgba(0, 0, 0, 0.3);"
        "    background: transparent;"
        "}"
    );
    
    m_logoutButton = new QPushButton("Logout", this);
    m_logoutButton->setObjectName("m_logoutButton");
    m_logoutButton->setStyleSheet(
        "QPushButton {"
        "    font-family: 'Segoe UI', sans-serif;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    color: white;"
        "    background-color: #e74c3c;"
        "    border: none;"
        "    border-radius: 8px;"
        "    padding: 12px 25px;"
        "    min-width: 120px;"
        "    border-bottom: 3px solid #c0392b;"
        "}"
        "QPushButton:hover {"
        "    background-color: #c0392b;"
        "    border-bottom-color: #a93226;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #a93226;"
        "    border-bottom-width: 1px;"
        "    margin-top: 2px;"
        "}"
    );
    m_logoutButton->setCursor(Qt::PointingHandCursor);
    
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
