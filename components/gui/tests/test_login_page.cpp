#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QTest>
#include <QSignalSpy>
#include <QApplication>
#include "login_page.h"

// Mock UserAuth class for testing
class MockUserAuth : public UserAuth {
public:
    MockUserAuth() : UserAuth("test.db") {}
    virtual ~MockUserAuth() = default;
    
    MOCK_METHOD(bool, login, (const std::string& username, const std::string& password), (override));
    MOCK_METHOD(bool, registerUser, (const std::string& username, const std::string& password), (override));
};

class LoginPageTest : public ::testing::Test {
public:
    static QApplication* app; // Public for main function access

protected:
    void SetUp() override {
        mockAuth = new MockUserAuth();
        loginPage = new LoginPage(mockAuth);
        loginPage->show(); // Show the window to ensure proper widget visibility
        QTest::qWaitForWindowExposed(loginPage);
    }
    
    void TearDown() override {
        delete loginPage;
        delete mockAuth;
    }
    
    MockUserAuth* mockAuth;
    LoginPage* loginPage;
};

// Static member definition
QApplication* LoginPageTest::app = nullptr;

// Initialize QApplication before tests
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QApplication app(argc, argv); // Initialize QApplication
    LoginPageTest::app = &app; // Store app instance
    return RUN_ALL_TESTS();
}

TEST_F(LoginPageTest, InitialState) {
    // Check that the login page is properly initialized with empty fields
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    ASSERT_NE(usernameEdit, nullptr);
    EXPECT_TRUE(usernameEdit->text().isEmpty());
    
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    ASSERT_NE(passwordEdit, nullptr);
    EXPECT_TRUE(passwordEdit->text().isEmpty());
    
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    ASSERT_NE(statusLabel, nullptr);
    EXPECT_TRUE(statusLabel->text().isEmpty());
}

TEST_F(LoginPageTest, SuccessfulLogin) {
    // Set up the mock to return true for login
    EXPECT_CALL(*mockAuth, login(testing::_, testing::_))
        .WillOnce(testing::Return(true));
    
    // Set up signal spy to catch the loginSuccessful signal
    QSignalSpy spy(loginPage, SIGNAL(loginSuccessful(const QString&)));
    
    // Fill in the login form
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* loginButton = loginPage->findChild<QPushButton*>("m_loginButton");
    ASSERT_NE(loginButton, nullptr);
    
    usernameEdit->setText("testuser");
    passwordEdit->setText("password123");
    
    // Click the login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Check that the signal was emitted with the correct username
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    EXPECT_EQ(arguments.at(0).toString(), "testuser");
    
    // Check that the status label is empty (no error)
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    EXPECT_TRUE(statusLabel->text().isEmpty());
}

TEST_F(LoginPageTest, FailedLogin) {
    // Set up the mock to return false for login
    EXPECT_CALL(*mockAuth, login(testing::_, testing::_))
        .WillOnce(testing::Return(false));

    // Set up signal spy to ensure the loginSuccessful signal is NOT emitted
    QSignalSpy spy(loginPage, SIGNAL(loginSuccessful(const QString&)));
    
    // Fill in the login form
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* loginButton = loginPage->findChild<QPushButton*>("m_loginButton");
    ASSERT_NE(loginButton, nullptr);
    
    usernameEdit->setText("testuser");
    passwordEdit->setText("wrongpassword");
    
    // Click the login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Check that the signal was NOT emitted
    EXPECT_EQ(spy.count(), 0);
    
    // Check that the status label shows an error
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    EXPECT_FALSE(statusLabel->text().isEmpty());
    EXPECT_TRUE(statusLabel->text().contains("failed", Qt::CaseInsensitive)); 
}

TEST_F(LoginPageTest, EmptyFieldsLogin) {
    // No auth method call should happen with empty fields
    EXPECT_CALL(*mockAuth, login(testing::_, testing::_))
        .Times(0);
    
    // Set up signal spy
    QSignalSpy spy(loginPage, SIGNAL(loginSuccessful(const QString&)));
    
    // Leave fields empty
    QPushButton* loginButton = loginPage->findChild<QPushButton*>("m_loginButton");
    ASSERT_NE(loginButton, nullptr);
    
    // Click the login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Check that the signal was NOT emitted
    EXPECT_EQ(spy.count(), 0);
    
    // Check that the status label shows an error about empty fields
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    EXPECT_FALSE(statusLabel->text().isEmpty());
    EXPECT_TRUE(statusLabel->text().contains("empty", Qt::CaseInsensitive));
}

