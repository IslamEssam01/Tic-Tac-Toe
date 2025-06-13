#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <QTest>
#include <QSignalSpy>
#include <QApplication>
#define TESTING // Define TESTING before including main_window.h if needed
#include "main_window.h"
#include "login_page.h"
#include "game_window.h"

// Helper class to access protected members of MainWindow
class TestableMainWindow : public MainWindow {
public:
    TestableMainWindow(QWidget *parent = nullptr) : MainWindow(parent) {}

    // Expose protected members for testing
    QStackedWidget* getStackedWidget() const { return m_stackedWidget; }
    LoginPage* getLoginPage() const { return m_loginPage; }
    GameWindow* getGameWindow() const { return m_gameWindow; }
};


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
    ASSERT_NE(loginPage, nullptr); // Ensure loginPage is not null
    EXPECT_EQ(stackedWidget->currentWidget(), loginPage);
}
