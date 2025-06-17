#include "performance_monitor.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>
#include <vector>
#include <random>
#include <QApplication>
#include <QWidget>
#include <QMainWindow>
#include <QStackedWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QSplitter>
#include <QHeaderView>
#include <QTimer>
#include <QEventLoop>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QTest>
#include <QElapsedTimer>
#include <QScreen>
#include <QDialog>
#include <QScrollBar>
#include <QProgressBar>

// Include actual Tic-Tac-Toe GUI components
#include "main_window.h"
#include "login_page.h"
#include "game_window.h"
#include "game_history_gui.h"
#include "auth/user_auth.h"
#include "game_history.h"
#include "Board.h"

class TicTacToeGUIPerformanceTest {
public:
    TicTacToeGUIPerformanceTest(QApplication* app) 
        : app_(app), monitor_(), tempDir_("ttt_gui_perf_test") {
        setupTestData();
    }
    
    void runAllTests() {
        std::cout << "=== Tic-Tac-Toe GUI Performance Test Suite ===" << std::endl;
        std::cout << "Testing only application-specific GUI components..." << std::endl << std::endl;
        
        // Core application window tests
        testMainWindowPerformance();
        testLoginPagePerformance();
        testGameWindowPerformance();
        testGameHistoryGUIPerformance();
        
        // Application-specific user interactions
        testGameBoardInteractions();
        testAuthenticationWorkflow();
        testGameNavigationWorkflow();
        
        // Real game scenarios
        testCompleteGamePlaythrough();
        testGameHistoryBrowsing();
        testMultipleGameSessions();
        
        // Memory usage during actual gameplay
        testMemoryUsageDuringGameplay();
        testConcurrentGameOperations();
        
        // Generate and display results
        generateReport();
    }

private:
    QApplication* app_;
    PerformanceMonitor monitor_;
    QString tempDir_;
    std::unique_ptr<UserAuth> testAuth_;
    std::unique_ptr<GameHistory> testHistory_;
    
    void setupTestData() {
        // Create test databases for realistic testing
        QString authDbPath = tempDir_ + "/ttt_gui_auth.db";
        QString historyDbPath = tempDir_ + "/ttt_gui_history.db";
        
        testAuth_ = std::make_unique<UserAuth>(authDbPath.toStdString());
        testHistory_ = std::make_unique<GameHistory>(historyDbPath.toStdString());
        
        // Register test users
        testAuth_->registerUser("player1", "password123");
        testAuth_->registerUser("player2", "password456");
        testAuth_->registerUser("testuser", "testpass");
        
        // Create realistic game history
        for (int i = 0; i < 50; ++i) {
            int gameId = testHistory_->initializeGame(1000 + i, 2000 + (i % 10));
            
            // Simulate realistic games with 5-9 moves
            int moveCount = 5 + (i % 5);
            for (int move = 0; move < moveCount; ++move) {
                testHistory_->recordMove(gameId, move);
            }
            
            // Set realistic winners (player 1 wins 40%, player 2 wins 35%, draws 25%)
            if (i % 4 == 0) {
                testHistory_->setWinner(gameId, -1); // Draw
            } else if (i % 3 == 0) {
                testHistory_->setWinner(gameId, 2000 + (i % 10)); // Player 2 wins
            } else {
                testHistory_->setWinner(gameId, 1000 + i); // Player 1 wins
            }
        }
    }
    
    void testMainWindowPerformance() {
        std::cout << "Testing MainWindow Performance..." << std::endl;
        
        // Test 1: MainWindow creation and initialization
        {
            PERFORMANCE_TEST(monitor_, "MainWindow_Creation_And_Setup");
            for (int i = 0; i < 20; ++i) {
                auto mainWindow = std::make_unique<MainWindow>();
                mainWindow->show();
                bool exposed = QTest::qWaitForWindowExposed(mainWindow.get());
                Q_UNUSED(exposed);
                mainWindow->close();
                QCoreApplication::processEvents();
            }
        }
        
        // Test 2: MainWindow navigation performance
        {
            PERFORMANCE_TEST(monitor_, "MainWindow_Navigation");
            auto mainWindow = std::make_unique<MainWindow>();
            mainWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(mainWindow.get());
            Q_UNUSED(exposed);
            
            // Test navigation between different views/pages in MainWindow
            auto stackedWidget = mainWindow->findChild<QStackedWidget*>();
            if (stackedWidget) {
                for (int i = 0; i < 30; ++i) {
                    stackedWidget->setCurrentIndex(i % stackedWidget->count());
                    QCoreApplication::processEvents();
                }
            }
            
            mainWindow->close();
        }
        
        // Test 3: MainWindow window operations
        {
            PERFORMANCE_TEST(monitor_, "MainWindow_WindowOperations");
            auto mainWindow = std::make_unique<MainWindow>();
            mainWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(mainWindow.get());
            Q_UNUSED(exposed);
            
            QSize originalSize = mainWindow->size();
            
            // Test resize operations
            for (int i = 0; i < 20; ++i) {
                QSize newSize(originalSize.width() + (i * 20), originalSize.height() + (i * 10));
                mainWindow->resize(newSize);
                QCoreApplication::processEvents();
            }
            
            mainWindow->close();
        }
    }
    
