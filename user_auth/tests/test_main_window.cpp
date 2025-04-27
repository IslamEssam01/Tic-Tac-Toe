#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QTest>
#include <QSignalSpy>
#include <QApplication>
#define TESTING
#include "../src/main_window.h"
#include "../src/login_page.h"
#include "../src/welcome_page.h"

class MainWindowTest : public ::testing::Test {
public:
    static QApplication* app; // Moved to public

protected:
    void SetUp() override {
        mainWindow = new TestableMainWindow();
    }
    
    void TearDown() override {
        delete mainWindow;
    }
    
    TestableMainWindow* mainWindow;
};

// Static member definition
QApplication* MainWindowTest::app = nullptr;

// Initialize QApplication before tests
int main(int argc, char *argv[]) {
    ::testing::InitGoogleTest(&argc, argv);
    QApplication app(argc, argv); // Initialize QApplication
    MainWindowTest::app = &app; // Store app instance
    return RUN_ALL_TESTS();
}

TEST_F(MainWindowTest, InitialState) {
    // Check that the initial page shown is the login page
    QStackedWidget* stackedWidget = mainWindow->getStackedWidget();
    ASSERT_NE(stackedWidget, nullptr);
    
    LoginPage* loginPage = mainWindow->getLoginPage();
    EXPECT_EQ(stackedWidget->currentWidget(), loginPage);
}

TEST_F(MainWindowTest, LoginNavigatesToGameWindow) {
    // We need to fake a successful login
    LoginPage* loginPage = mainWindow->getLoginPage();
    GameWindow* gameWindow = mainWindow->getGameWindow();
    QStackedWidget* stackedWidget = mainWindow->getStackedWidget();
    
    // Emit the loginSuccessful signal
    emit loginPage->loginSuccessful("TestUser");
    
    // Check that we've navigated to the game window
    EXPECT_EQ(stackedWidget->currentWidget(), gameWindow);
}

TEST_F(MainWindowTest, LogoutNavigatesToLoginPage) {
    // First, navigate to welcome page
    LoginPage* loginPage = mainWindow->getLoginPage();
    WelcomePage* welcomePage = mainWindow->getWelcomePage();
    QStackedWidget* stackedWidget = mainWindow->getStackedWidget();
    
    // Set current widget to welcome page
    stackedWidget->setCurrentWidget(welcomePage);
    
    // Emit the logout signal
    emit welcomePage->logout();
    
    // Check that we've navigated back to the login page
    EXPECT_EQ(stackedWidget->currentWidget(), loginPage);
}
