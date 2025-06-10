#include "main_window.h"
#include "ui_constants.h"
#include "login_page.h"
#include <QVBoxLayout>
#include <QStyle>
#include <QScreen>
#include <QGuiApplication>
#include "game_window.h"

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
    
    // Connect signals for initial login
    connect(m_loginPage, &LoginPage::loginSuccessful, 
            [this](const QString &username) {
                m_currentUser = username; // Store the first user
                m_stackedWidget->setCurrentWidget(m_gameWindow);
                setupGameWindowConnections();
            });
    
    // Connect signals for second player login
    connect(m_loginPage, &LoginPage::secondPlayerLoginSuccessful,
            [this](const QString &secondPlayerUsername) {
                // Both players authenticated, set them in game window
                m_gameWindow->setPlayerNames(m_currentUser, secondPlayerUsername);
                m_stackedWidget->setCurrentWidget(m_gameWindow);
                // Resize to symbol selection size
                setFixedSize(UIConstants::WindowSize::SETUP_WIDTH, UIConstants::WindowSize::SETUP_HEIGHT);
                centerWindow();
            });
    
    // Connect back button from login page
    connect(m_loginPage, &LoginPage::backRequested,
            [this]() {
                m_stackedWidget->setCurrentWidget(m_gameWindow);
                // Resize back to game setup size
                setFixedSize(UIConstants::WindowSize::SETUP_WIDTH, UIConstants::WindowSize::SETUP_HEIGHT);
                centerWindow();
            });
    
    // Connect second player authentication request from game window
    connect(m_gameWindow, &GameWindow::secondPlayerAuthenticationRequested,
            [this]() {
                m_loginPage->setMode(LoginPageMode::SecondPlayerLogin, m_currentUser);
                m_stackedWidget->setCurrentWidget(m_loginPage);
                // Resize window for login
                setFixedSize(UIConstants::WindowSize::LOGIN_WIDTH, UIConstants::WindowSize::LOGIN_HEIGHT);
                centerWindow();
            });
    
    // Set the stacked widget as the central widget
    setCentralWidget(m_stackedWidget);
    
    // Set window properties
    setWindowTitle("Tic-Tac-Toe");
    setFixedSize(UIConstants::WindowSize::LOGIN_WIDTH, UIConstants::WindowSize::LOGIN_HEIGHT); // Fixed size for login window
    
    // Center the window on screen
    centerWindow();
    
    // Make the window floating (frameless)
    setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_TranslucentBackground);
}

void MainWindow::setupGameWindowConnections() {
    // Connect to game window's setup UI showing signal to resize window
    connect(m_gameWindow, &GameWindow::setupUIShown, this, [this]() {
        // Size for setup UI
        setFixedSize(UIConstants::WindowSize::SETUP_WIDTH, UIConstants::WindowSize::SETUP_HEIGHT);
        centerWindow();
    });
    
    // Connect to game window's game board UI showing signal to resize window
    connect(m_gameWindow, &GameWindow::gameBoardUIShown, this, [this]() {
        // Size for game board UI
        setFixedSize(UIConstants::WindowSize::GAME_WIDTH, UIConstants::WindowSize::GAME_HEIGHT);
        centerWindow();
    });

    // Connect logout signal from game window
    connect(m_gameWindow, &GameWindow::logoutRequested, this, [this]() {
        // Reset all game state to initial setup
        m_gameWindow->resetGameState();
        
        // Clear current user information
        m_currentUser.clear();
        
        // Reset login page and switch to it
        m_loginPage->setMode(LoginPageMode::InitialLogin);
        m_loginPage->clearFields();
        m_stackedWidget->setCurrentWidget(m_loginPage);
        
        // Resize window back to login size
        setFixedSize(UIConstants::WindowSize::LOGIN_WIDTH, UIConstants::WindowSize::LOGIN_HEIGHT);
        centerWindow();
    });
    
    // Start with setup UI size
    setFixedSize(UIConstants::WindowSize::SETUP_WIDTH, UIConstants::WindowSize::SETUP_HEIGHT);
    centerWindow();
}

void MainWindow::centerWindow() {
    setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            size(),
            QGuiApplication::primaryScreen()->availableGeometry()
        )
    );
}

MainWindow::~MainWindow() {
    // Qt will handle deleting the child objects
}