    void testLoginPagePerformance() {
        std::cout << "Testing LoginPage Performance..." << std::endl;
        
        // Test 1: LoginPage creation and display
        {
            PERFORMANCE_TEST(monitor_, "LoginPage_Creation");
            for (int i = 0; i < 25; ++i) {
                auto loginPage = std::make_unique<LoginPage>(testAuth_.get());
                loginPage->show();
                bool exposed = QTest::qWaitForWindowExposed(loginPage.get());
                Q_UNUSED(exposed);
                loginPage->close();
                QCoreApplication::processEvents();
            }
        }
        
        // Test 2: Login form input performance
        {
            PERFORMANCE_TEST(monitor_, "LoginPage_FormInput");
            auto loginPage = std::make_unique<LoginPage>(testAuth_.get());
            loginPage->show();
            bool exposed = QTest::qWaitForWindowExposed(loginPage.get());
            Q_UNUSED(exposed);
            
            auto usernameEdit = loginPage->findChild<QLineEdit*>("usernameEdit");
            auto passwordEdit = loginPage->findChild<QLineEdit*>("passwordEdit");
            
            if (!usernameEdit) usernameEdit = loginPage->findChild<QLineEdit*>();
            if (!passwordEdit) {
                auto lineEdits = loginPage->findChildren<QLineEdit*>();
                if (lineEdits.size() >= 2) passwordEdit = lineEdits[1];
            }
            
            if (usernameEdit && passwordEdit) {
                for (int i = 0; i < 20; ++i) {
                    usernameEdit->clear();
                    passwordEdit->clear();
                    
                    QString username = QString("testuser%1").arg(i);
                    QString password = QString("testpass%1").arg(i);
                    
                    QTest::keyClicks(usernameEdit, username);
                    QTest::keyClicks(passwordEdit, password);
                    QCoreApplication::processEvents();
                }
            }
            
            loginPage->close();
        }
        
        // Test 3: Actual authentication process
        {
            PERFORMANCE_TEST(monitor_, "LoginPage_Authentication");
            auto loginPage = std::make_unique<LoginPage>(testAuth_.get());
            loginPage->show();
            bool exposed = QTest::qWaitForWindowExposed(loginPage.get());
            Q_UNUSED(exposed);
            
            // Simulate login attempts with existing users
            for (int i = 0; i < 10; ++i) {
                QString username = (i % 2 == 0) ? "player1" : "player2";
                QString password = (i % 2 == 0) ? "password123" : "password456";
                
                // Find and fill login form
                auto usernameEdit = loginPage->findChildren<QLineEdit*>().value(0);
                auto passwordEdit = loginPage->findChildren<QLineEdit*>().value(1);
                
                if (usernameEdit && passwordEdit) {
                    usernameEdit->setText(username);
                    passwordEdit->setText(password);
                    
                    // Find and click login button
                    auto loginButton = loginPage->findChild<QPushButton*>();
                    if (loginButton) {
                        QTest::mouseClick(loginButton, Qt::LeftButton);
                        QCoreApplication::processEvents();
                    }
                }
            }
            
            loginPage->close();
        }
    }
    
