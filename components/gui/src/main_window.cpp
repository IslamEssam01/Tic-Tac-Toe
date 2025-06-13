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
    
    // Initialize game history
    m_gameHistory = new GameHistory("game_history.db", this);
    m_gameHistoryWindow = nullptr; // Create on demand
    
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
                m_gameWindow->setGameHistory(m_gameHistory); // Set game history
                m_gameWindow->setCurrentUser(username); // Set current user
                
                // Register username mapping for game history
                if (m_gameHistoryWindow) {
                    m_gameHistoryWindow->registerUsernameMapping(username);
                }
                
                m_stackedWidget->setCurrentWidget(m_gameWindow);
                setupGameWindowConnections();
            });
    
    // Connect signals for second player login
    connect(m_loginPage, &LoginPage::secondPlayerLoginSuccessful,
            [this](const QString &secondPlayerUsername) {
                // Both players authenticated, set them in game window
                m_gameWindow->setGameHistory(m_gameHistory); // Ensure game history is set
                m_gameWindow->setCurrentUser(m_currentUser); // Set first player as current user
                m_gameWindow->setPlayerNames(m_currentUser, secondPlayerUsername);
                
                // Register username mappings for both players
                if (m_gameHistoryWindow) {
                    m_gameHistoryWindow->registerUsernameMapping(m_currentUser);
                    m_gameHistoryWindow->registerUsernameMapping(secondPlayerUsername);
                }
                
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
        
        // Close and delete game history window to ensure clean state
        if (m_gameHistoryWindow) {
            m_gameHistoryWindow->close();
            delete m_gameHistoryWindow;
            m_gameHistoryWindow = nullptr;
        }
        
        // Reset login page and switch to it
        m_loginPage->setMode(LoginPageMode::InitialLogin);
        m_loginPage->clearFields();
        m_stackedWidget->setCurrentWidget(m_loginPage);
        
        // Resize window back to login size
        setFixedSize(UIConstants::WindowSize::LOGIN_WIDTH, UIConstants::WindowSize::LOGIN_HEIGHT);
        centerWindow();
    });
    
    // Connect view history signal from game window
    connect(m_gameWindow, &GameWindow::viewHistoryRequested, this, &MainWindow::showGameHistory);
    
    // Connect username mapping signal from game window
    connect(m_gameWindow, &GameWindow::playerUsernameRegistered, this, [this](const QString& username) {
        if (m_gameHistoryWindow) {
            m_gameHistoryWindow->registerUsernameMapping(username);
        }
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

void MainWindow::showGameHistory() {
    // Create game history window if it doesn't exist
    if (!m_gameHistoryWindow) {
        m_gameHistoryWindow = new GameHistoryGUI(m_gameHistory, m_currentUser);
        m_gameHistoryWindow->setUserAuth(&m_auth); // Set UserAuth for username resolution
        m_gameHistoryWindow->setWindowTitle("Tic-Tac-Toe Game History");
        m_gameHistoryWindow->setAttribute(Qt::WA_DeleteOnClose, false); // Don't delete when closed
        
        // Register current user mapping
        m_gameHistoryWindow->registerUsernameMapping(m_currentUser);
    } else {
        // Update the current user to ensure the window shows the correct user's games
        m_gameHistoryWindow->setCurrentUser(m_currentUser);
        m_gameHistoryWindow->setUserAuth(&m_auth); // Ensure UserAuth is set for username resolution
        
        // Register current user mapping
        m_gameHistoryWindow->registerUsernameMapping(m_currentUser);
    }
    
    // Show the game history window
    m_gameHistoryWindow->show();
    m_gameHistoryWindow->raise();
    m_gameHistoryWindow->activateWindow();
}

MainWindow::~MainWindow() {
    // Clean up game history resources
    delete m_gameHistory;
    if (m_gameHistoryWindow) {
        delete m_gameHistoryWindow;
    }
    // Qt will handle deleting the child objects
}