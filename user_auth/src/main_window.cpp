#include "main_window.h"
#include "login_page.h"
#include <QVBoxLayout>
#include <QStyle>
#include <QScreen>
#include <QGuiApplication>
#include "../../Game/GUI/src/GameWindow.h" // Include the GameWindow header

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_auth("users.db") {
    
    // Create the stacked widget to manage pages
    m_stackedWidget = new QStackedWidget(this);
    
    // Create pages
    m_loginPage = new LoginPage(&m_auth);
    m_gameWindow = new GameWindow(); // Create GameWindow instance
    
    // Fix focus issue in GameWindow and add background color
    m_gameWindow->setStyleSheet(
        "QPushButton:focus { outline: none; border-color: inherit; }"
        "QMainWindow { background-color: #e8eff1; }" // Add light blue-gray background color
    );
    m_loginPage->setStyleSheet(
        "QPushButton:focus { outline: none; border-color: inherit; }"
        "QMainWindow { background-color: #e8eff1; }" // Add light blue-gray background color
    );
    
    // Add pages to stacked widget
    m_stackedWidget->addWidget(m_loginPage);
    m_stackedWidget->addWidget(m_gameWindow); // Add GameWindow to stacked widget
    
    // Connect signals
    connect(m_loginPage, &LoginPage::loginSuccessful, 
            [this](const QString &username) {
                m_stackedWidget->setCurrentWidget(m_gameWindow);
                
                // Connect to game window's setup UI showing signal to resize window
                connect(m_gameWindow, &GameWindow::setupUIShown, this, [this]() {
                    // Size for setup UI
                    setFixedSize(500, 350);
                    // Center the window on screen
                    setGeometry(
                        QStyle::alignedRect(
                            Qt::LeftToRight,
                            Qt::AlignCenter,
                            size(),
                            QGuiApplication::primaryScreen()->availableGeometry()
                        )
                    );
                });
                
                // Connect to game window's game board UI showing signal to resize window
                connect(m_gameWindow, &GameWindow::gameBoardUIShown, this, [this]() {
                    // Size for game board UI
                    setFixedSize(500, 700);
                    // Center the window on screen
                    setGeometry(
                        QStyle::alignedRect(
                            Qt::LeftToRight,
                            Qt::AlignCenter,
                            size(),
                            QGuiApplication::primaryScreen()->availableGeometry()
                        )
                    );
                });

                // Connect logout signal from game window
                connect(m_gameWindow, &GameWindow::logoutRequested, this, [this]() {
                    m_loginPage->clearFields();
                    m_stackedWidget->setCurrentWidget(m_loginPage);
                    // Resize window back to login size
                    setFixedSize(500, 400);
                    // Center the window on screen
                    setGeometry(
                        QStyle::alignedRect(
                            Qt::LeftToRight,
                            Qt::AlignCenter,
                            size(),
                            QGuiApplication::primaryScreen()->availableGeometry()
                        )
                    );
                });
                
                // Start with setup UI size
                setFixedSize(500, 350);
                // Center the window on screen
                setGeometry(
                    QStyle::alignedRect(
                        Qt::LeftToRight,
                        Qt::AlignCenter,
                        size(),
                        QGuiApplication::primaryScreen()->availableGeometry()
                    )
                );
            });
    
    
    // Set the stacked widget as the central widget
    setCentralWidget(m_stackedWidget);
    
    // Set window properties
    setWindowTitle("Tic-Tac-Toe");
    setFixedSize(500, 400); // Fixed size for login window
    
    // Center the window on screen
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
    
    // Make the window floating (frameless)
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground);
}

MainWindow::~MainWindow() {
    // Qt will handle deleting the child objects
}