    void testGameWindowPerformance() {
        std::cout << "Testing GameWindow Performance..." << std::endl;
        
        // Test 1: GameWindow creation and setup
        {
            PERFORMANCE_TEST(monitor_, "GameWindow_Creation");
            for (int i = 0; i < 15; ++i) {
                auto gameWindow = std::make_unique<GameWindow>();
                gameWindow->show();
                bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
                Q_UNUSED(exposed);
                gameWindow->close();
                QCoreApplication::processEvents();
            }
        }
        
        // Test 2: Game board rendering and updates
        {
            PERFORMANCE_TEST(monitor_, "GameWindow_BoardUpdates");
            auto gameWindow = std::make_unique<GameWindow>();
            gameWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
            Q_UNUSED(exposed);
            
            // Find game board buttons
            auto buttons = gameWindow->findChildren<QPushButton*>();
            std::vector<QPushButton*> boardButtons;
            
            // Assume first 9 buttons are the game board
            for (int i = 0; i < std::min(9, static_cast<int>(buttons.size())); ++i) {
                boardButtons.push_back(buttons[i]);
            }
            
            // Simulate rapid board updates during gameplay
            for (int game = 0; game < 20; ++game) {
                // Clear board
                for (auto* button : boardButtons) {
                    button->setText("");
                    button->setEnabled(true);
                }
                QCoreApplication::processEvents();
                
                // Simulate game moves
                for (int move = 0; move < 7; ++move) {
                    if (move < static_cast<int>(boardButtons.size())) {
                        boardButtons[move]->setText((move % 2 == 0) ? "X" : "O");
                        boardButtons[move]->setEnabled(false);
                        QCoreApplication::processEvents();
                    }
                }
            }
            
            gameWindow->close();
        }
        
        // Test 3: Game status updates
        {
            PERFORMANCE_TEST(monitor_, "GameWindow_StatusUpdates");
            auto gameWindow = std::make_unique<GameWindow>();
            gameWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
            Q_UNUSED(exposed);
            
            // Find status labels/displays
            auto labels = gameWindow->findChildren<QLabel*>();
            
            for (int i = 0; i < 50; ++i) {
                // Simulate status updates during game
                QString status = QString("Player %1's Turn").arg((i % 2) + 1);
                for (auto* label : labels) {
                    if (label->objectName().contains("status", Qt::CaseInsensitive) ||
                        label->text().contains("Player", Qt::CaseInsensitive)) {
                        label->setText(status);
                        break;
                    }
                }
                QCoreApplication::processEvents();
            }
            
            gameWindow->close();
        }
    }
    
    void testGameHistoryGUIPerformance() {
        std::cout << "Testing GameHistoryGUI Performance..." << std::endl;
        
        // Test 1: GameHistoryGUI creation with real data
        {
            PERFORMANCE_TEST(monitor_, "GameHistoryGUI_Creation_WithData");
            for (int i = 0; i < 10; ++i) {
                auto historyGUI = std::make_unique<GameHistoryGUI>(testHistory_.get(), "player1");
                historyGUI->show();
                bool exposed = QTest::qWaitForWindowExposed(historyGUI.get());
                Q_UNUSED(exposed);
                historyGUI->close();
                QCoreApplication::processEvents();
            }
        }
        
        // Test 2: Game history data loading and display
        {
            PERFORMANCE_TEST(monitor_, "GameHistoryGUI_DataLoading");
            auto historyGUI = std::make_unique<GameHistoryGUI>(testHistory_.get(), "player1");
            historyGUI->show();
            bool exposed = QTest::qWaitForWindowExposed(historyGUI.get());
            Q_UNUSED(exposed);
            
            // Force refresh multiple times to test data loading
            for (int i = 0; i < 15; ++i) {
                historyGUI->setCurrentUser("player1");
                QCoreApplication::processEvents();
                historyGUI->setCurrentUser("player2");
                QCoreApplication::processEvents();
            }
            
            historyGUI->close();
        }
        
        // Test 3: Game history scrolling and navigation
        {
            PERFORMANCE_TEST(monitor_, "GameHistoryGUI_Navigation");
            auto historyGUI = std::make_unique<GameHistoryGUI>(testHistory_.get(), "player1");
            historyGUI->show();
            bool exposed = QTest::qWaitForWindowExposed(historyGUI.get());
            Q_UNUSED(exposed);
            
            auto treeWidget = historyGUI->findChild<QTreeWidget*>();
            if (treeWidget && treeWidget->topLevelItemCount() > 0) {
                // Simulate browsing through game history
                for (int i = 0; i < std::min(25, treeWidget->topLevelItemCount()); ++i) {
                    auto item = treeWidget->topLevelItem(i);
                    treeWidget->setCurrentItem(item);
                    QCoreApplication::processEvents();
                    
                    // Simulate viewing game details
                    QTest::mouseDClick(treeWidget->viewport(), Qt::LeftButton,
                                      Qt::NoModifier, treeWidget->visualItemRect(item).center());
                    QCoreApplication::processEvents();
                }
            }
            
            historyGUI->close();
        }
    }
    