TEST_F(LoginPageTest, SuccessfulRegistration) {
    // Set up the mock to return true for registration
    EXPECT_CALL(*mockAuth, registerUser(testing::_, testing::_))
        .WillOnce(testing::Return(true));
    
    // Fill in the registration form
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* registerButton = loginPage->findChild<QPushButton*>("m_registerButton");
    ASSERT_NE(registerButton, nullptr);
    
    usernameEdit->setText("newuser");
    passwordEdit->setText("newpass123");
    
    // Click the register button
    QTest::mouseClick(registerButton, Qt::LeftButton);
    
    // Check that the status label shows success message
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    EXPECT_FALSE(statusLabel->text().isEmpty());
    EXPECT_TRUE(statusLabel->text().contains("success", Qt::CaseInsensitive));
    
    // Check fields were cleared
    EXPECT_TRUE(usernameEdit->text().isEmpty());
    EXPECT_TRUE(passwordEdit->text().isEmpty());
}

TEST_F(LoginPageTest, FailedRegistration) {
    // Set up the mock to return false for registration
    EXPECT_CALL(*mockAuth, registerUser(testing::_, testing::_))
        .WillOnce(testing::Return(false));
    
    // Fill in the registration form
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* registerButton = loginPage->findChild<QPushButton*>("m_registerButton");
    ASSERT_NE(registerButton, nullptr);
    
    usernameEdit->setText("existinguser");
    passwordEdit->setText("pass123"); // Changed to meet validation: 5+ chars, letters + digits
    
    // Click the register button
    QTest::mouseClick(registerButton, Qt::LeftButton);
    
    // Check that the status label shows failure message
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    EXPECT_FALSE(statusLabel->text().isEmpty());
    EXPECT_TRUE(statusLabel->text().contains("failed", Qt::CaseInsensitive));
    
    // Check fields were not cleared after failure
    EXPECT_EQ(usernameEdit->text(), "existinguser");
    EXPECT_EQ(passwordEdit->text(), "pass123");
}

TEST_F(LoginPageTest, ClearFieldsMethod) {
    // Find widgets
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    
    // Check that widgets exist before proceeding
    ASSERT_NE(usernameEdit, nullptr);
    ASSERT_NE(passwordEdit, nullptr);
    ASSERT_NE(statusLabel, nullptr);
    
    // Set initial values
    usernameEdit->setText("testuser");
    passwordEdit->setText("testpass");
    statusLabel->setText("Some status");
    
    // Call clearFields
    loginPage->clearFields();
    
    // Check that input fields were cleared
    EXPECT_TRUE(usernameEdit->text().isEmpty());
    EXPECT_TRUE(passwordEdit->text().isEmpty());
    // Status label is not cleared by clearFields
    EXPECT_EQ(statusLabel->text(), "Some status");
}



TEST_F(LoginPageTest, SecondPlayerLoginMode) {
    // Set to second player login mode
    loginPage->setMode(LoginPageMode::SecondPlayerLogin);
    
    // Process events to ensure UI updates
    QCoreApplication::processEvents();
    
    // Check that back button is visible and register button is also visible (second player can register too!)
    QPushButton* backButton = loginPage->findChild<QPushButton*>("m_backButton");
    QPushButton* registerButton = loginPage->findChild<QPushButton*>("m_registerButton");
    
    ASSERT_NE(backButton, nullptr);
    ASSERT_NE(registerButton, nullptr);
    
    EXPECT_TRUE(backButton->isVisible());
    EXPECT_TRUE(registerButton->isVisible()); // Changed: register should be visible for second player
    
    // Check title is updated for second player
    bool foundTitleLabel = false;
    QList<QLabel*> labels = loginPage->findChildren<QLabel*>();
    for (auto* label : labels) {
        if (label->text().contains("Player 2")) {
            foundTitleLabel = true;
            break;
        }
    }
    EXPECT_TRUE(foundTitleLabel);
}

TEST_F(LoginPageTest, SecondPlayerLoginSuccessful) {
    // Set to second player mode
    loginPage->setMode(LoginPageMode::SecondPlayerLogin);
    
    // Set up the mock to return true for login
    EXPECT_CALL(*mockAuth, login(testing::_, testing::_))
        .WillOnce(testing::Return(true));
    
    // Set up signal spy for second player login
    QSignalSpy spy(loginPage, SIGNAL(secondPlayerLoginSuccessful(const QString&)));
    
    // Fill in the login form
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* loginButton = loginPage->findChild<QPushButton*>("m_loginButton");
    
    usernameEdit->setText("player2");
    passwordEdit->setText("password123");
    
    // Click the login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Check that the second player signal was emitted
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    EXPECT_EQ(arguments.at(0).toString(), "player2");
}

