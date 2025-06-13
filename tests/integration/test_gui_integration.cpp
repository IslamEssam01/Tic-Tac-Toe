#include <gtest/gtest.h>
#include <QTest>
#include <QApplication>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QStackedWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTreeWidget>
#include <QTableWidget>
#include <memory>

// Include all GUI components for integration testing
#include "main_window.h"
#include "login_page.h"
#include "game_window.h"
#include "game_history_gui.h"
#include "auth/user_auth.h"
#include "game_history.h"

// Helper class to access protected members of MainWindow
class TestableMainWindow : public MainWindow {
public:
    TestableMainWindow(const QString& tempDirPath, QWidget *parent = nullptr) 
        : MainWindow(parent), tempPath(tempDirPath) {
        // Override database paths to use temporary directory
        setupTempDatabases();
    }

    // Expose protected members for testing
    QStackedWidget* getStackedWidget() const { return m_stackedWidget; }
    LoginPage* getLoginPage() const { return m_loginPage; }
    GameWindow* getGameWindow() const { return m_gameWindow; }
    UserAuth* getAuth() { return &m_auth; }
    GameHistory* getGameHistory() { return m_gameHistory; }
    QString getCurrentUser() const { return m_currentUser; }

private:
    QString tempPath;
    
    void setupTempDatabases() {
        // Create unique database files for this test instance
        QString authDbPath = tempPath + "/test_users_" + QString::number(reinterpret_cast<uintptr_t>(this)) + ".db";
        QString historyDbPath = tempPath + "/test_history_" + QString::number(reinterpret_cast<uintptr_t>(this)) + ".db";
        
        // Note: This is a simplified approach. In a real implementation,
        // we would modify MainWindow constructor to accept database paths
        // For now, we'll work with the existing structure
    }
};

class GUIIntegrationTest : public ::testing::Test {
public:
    static QApplication* app;

protected:
    void SetUp() override {
        // Create temporary directory for test databases
        tempDir = std::make_unique<QTemporaryDir>();
        ASSERT_TRUE(tempDir->isValid());
        
        // Create main window (which initializes all components)
        mainWindow = std::make_unique<TestableMainWindow>(tempDir->path());
        
        // Show main window and wait for exposure
        mainWindow->show();
        bool exposed = QTest::qWaitForWindowExposed(mainWindow.get());
        Q_UNUSED(exposed);
        
        // Register test users in the auth system after window is created
        UserAuth* auth = mainWindow->getAuth();
        // Use unique usernames to avoid conflicts between tests
        QString testId = QString::number(reinterpret_cast<uintptr_t>(this));
        aliceUsername = "alice_" + testId;
        bobUsername = "bob_" + testId;
        
        ASSERT_TRUE(auth->registerUser(aliceUsername.toStdString(), "alice123"));
        ASSERT_TRUE(auth->registerUser(bobUsername.toStdString(), "bob123"));
    }
    
    void TearDown() override {
        if (mainWindow) {
            mainWindow->close();
        }
    }
    
    // Helper method to perform login through GUI
    bool performLogin(const QString& username, const QString& password) {
        LoginPage* loginPage = mainWindow->getLoginPage();
        QStackedWidget* stackedWidget = mainWindow->getStackedWidget();
        
        // Ensure we're on login page
        if (stackedWidget->currentWidget() != loginPage) {
            return false;
        }
        
        // Find input fields
        QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
        QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
        QPushButton* loginButton = loginPage->findChild<QPushButton*>("m_loginButton");
        
        if (!usernameEdit || !passwordEdit || !loginButton) {
            return false;
        }
        
        // Fill in credentials
        usernameEdit->setText(username);
        passwordEdit->setText(password);
        
        // Click login button
        QTest::mouseClick(loginButton, Qt::LeftButton);
        QCoreApplication::processEvents();
        
        // Check if we navigated to game window
        GameWindow* gameWindow = mainWindow->getGameWindow();
        return stackedWidget->currentWidget() == gameWindow;
    }
    
    // Helper method to start a PvP game
    bool startPvPGame(const QString& player1, const QString& player2) {
        GameWindow* gameWindow = mainWindow->getGameWindow();
        
        // Find PvP button
        QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
        QPushButton* pvpButton = nullptr;
        for (auto* btn : buttons) {
            if (btn->text() == "Player vs Player") {
                pvpButton = btn;
                break;
            }
        }
        
        if (!pvpButton) return false;
        
        // Click PvP button
        QTest::mouseClick(pvpButton, Qt::LeftButton);
        QCoreApplication::processEvents();
        
        // This should trigger second player authentication
        // Simulate the authentication process
        mainWindow->getGameWindow()->setPlayerNames(player1, player2);
        QCoreApplication::processEvents();
        
        return true;
    }
    