    void testGameBoardInteractions() {
        std::cout << "Testing Game Board Interactions..." << std::endl;
        
        // Test 1: Board cell click response time
        {
            PERFORMANCE_TEST(monitor_, "GameBoard_CellClickResponse");
            auto gameWindow = std::make_unique<GameWindow>();
            gameWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
            Q_UNUSED(exposed);
            
            auto buttons = gameWindow->findChildren<QPushButton*>();
            std::vector<QPushButton*> boardButtons;
            
            // Get board buttons (assume first 9 are board cells)
            for (int i = 0; i < std::min(9, static_cast<int>(buttons.size())); ++i) {
                boardButtons.push_back(buttons[i]);
            }
            
            // Test rapid clicking on board cells
            for (int i = 0; i < 50; ++i) {
                if (!boardButtons.empty()) {
                    QPushButton* button = boardButtons[i % boardButtons.size()];
                    QTest::mouseClick(button, Qt::LeftButton);
                    QCoreApplication::processEvents();
                }
            }
            
            gameWindow->close();
        }
        
        // Test 2: Complete game sequence timing
        {
            PERFORMANCE_TEST(monitor_, "GameBoard_CompleteGameSequence");
            auto gameWindow = std::make_unique<GameWindow>();
            gameWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
            Q_UNUSED(exposed);
            
            auto buttons = gameWindow->findChildren<QPushButton*>();
            std::vector<QPushButton*> boardButtons;
            
            for (int i = 0; i < std::min(9, static_cast<int>(buttons.size())); ++i) {
                boardButtons.push_back(buttons[i]);
            }
            
            // Simulate 10 complete games
            for (int game = 0; game < 10; ++game) {
                // Reset board state
                for (auto* button : boardButtons) {
                    button->setText("");
                    button->setEnabled(true);
                }
                QCoreApplication::processEvents();
                
                // Play a typical 5-move game
                std::vector<int> moves = {4, 0, 1, 8, 7}; // Center, corner, edge, corner, edge
                for (int moveIndex : moves) {
                    if (moveIndex < static_cast<int>(boardButtons.size())) {
                        QTest::mouseClick(boardButtons[moveIndex], Qt::LeftButton);
                        QCoreApplication::processEvents();
                    }
                }
            }
            
            gameWindow->close();
        }
    }
    
    void testAuthenticationWorkflow() {
        std::cout << "Testing Authentication Workflow..." << std::endl;
        
        // Test 1: Complete login workflow
        {
            PERFORMANCE_TEST(monitor_, "Auth_CompleteLoginWorkflow");
            for (int i = 0; i < 10; ++i) {
                auto loginPage = std::make_unique<LoginPage>(testAuth_.get());
                loginPage->show();
                bool exposed = QTest::qWaitForWindowExposed(loginPage.get());
                Q_UNUSED(exposed);
                
                // Fill username and password
                auto lineEdits = loginPage->findChildren<QLineEdit*>();
                if (lineEdits.size() >= 2) {
                    QTest::keyClicks(lineEdits[0], "player1");
                    QTest::keyClicks(lineEdits[1], "password123");
                    
                    // Find and click login button
                    auto buttons = loginPage->findChildren<QPushButton*>();
                    for (auto* button : buttons) {
                        if (button->text().contains("Login", Qt::CaseInsensitive) ||
                            button->text().contains("Sign In", Qt::CaseInsensitive)) {
                            QTest::mouseClick(button, Qt::LeftButton);
                            break;
                        }
                    }
                }
                
                QCoreApplication::processEvents();
                loginPage->close();
            }
        }
        
        // Test 2: Registration workflow
        {
            PERFORMANCE_TEST(monitor_, "Auth_RegistrationWorkflow");
            for (int i = 0; i < 5; ++i) {
                auto loginPage = std::make_unique<LoginPage>(testAuth_.get());
                loginPage->show();
                bool exposed = QTest::qWaitForWindowExposed(loginPage.get());
                Q_UNUSED(exposed);
                
                // Find registration controls
                auto lineEdits = loginPage->findChildren<QLineEdit*>();
                auto buttons = loginPage->findChildren<QPushButton*>();
                
                if (lineEdits.size() >= 2) {
                    QString newUsername = QString("newuser%1").arg(i);
                    QString newPassword = QString("newpass%1").arg(i);
                    
                    QTest::keyClicks(lineEdits[0], newUsername);
                    QTest::keyClicks(lineEdits[1], newPassword);
                    
                    // Look for register button
                    for (auto* button : buttons) {
                        if (button->text().contains("Register", Qt::CaseInsensitive) ||
                            button->text().contains("Sign Up", Qt::CaseInsensitive)) {
                            QTest::mouseClick(button, Qt::LeftButton);
                            break;
                        }
                    }
                }
                
                QCoreApplication::processEvents();
                loginPage->close();
            }
        }
    }
    
