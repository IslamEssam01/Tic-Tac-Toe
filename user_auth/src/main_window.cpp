#include "main_window.h"
#include "login_page.h"
#include "welcome_page.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_auth("users.db") {
    
    // Create the stacked widget to manage pages
    m_stackedWidget = new QStackedWidget(this);
    
    // Create pages
    m_loginPage = new LoginPage(&m_auth);
    m_welcomePage = new WelcomePage();
    
    // Add pages to stacked widget
    m_stackedWidget->addWidget(m_loginPage);
    m_stackedWidget->addWidget(m_welcomePage);
    
    // Connect signals
    connect(m_loginPage, &LoginPage::loginSuccessful, 
            [this](const QString &username) {
                m_welcomePage->setUsername(username);
                m_stackedWidget->setCurrentWidget(m_welcomePage);
            });
    
    connect(m_welcomePage, &WelcomePage::logout,
            [this]() {
                m_loginPage->clearFields();
                m_stackedWidget->setCurrentWidget(m_loginPage);
            });
    
    // Set the stacked widget as the central widget
    setCentralWidget(m_stackedWidget);
    
    // Set window properties
    setWindowTitle("User Authentication");
    resize(400, 300);
}

MainWindow::~MainWindow() {
    // Qt will handle deleting the child objects
}