#include "test_welcome_page.h"
#include <QLabel>
#include <QPushButton>

void TestWelcomePage::initTestCase() {
    m_welcomePage = new WelcomePage();
}

void TestWelcomePage::cleanupTestCase() {
    delete m_welcomePage;
}

void TestWelcomePage::testSetUsername() {
    // Set the username
    m_welcomePage->setUsername("TestUser");
    
    // Find the welcome label
    QLabel* welcomeLabel = m_welcomePage->findChild<QLabel*>();
    
    // Verify the label text contains the username
    QVERIFY(welcomeLabel->text().contains("TestUser"));
}

void TestWelcomePage::testLogoutButton() {
    // Find the logout button
    QPushButton* logoutButton = m_welcomePage->findChild<QPushButton*>();
    
    // Set up signal spy
    QSignalSpy spy(m_welcomePage, SIGNAL(logout()));
    
    // Click the logout button
    QTest::mouseClick(logoutButton, Qt::LeftButton);
    
    // Verify logout signal was emitted
    QCOMPARE(spy.count(), 1);
}