TEST_F(LoginPageTest, BackButtonSignal) {
    // Set to second player mode
    loginPage->setMode(LoginPageMode::SecondPlayerLogin);
    
    // Set up signal spy for back request
    QSignalSpy spy(loginPage, SIGNAL(backRequested()));
    
    // Find and click back button
    QPushButton* backButton = loginPage->findChild<QPushButton*>("m_backButton");
    ASSERT_NE(backButton, nullptr);
    
    QTest::mouseClick(backButton, Qt::LeftButton);
    
    // Check that the back signal was emitted
    EXPECT_EQ(spy.count(), 1);
}

TEST_F(LoginPageTest, InitialLoginMode) {
    // Ensure we start in initial login mode
    loginPage->setMode(LoginPageMode::InitialLogin);
    
    // Process events to ensure UI updates
    QCoreApplication::processEvents();
    
    // Check that register button is visible and back button is hidden
    QPushButton* backButton = loginPage->findChild<QPushButton*>("m_backButton");
    QPushButton* registerButton = loginPage->findChild<QPushButton*>("m_registerButton");
    
    ASSERT_NE(backButton, nullptr);
    ASSERT_NE(registerButton, nullptr);
    
    EXPECT_FALSE(backButton->isVisible());
    EXPECT_TRUE(registerButton->isVisible());
}

TEST_F(LoginPageTest, SecondPlayerSameUsernameLoginPrevented) {
    // Set up the mock to return true for login
    EXPECT_CALL(*mockAuth, login(testing::_, testing::_))
        .WillOnce(testing::Return(true));
    
    // Set to second player mode with first player name "player1"
    loginPage->setMode(LoginPageMode::SecondPlayerLogin, "player1");
    
    // Set up signal spy to ensure the secondPlayerLoginSuccessful signal is NOT emitted
    QSignalSpy spy(loginPage, SIGNAL(secondPlayerLoginSuccessful(const QString&)));
    
    // Fill in the login form with same username as first player
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* loginButton = loginPage->findChild<QPushButton*>("m_loginButton");
    
    usernameEdit->setText("player1"); // Same as first player
    passwordEdit->setText("password123");
    
    // Click the login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Check that the signal was NOT emitted
    EXPECT_EQ(spy.count(), 0);
    
    // Check that the status label shows an error about same username
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    EXPECT_FALSE(statusLabel->text().isEmpty());
    EXPECT_TRUE(statusLabel->text().contains("cannot use the same username", Qt::CaseInsensitive));
}

TEST_F(LoginPageTest, SecondPlayerSameUsernameRegistrationPrevented) {
    // Set up the mock to return true for registration
    EXPECT_CALL(*mockAuth, registerUser(testing::_, testing::_))
        .Times(0); // Should not be called due to validation
    
    // Set to second player mode with first player name "player1"
    loginPage->setMode(LoginPageMode::SecondPlayerLogin, "player1");
    
    // Fill in the registration form with same username as first player
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* registerButton = loginPage->findChild<QPushButton*>("m_registerButton");
    
    usernameEdit->setText("player1"); // Same as first player
    passwordEdit->setText("pass123abc"); // Valid password
    
    // Click the register button
    QTest::mouseClick(registerButton, Qt::LeftButton);
    
    // Check that the status label shows an error about same username
    QLabel* statusLabel = loginPage->findChild<QLabel*>("m_statusLabel");
    EXPECT_FALSE(statusLabel->text().isEmpty());
    EXPECT_TRUE(statusLabel->text().contains("cannot register with the same username", Qt::CaseInsensitive));
}

TEST_F(LoginPageTest, SecondPlayerDifferentUsernameAllowed) {
    // Set up the mock to return true for login
    EXPECT_CALL(*mockAuth, login(testing::_, testing::_))
        .WillOnce(testing::Return(true));
    
    // Set to second player mode with first player name "player1"
    loginPage->setMode(LoginPageMode::SecondPlayerLogin, "player1");
    
    // Set up signal spy for second player login
    QSignalSpy spy(loginPage, SIGNAL(secondPlayerLoginSuccessful(const QString&)));
    
    // Fill in the login form with different username
    QLineEdit* usernameEdit = loginPage->findChild<QLineEdit*>("m_usernameEdit");
    QLineEdit* passwordEdit = loginPage->findChild<QLineEdit*>("m_passwordEdit");
    QPushButton* loginButton = loginPage->findChild<QPushButton*>("m_loginButton");
    
    usernameEdit->setText("player2"); // Different from first player
    passwordEdit->setText("password123");
    
    // Click the login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Check that the signal was emitted successfully
    ASSERT_EQ(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    EXPECT_EQ(arguments.at(0).toString(), "player2");
}