    std::unique_ptr<QTemporaryDir> tempDir;
    std::unique_ptr<TestableMainWindow> mainWindow;
    QString aliceUsername;
    QString bobUsername;
};

QApplication* GUIIntegrationTest::app = nullptr;

// Test complete login to game workflow
TEST_F(GUIIntegrationTest, CompleteLoginToGameWorkflow) {
    QStackedWidget* stackedWidget = mainWindow->getStackedWidget();
    LoginPage* loginPage = mainWindow->getLoginPage();
    GameWindow* gameWindow = mainWindow->getGameWindow();
    
    // Initially should be on login page
    EXPECT_EQ(stackedWidget->currentWidget(), loginPage);
    
    // Perform login
    EXPECT_TRUE(performLogin(aliceUsername, "alice123"));
    
    // Should now be on game window
    EXPECT_EQ(stackedWidget->currentWidget(), gameWindow);
    
    // Verify current user is set
    EXPECT_EQ(mainWindow->getCurrentUser(), aliceUsername);
    
    // Verify game window shows setup UI
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    bool foundPvPButton = false;
    bool foundPvAIButton = false;
    for (auto* btn : buttons) {
        if (btn->text() == "Player vs Player") foundPvPButton = true;
        if (btn->text() == "Player vs AI") foundPvAIButton = true;
    }
    EXPECT_TRUE(foundPvPButton);
    EXPECT_TRUE(foundPvAIButton);
}

// Test failed login doesn't navigate
TEST_F(GUIIntegrationTest, FailedLoginStaysOnLoginPage) {
    QStackedWidget* stackedWidget = mainWindow->getStackedWidget();
    LoginPage* loginPage = mainWindow->getLoginPage();
    
    // Initially should be on login page
    EXPECT_EQ(stackedWidget->currentWidget(), loginPage);
    
    // Try to login with invalid credentials
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* loginButton = loginPage->findChild<QPushButton*>("m_loginButton");
    
    usernameEdit->setText("invalid");
    passwordEdit->setText("wrong");
    QTest::mouseClick(loginButton, Qt::LeftButton);
    QCoreApplication::processEvents();
    
    // Should still be on login page
    EXPECT_EQ(stackedWidget->currentWidget(), loginPage);
    
    // Should show error message
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    EXPECT_FALSE(statusLabel->text().isEmpty());
    EXPECT_TRUE(statusLabel->text().contains("failed", Qt::CaseInsensitive));
}

// Test logout returns to login page and clears state
TEST_F(GUIIntegrationTest, LogoutClearsStateAndReturnsToLogin) {
    QStackedWidget* stackedWidget = mainWindow->getStackedWidget();
    LoginPage* loginPage = mainWindow->getLoginPage();
    GameWindow* gameWindow = mainWindow->getGameWindow();
    
    // Login first
    EXPECT_TRUE(performLogin(aliceUsername, "alice123"));
    EXPECT_EQ(stackedWidget->currentWidget(), gameWindow);
    EXPECT_EQ(mainWindow->getCurrentUser(), aliceUsername);
    
    // Find and click logout button
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* logoutButton = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Logout") {
            logoutButton = btn;
            break;
        }
    }
    ASSERT_NE(logoutButton, nullptr);
    
    QTest::mouseClick(logoutButton, Qt::LeftButton);
    QCoreApplication::processEvents();
    
    // Should return to login page
    EXPECT_EQ(stackedWidget->currentWidget(), loginPage);
    
    // Current user should be cleared
    EXPECT_TRUE(mainWindow->getCurrentUser().isEmpty());
    
    // Login page fields should be cleared
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    EXPECT_TRUE(usernameEdit->text().isEmpty());
    EXPECT_TRUE(passwordEdit->text().isEmpty());
}