    void testGameNavigationWorkflow() {
        std::cout << "Testing Game Navigation Workflow..." << std::endl;
        
        // Test 1: Main menu to game navigation
        {
            PERFORMANCE_TEST(monitor_, "Navigation_MainMenuToGame");
            for (int i = 0; i < 15; ++i) {
                auto mainWindow = std::make_unique<MainWindow>();
                mainWindow->show();
                bool exposed = QTest::qWaitForWindowExposed(mainWindow.get());
                Q_UNUSED(exposed);
                
                // Find "New Game" or "Play" button
                auto buttons = mainWindow->findChildren<QPushButton*>();
                for (auto* button : buttons) {
                    if (button->text().contains("New Game", Qt::CaseInsensitive) ||
                        button->text().contains("Play", Qt::CaseInsensitive) ||
                        button->text().contains("Start", Qt::CaseInsensitive)) {
                        QTest::mouseClick(button, Qt::LeftButton);
                        QCoreApplication::processEvents();
                        break;
                    }
                }
                
                mainWindow->close();
            }
        }
        
        // Test 2: Game to history navigation
        {
            PERFORMANCE_TEST(monitor_, "Navigation_GameToHistory");
            for (int i = 0; i < 10; ++i) {
                auto gameWindow = std::make_unique<GameWindow>();
                gameWindow->show();
                bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
                Q_UNUSED(exposed);
                
                // Find "History" or "View Games" button
                auto buttons = gameWindow->findChildren<QPushButton*>();
                for (auto* button : buttons) {
                    if (button->text().contains("History", Qt::CaseInsensitive) ||
                        button->text().contains("Games", Qt::CaseInsensitive) ||
                        button->text().contains("Past", Qt::CaseInsensitive)) {
                        QTest::mouseClick(button, Qt::LeftButton);
                        QCoreApplication::processEvents();
                        break;
                    }
                }
                
                gameWindow->close();
            }
        }
    }
    
