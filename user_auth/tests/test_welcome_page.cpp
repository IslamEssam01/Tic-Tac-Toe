#include <gtest/gtest.h>
#include <QTest>
#include <QSignalSpy>
#include <QApplication>
#include "../src/welcome_page.h"

class WelcomePageTest : public ::testing::Test {
public:
    static QApplication* app; // Moved to public

protected:
    void SetUp() override {
        welcomePage = new WelcomePage();
    }
    
    void TearDown() override {
        delete welcomePage;
    }
    
    WelcomePage* welcomePage;
};

// Static member definition
QApplication* WelcomePageTest::app = nullptr;

// Initialize QApplication before tests
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QApplication app(argc, argv); // Initialize QApplication
    WelcomePageTest::app = &app; // Store app instance
    return RUN_ALL_TESTS();
}

TEST_F(WelcomePageTest, InitialState) {
    // Check that the welcome label exists but doesn't have a specific user set
    QLabel* welcomeLabel = welcomePage->findChild<QLabel*>("m_welcomeLabel");
    ASSERT_NE(welcomeLabel, nullptr);
    EXPECT_FALSE(welcomeLabel->text().contains("Hello, ", Qt::CaseInsensitive));
}

TEST_F(WelcomePageTest, SetUsername) {
    // Set a username and check if the welcome message is updated
    welcomePage->setUsername("TestUser");
    
    QLabel* welcomeLabel = welcomePage->findChild<QLabel*>("m_welcomeLabel");
    ASSERT_NE(welcomeLabel, nullptr);
    EXPECT_EQ(welcomeLabel->text(), "Hello, TestUser!");
}

TEST_F(WelcomePageTest, LogoutButtonClicked) {
    // Set up signal spy to catch the logout signal
    QSignalSpy spy(welcomePage, SIGNAL(logout()));
    
    // Find and click the logout button
    QPushButton* logoutButton = welcomePage->findChild<QPushButton*>("m_logoutButton");
    ASSERT_NE(logoutButton, nullptr);
    
    // Click the logout button
    QTest::mouseClick(logoutButton, Qt::LeftButton);
    
    // Check that the signal was emitted
    ASSERT_EQ(spy.count(), 1);
}