#include "test_main_window.h"
#include <QStackedWidget>

void TestMainWindow::initTestCase() {
    m_mainWindow = new MainWindow();
}

void TestMainWindow::cleanupTestCase() {
    delete m_mainWindow;
}

void TestMainWindow::testPageTransition() {
    // Find the stacked widget
    QStackedWidget* stackedWidget = m_mainWindow->findChild<QStackedWidget*>();
    
    // Find the login page
    LoginPage* loginPage = m_mainWindow->findChild<LoginPage*>();
    
    // Find the welcome page
    WelcomePage* welcomePage = m_mainWindow->findChild<WelcomePage*>();
    
    // Verify initial page is login page
    QCOMPARE(stackedWidget->currentWidget(), qobject_cast<QWidget*>(loginPage));
    
    // Emit login successful signal
    emit loginPage->loginSuccessful("TestUser");
    
    // Verify page changed to welcome page
    QCOMPARE(stackedWidget->currentWidget(), qobject_cast<QWidget*>(welcomePage));
    
    // Verify username was set correctly on welcome page
    QLabel* welcomeLabel = welcomePage->findChild<QLabel*>();
    QVERIFY(welcomeLabel->text().contains("TestUser"));
    
    // Emit logout signal
    emit welcomePage->logout();
    
    // Verify page changed back to login page
    QCOMPARE(stackedWidget->currentWidget(), qobject_cast<QWidget*>(loginPage));
}