    void testCompleteGamePlaythrough() {
        std::cout << "Testing Complete Game Playthrough..." << std::endl;
        
        // Test 1: Full game session from start to finish
        {
            PERFORMANCE_TEST(monitor_, "CompleteGame_FullSession");
            for (int session = 0; session < 5; ++session) {
                // Start with main window
                auto mainWindow = std::make_unique<MainWindow>();
                mainWindow->show();
                bool exposed = QTest::qWaitForWindowExposed(mainWindow.get());
                Q_UNUSED(exposed);
                QCoreApplication::processEvents();
                
                // Navigate to game
                auto gameWindow = std::make_unique<GameWindow>();
                gameWindow->show();
                exposed = QTest::qWaitForWindowExposed(gameWindow.get());
                Q_UNUSED(exposed);
                
                // Play a complete game
                auto buttons = gameWindow->findChildren<QPushButton*>();
                std::vector<QPushButton*> boardButtons;
                for (int i = 0; i < std::min(9, static_cast<int>(buttons.size())); ++i) {
                    boardButtons.push_back(buttons[i]);
                }
                
                // Simulate realistic game play
                std::vector<int> gameSequence = {4, 0, 1, 8, 7, 2, 5}; // Realistic game
                for (int move : gameSequence) {
                    if (move < static_cast<int>(boardButtons.size())) {
                        QTest::mouseClick(boardButtons[move], Qt::LeftButton);
                        QCoreApplication::processEvents();
                        QTest::qWait(50); // Simulate thinking time
                    }
                }
                
                gameWindow->close();
                mainWindow->close();
            }
        }
        
        // Test 2: Multiple consecutive games
        {
            PERFORMANCE_TEST(monitor_, "CompleteGame_MultipleGames");
            auto gameWindow = std::make_unique<GameWindow>();
            gameWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
            Q_UNUSED(exposed);
            
            auto buttons = gameWindow->findChildren<QPushButton*>();
            std::vector<QPushButton*> boardButtons;
            for (int i = 0; i < std::min(9, static_cast<int>(buttons.size())); ++i) {
                boardButtons.push_back(buttons[i]);
            }
            
            // Play 8 quick games in succession
            std::vector<std::vector<int>> gamePatterns = {
                {4, 0, 1, 8, 7},           // X wins
                {0, 4, 1, 5, 2},           // X wins
                {4, 0, 3, 1, 6, 2},        // O wins
                {0, 1, 2, 4, 3, 5, 6, 7, 8}, // Draw
                {4, 2, 0, 6, 8},           // X wins
                {1, 4, 7, 5, 6, 3},        // O wins
                {0, 4, 2, 1, 6, 7},        // O wins
                {4, 0, 8, 1, 5, 2}         // X wins
            };
            
            for (const auto& pattern : gamePatterns) {
                // Reset board
                for (auto* button : boardButtons) {
                    button->setText("");
                    button->setEnabled(true);
                }
                QCoreApplication::processEvents();
                
                // Play the pattern
                for (int move : pattern) {
                    if (move < static_cast<int>(boardButtons.size())) {
                        QTest::mouseClick(boardButtons[move], Qt::LeftButton);
                        QCoreApplication::processEvents();
                    }
                }
                
                // Find and click "New Game" button if exists
                for (auto* button : buttons) {
                    if (button->text().contains("New", Qt::CaseInsensitive) &&
                        (button->text().contains("Game", Qt::CaseInsensitive) ||
                         button->text().contains("Reset", Qt::CaseInsensitive))) {
                        QTest::mouseClick(button, Qt::LeftButton);
                        QCoreApplication::processEvents();
                        break;
                    }
                }
            }
            
            gameWindow->close();
        }
    }
    
    void testGameHistoryBrowsing() {
        std::cout << "Testing Game History Browsing..." << std::endl;
        
        // Test 1: Browsing through game history
        {
            PERFORMANCE_TEST(monitor_, "GameHistory_BrowsingPerformance");
            auto historyGUI = std::make_unique<GameHistoryGUI>(testHistory_.get(), "player1");
            historyGUI->show();
            bool exposed = QTest::qWaitForWindowExposed(historyGUI.get());
            Q_UNUSED(exposed);
            
            auto treeWidget = historyGUI->findChild<QTreeWidget*>();
            if (treeWidget) {
                // Scroll through all games
                for (int i = 0; i < treeWidget->topLevelItemCount(); ++i) {
                    auto item = treeWidget->topLevelItem(i);
                    treeWidget->setCurrentItem(item);
                    treeWidget->scrollToItem(item);
                    QCoreApplication::processEvents();
                }
                
                // Sort by different columns
                for (int col = 0; col < treeWidget->columnCount(); ++col) {
                    treeWidget->sortByColumn(col, Qt::AscendingOrder);
                    QCoreApplication::processEvents();
                    treeWidget->sortByColumn(col, Qt::DescendingOrder);
                    QCoreApplication::processEvents();
                }
            }
            
            historyGUI->close();
        }
        
        // Test 2: Filtering and searching games
        {
            PERFORMANCE_TEST(monitor_, "GameHistory_FilteringAndSearch");
            auto historyGUI = std::make_unique<GameHistoryGUI>(testHistory_.get(), "player1");
            historyGUI->show();
            bool exposed = QTest::qWaitForWindowExposed(historyGUI.get());
            Q_UNUSED(exposed);
            
            // Switch between different players/filters
            for (int i = 0; i < 20; ++i) {
                QString player = (i % 2 == 0) ? "player1" : "player2";
                historyGUI->setCurrentUser(player);
                QCoreApplication::processEvents();
            }
            
            historyGUI->close();
        }
    }
    