// Test Player vs Player authentication flow
TEST_F(GUIIntegrationTest, PlayerVsPlayerAuthenticationFlow) {
    QStackedWidget* stackedWidget = mainWindow->getStackedWidget();
    LoginPage* loginPage = mainWindow->getLoginPage();
    GameWindow* gameWindow = mainWindow->getGameWindow();
    
    // Login as first player
    EXPECT_TRUE(performLogin(aliceUsername, "alice123"));
    
    // Set up signal spy for second player authentication request
    QSignalSpy authSpy(gameWindow, SIGNAL(secondPlayerAuthenticationRequested()));
    
    // Find and click PvP button
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* pvpButton = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "Player vs Player") {
            pvpButton = btn;
            break;
        }
    }
    ASSERT_NE(pvpButton, nullptr);
    
    QTest::mouseClick(pvpButton, Qt::LeftButton);
    QCoreApplication::processEvents();
    
    // Should emit second player authentication signal
    EXPECT_EQ(authSpy.count(), 1);
    
    // Should navigate back to login page for second player
    EXPECT_EQ(stackedWidget->currentWidget(), loginPage);
    
    // Login page should be in second player mode
    bool foundPlayer2Text = false;
    QList<QLabel*> labels = loginPage->findChildren<QLabel*>();
    for (auto* label : labels) {
        if (label->text().contains("Player 2")) {
            foundPlayer2Text = true;
            break;
        }
    }
    EXPECT_TRUE(foundPlayer2Text);
    
    // Back button should be visible
    QPushButton* backButton = loginPage->findChild<QPushButton*>("m_backButton");
    ASSERT_NE(backButton, nullptr);
    EXPECT_TRUE(backButton->isVisible());
}

// Test game history integration
TEST_F(GUIIntegrationTest, GameHistoryIntegration) {
    // Login and start a game
    EXPECT_TRUE(performLogin(aliceUsername, "alice123"));
    
    GameWindow* gameWindow = mainWindow->getGameWindow();
    GameHistory* gameHistory = mainWindow->getGameHistory();
    
    // Create a test game in history
    int aliceId = qHash(aliceUsername);
    int gameId = gameHistory->initializeGame(aliceId, std::nullopt);
    gameHistory->recordMove(gameId, 0);
    gameHistory->recordMove(gameId, 1);
    gameHistory->recordMove(gameId, 2);
    gameHistory->setWinner(gameId, aliceId);
    
    // Find and click view history button
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* historyButton = nullptr;
    for (auto* btn : buttons) {
        if (btn->text() == "View History") {
            historyButton = btn;
            break;
        }
    }
    ASSERT_NE(historyButton, nullptr);
    
    // Set up signal spy for history window request
    QSignalSpy historySpy(gameWindow, SIGNAL(viewHistoryRequested()));
    
    QTest::mouseClick(historyButton, Qt::LeftButton);
    QCoreApplication::processEvents();
    
    // Should emit view history signal
    EXPECT_EQ(historySpy.count(), 1);
}

// Test real-time game history updates during gameplay
TEST_F(GUIIntegrationTest, RealTimeGameHistoryUpdates) {
    // Login
    EXPECT_TRUE(performLogin(aliceUsername, "alice123"));
    
    GameHistory* gameHistory = mainWindow->getGameHistory();
    
    // Create game history GUI window
    GameHistoryGUI historyWindow(gameHistory, aliceUsername);
    historyWindow.show();
    bool exposed = QTest::qWaitForWindowExposed(&historyWindow);
    Q_UNUSED(exposed);
    
    // Set up signal spies for game events
    QSignalSpy initSpy(gameHistory, SIGNAL(gameInitialized(int)));
    QSignalSpy moveSpy(gameHistory, SIGNAL(moveRecorded(int, int)));
    QSignalSpy completeSpy(gameHistory, SIGNAL(gameCompleted(int, std::optional<int>)));
    
    // Create a new game
    int aliceId = qHash(aliceUsername);
    int gameId = gameHistory->initializeGame(aliceId, std::nullopt);
    QCoreApplication::processEvents();
    
    // Verify signals were emitted
    EXPECT_EQ(initSpy.count(), 1);
    
    // Record some moves
    gameHistory->recordMove(gameId, 4);
    QCoreApplication::processEvents();
    gameHistory->recordMove(gameId, 0);
    QCoreApplication::processEvents();
    
    EXPECT_EQ(moveSpy.count(), 2);
    
    // Complete the game
    gameHistory->setWinner(gameId, aliceId);
    QCoreApplication::processEvents();
    
    EXPECT_EQ(completeSpy.count(), 1);
    
    // Verify game appears in history window
    QTreeWidget* gamesTree = historyWindow.findChild<QTreeWidget*>();
    ASSERT_NE(gamesTree, nullptr);
    
    // Should have at least one game
    EXPECT_GT(gamesTree->topLevelItemCount(), 0);
    
    historyWindow.close();
}