    void testMultipleGameSessions() {
        std::cout << "Testing Multiple Game Sessions..." << std::endl;
        
        // Test 1: Concurrent window handling
        {
            PERFORMANCE_TEST(monitor_, "MultipleSession_ConcurrentWindows");
            std::vector<std::unique_ptr<QWidget>> windows;
            
            // Create multiple game-related windows
            for (int i = 0; i < 5; ++i) {
                auto gameWindow = std::make_unique<GameWindow>();
                gameWindow->show();
                bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
                Q_UNUSED(exposed);
                windows.push_back(std::move(gameWindow));
                
                auto historyGUI = std::make_unique<GameHistoryGUI>(testHistory_.get(), "player1");
                historyGUI->show();
                exposed = QTest::qWaitForWindowExposed(historyGUI.get());
                Q_UNUSED(exposed);
                windows.push_back(std::move(historyGUI));
                
                QCoreApplication::processEvents();
            }
            
            // Interact with all windows
            for (auto& window : windows) {
                window->raise();
                window->activateWindow();
                QCoreApplication::processEvents();
            }
            
            // Close all windows
            windows.clear();
        }
    }
    
    void testMemoryUsageDuringGameplay() {
        std::cout << "Testing Memory Usage During Gameplay..." << std::endl;
        
        // Test 1: Memory usage with extended gameplay
        {
            PERFORMANCE_TEST(monitor_, "Memory_ExtendedGameplay");
            auto gameWindow = std::make_unique<GameWindow>();
            gameWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
            Q_UNUSED(exposed);
            
            auto buttons = gameWindow->findChildren<QPushButton*>();
            std::vector<QPushButton*> boardButtons;
            for (int i = 0; i < std::min(9, static_cast<int>(buttons.size())); ++i) {
                boardButtons.push_back(buttons[i]);
            }
            
            // Play many games to test memory usage
            for (int game = 0; game < 100; ++game) {
                // Reset board
                for (auto* button : boardButtons) {
                    button->setText("");
                    button->setEnabled(true);
                }
                
                // Quick game
                std::vector<int> quickGame = {4, 0, 1, 8, 7};
                for (int move : quickGame) {
                    if (move < static_cast<int>(boardButtons.size())) {
                        boardButtons[move]->setText((move % 2 == 0) ? "X" : "O");
                        boardButtons[move]->setEnabled(false);
                    }
                }
                
                if (game % 10 == 0) {
                    QCoreApplication::processEvents();
                }
            }
            
            QTest::qWait(100); // Let memory settle
            gameWindow->close();
        }
        
        // Test 2: Memory usage with large game history
        {
            PERFORMANCE_TEST(monitor_, "Memory_LargeGameHistory");
            auto historyGUI = std::make_unique<GameHistoryGUI>(testHistory_.get(), "player1");
            historyGUI->show();
            bool exposed = QTest::qWaitForWindowExposed(historyGUI.get());
            Q_UNUSED(exposed);
            
            // Load and display large amounts of game data
            for (int i = 0; i < 20; ++i) {
                historyGUI->setCurrentUser("player1");
                QCoreApplication::processEvents();
                historyGUI->setCurrentUser("player2");
                QCoreApplication::processEvents();
            }
            
            QTest::qWait(100);
            historyGUI->close();
        }
    }
    
    void testConcurrentGameOperations() {
        std::cout << "Testing Concurrent Game Operations..." << std::endl;
        
        // Test 1: Multiple rapid UI updates
        {
            PERFORMANCE_TEST(monitor_, "Concurrent_RapidUIUpdates");
            auto gameWindow = std::make_unique<GameWindow>();
            gameWindow->show();
            bool exposed = QTest::qWaitForWindowExposed(gameWindow.get());
            Q_UNUSED(exposed);
            
            auto buttons = gameWindow->findChildren<QPushButton*>();
            auto labels = gameWindow->findChildren<QLabel*>();
            
            // Rapid updates to multiple UI elements
            for (int i = 0; i < 100; ++i) {
                // Update board buttons
                for (int j = 0; j < std::min(9, static_cast<int>(buttons.size())); ++j) {
                    QString text = (i + j) % 3 == 0 ? "X" : (i + j) % 3 == 1 ? "O" : "";
                    buttons[j]->setText(text);
                }
                
                // Update status labels
                for (auto* label : labels) {
                    label->setText(QString("Update %1").arg(i));
                }
                
                if (i % 10 == 0) {
                    QCoreApplication::processEvents();
                }
            }
            
            gameWindow->close();
        }
    }
    
    void generateReport() {
        std::cout << std::endl << "=== Tic-Tac-Toe GUI Performance Results ===" << std::endl;
        
        auto summary = monitor_.generateSummary();
        
        std::cout << "Total GUI Tests: " << summary.totalTests << std::endl;
        std::cout << "Successful: " << summary.successfulTests << std::endl;
        std::cout << "Failed: " << summary.failedTests << std::endl;
        std::cout << std::endl;
        
        std::cout << "GUI Response Time Statistics:" << std::endl;
        std::cout << "  Average: " << std::fixed << std::setprecision(2) 
                  << summary.avgResponseTime << " ms" << std::endl;
        std::cout << "  Min: " << summary.minResponseTime << " ms" << std::endl;
        std::cout << "  Max: " << summary.maxResponseTime << " ms" << std::endl;
        std::cout << std::endl;
        
        std::cout << "GUI Memory Usage Statistics:" << std::endl;
        std::cout << "  Average: " << std::fixed << std::setprecision(2) 
                  << summary.avgMemoryUsage << " MB" << std::endl;
        std::cout << "  Peak: " << summary.maxMemoryUsage << " MB" << std::endl;
        std::cout << std::endl;
        
        // Export detailed results
        monitor_.exportToCSV("gui_performance_results.csv");
        monitor_.exportToJSON("gui_performance_results.json");
        
        std::cout << "Tic-Tac-Toe GUI performance results exported to:" << std::endl;
        std::cout << "  - gui_performance_results.csv" << std::endl;
        std::cout << "  - gui_performance_results.json" << std::endl;
        std::cout << std::endl;
        
        // Print individual test results
        std::cout << "Individual GUI Test Results:" << std::endl;
        std::cout << std::string(90, '-') << std::endl;
        std::cout << std::left << std::setw(40) << "Test Name" 
                  << std::setw(15) << "Time (ms)" 
                  << std::setw(15) << "Memory (MB)" 
                  << std::setw(15) << "CPU (%)" 
                  << "Status" << std::endl;
        std::cout << std::string(90, '-') << std::endl;
        
        for (const auto& result : monitor_.getResults()) {
            std::cout << std::left << std::setw(40) << result.testName
                      << std::fixed << std::setprecision(2)
                      << std::setw(15) << result.responseTimeMs
                      << std::setw(15) << result.memoryUsageMB
                      << std::setw(15) << result.cpuUsagePercent
                      << (result.success ? "PASS" : "FAIL");
            
            if (!result.success && !result.errorMessage.empty()) {
                std::cout << " (" << result.errorMessage << ")";
            }
            
            std::cout << std::endl;
        }
        
        // Tic-Tac-Toe specific recommendations
        std::cout << std::endl << "Tic-Tac-Toe GUI Performance Insights:" << std::endl;
        if (summary.maxMemoryUsage > 100) {
            std::cout << "- Game windows may be using excessive memory" << std::endl;
        }
        if (summary.maxResponseTime > 100) {
            std::cout << "- Some game interactions are slow, consider optimization" << std::endl;
        }
        if (summary.avgResponseTime > 50) {
            std::cout << "- Average game response time could be improved" << std::endl;
        }
        std::cout << "- For smooth gameplay: aim for <50ms board update time" << std::endl;
        std::cout << "- Game window creation should be <500ms for good UX" << std::endl;
        std::cout << "- History browsing should remain responsive with large datasets" << std::endl;
    }
};

int main(int argc, char *argv[]) {
    // Initialize QApplication for GUI components
    QApplication app(argc, argv);
    
    // Set up for headless testing if needed
    if (qEnvironmentVariableIsSet("QT_QPA_PLATFORM")) {
        std::cout << "Running in headless mode with platform: " 
                  << qgetenv("QT_QPA_PLATFORM").toStdString() << std::endl;
    }
    
    try {
        TicTacToeGUIPerformanceTest testSuite(&app);
        testSuite.runAllTests();
        
        std::cout << std::endl << "Tic-Tac-Toe GUI Performance testing completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Tic-Tac-Toe GUI Performance testing failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Tic-Tac-Toe GUI Performance testing failed with unknown exception!" << std::endl;
        return 1;
    }
}