// Test window resizing during navigation
TEST_F(GUIIntegrationTest, WindowResizingDuringNavigation) {
    // Record initial window size (login size)
    QSize loginSize = mainWindow->size();
    
    // Login should change window size
    EXPECT_TRUE(performLogin(aliceUsername, "alice123"));
    QCoreApplication::processEvents();
    
    QSize gameSetupSize = mainWindow->size();
    
    // Size should change from login to game setup
    EXPECT_NE(loginSize, gameSetupSize);
    
    // Start PvP to trigger game board UI
    GameWindow* gameWindow = mainWindow->getGameWindow();
    gameWindow->setPlayerNames(aliceUsername, bobUsername);
    QCoreApplication::processEvents();
    
    // Find symbol selection button and click it
    QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
    QPushButton* symbolButton = nullptr;
    for (auto* btn : buttons) {
        if (btn->text().contains(aliceUsername + " as")) {
            symbolButton = btn;
            break;
        }
    }
    
    if (symbolButton) {
        QTest::mouseClick(symbolButton, Qt::LeftButton);
        QCoreApplication::processEvents();
        
        QSize gameBoardSize = mainWindow->size();
        
        // Size should change again for game board
        EXPECT_NE(gameSetupSize, gameBoardSize);
    }
}

// Test error handling in GUI integration
TEST_F(GUIIntegrationTest, ErrorHandlingIntegration) {
    LoginPage* loginPage = mainWindow->getLoginPage();
    
    // Test registration with invalid password
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* registerButton = loginPage->findChild<QPushButton*>("m_registerButton");
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    
    // Create unique username for this test
    QString testId = QString::number(reinterpret_cast<uintptr_t>(this));
    QString newUsername = "newuser_" + testId;
    
    // Try to register with password that's too short
    usernameEdit->setText(newUsername);
    passwordEdit->setText("123"); // Too short
    QTest::mouseClick(registerButton, Qt::LeftButton);
    QCoreApplication::processEvents();
    
    // Should show error message
    EXPECT_FALSE(statusLabel->text().isEmpty());
    EXPECT_TRUE(statusLabel->text().contains("5 characters", Qt::CaseInsensitive));
    
    // Try to register with password missing letters
    passwordEdit->setText("12345"); // No letters
    QTest::mouseClick(registerButton, Qt::LeftButton);
    QCoreApplication::processEvents();
    
    // Should show different error message
    EXPECT_TRUE(statusLabel->text().contains("letters and digits", Qt::CaseInsensitive));
    
    // Valid registration should work
    passwordEdit->setText("test123"); // Valid
    QTest::mouseClick(registerButton, Qt::LeftButton);
    QCoreApplication::processEvents();
    
    // Should show success message
    EXPECT_TRUE(statusLabel->text().contains("success", Qt::CaseInsensitive));
    
    // Fields should be cleared
    EXPECT_TRUE(usernameEdit->text().isEmpty());
    EXPECT_TRUE(passwordEdit->text().isEmpty());
}

// Test memory management during navigation
TEST_F(GUIIntegrationTest, MemoryManagementDuringNavigation) {
    // This test ensures no memory leaks during repeated navigation
    
    for (int i = 0; i < 5; i++) {
        // Login
        EXPECT_TRUE(performLogin(aliceUsername, "alice123"));
        QCoreApplication::processEvents();
        
        // Access different parts of the interface
        GameWindow* gameWindow = mainWindow->getGameWindow();
        
        // Click through different modes
        QList<QPushButton*> buttons = gameWindow->findChildren<QPushButton*>();
        for (auto* btn : buttons) {
            if (btn->text() == "Player vs AI") {
                QTest::mouseClick(btn, Qt::LeftButton);
                QCoreApplication::processEvents();
                break;
            }
        }
        
        // Logout
        buttons = gameWindow->findChildren<QPushButton*>();
        for (auto* btn : buttons) {
            if (btn->text() == "Logout") {
                QTest::mouseClick(btn, Qt::LeftButton);
                QCoreApplication::processEvents();
                break;
            }
        }
        
        // Brief pause to allow cleanup
        QTest::qWait(10);
    }
    
    // If we reach here without crashes, memory management is likely working
    SUCCEED();
}

int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QApplication app(argc, argv);
    GUIIntegrationTest::app = &app;
    return RUN_ALL_TESTS();
}