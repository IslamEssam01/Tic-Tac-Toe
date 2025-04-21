#include "test_login_page.h"
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>

void TestLoginPage::initTestCase() {
    m_mockAuth = new MockUserAuth();
    m_loginPage = new LoginPage(m_mockAuth);
    
    // Pre-register a test user
    m_mockAuth->registerUser("testuser", "Password123");
}

void TestLoginPage::cleanupTestCase() {
    delete m_loginPage;
    delete m_mockAuth;
}

void TestLoginPage::testRegisterSuccess() {
    // Find UI elements
    QLineEdit* usernameEdit = m_loginPage->findChild<QLineEdit*>();
    QLineEdit* passwordEdit = m_loginPage->findChildren<QLineEdit*>().at(1);
    QPushButton* registerButton = m_loginPage->findChildren<QPushButton*>().at(1); // Assuming register is second button
    
    // Set text in the fields
    usernameEdit->setText("newuser");
    passwordEdit->setText("NewPass123");
    
    // Click the register button
    QTest::mouseClick(registerButton, Qt::LeftButton);
    
    // Verify fields are cleared (indicating success)
    QVERIFY(usernameEdit->text().isEmpty());
    QVERIFY(passwordEdit->text().isEmpty());
    
    // Verify user can now log in
    usernameEdit->setText("newuser");
    passwordEdit->setText("NewPass123");
    
    QPushButton* loginButton = m_loginPage->findChildren<QPushButton*>().first();
    
    // Set up signal spy to catch the loginSuccessful signal
    QSignalSpy spy(m_loginPage, SIGNAL(loginSuccessful(QString)));
    
    // Click login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Verify login signal was emitted
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString("newuser"));
}

void TestLoginPage::testRegisterFailureExistingUser() {
    // Find UI elements
    QLineEdit* usernameEdit = m_loginPage->findChild<QLineEdit*>();
    QLineEdit* passwordEdit = m_loginPage->findChildren<QLineEdit*>().at(1);
    QPushButton* registerButton = m_loginPage->findChildren<QPushButton*>().at(1);
    QLabel* statusLabel = m_loginPage->findChild<QLabel*>();
    
    // Set text in the fields
    usernameEdit->setText("existinguser");  // This username should fail
    passwordEdit->setText("Password123");
    
    // Click the register button
    QTest::mouseClick(registerButton, Qt::LeftButton);
    
    // Verify error message appears
    QVERIFY(!statusLabel->text().isEmpty());
}

void TestLoginPage::testRegisterFailureInvalidPassword() {
    // Find UI elements
    QLineEdit* usernameEdit = m_loginPage->findChild<QLineEdit*>();
    QLineEdit* passwordEdit = m_loginPage->findChildren<QLineEdit*>().at(1);
    QPushButton* registerButton = m_loginPage->findChildren<QPushButton*>().at(1);
    QLabel* statusLabel = m_loginPage->findChild<QLabel*>();
    
    // Test too short password
    usernameEdit->setText("validuser");
    passwordEdit->setText("abc");
    
    // Click the register button
    QTest::mouseClick(registerButton, Qt::LeftButton);
    
    // Verify error message appears
    QVERIFY(!statusLabel->text().isEmpty());
    
    // Test password without digit
    statusLabel->clear();
    passwordEdit->setText("Password");
    
    // Click the register button
    QTest::mouseClick(registerButton, Qt::LeftButton);
    
    // Verify error message appears
    QVERIFY(!statusLabel->text().isEmpty());
}

void TestLoginPage::testLoginSuccess() {
    // Find UI elements
    QLineEdit* usernameEdit = m_loginPage->findChild<QLineEdit*>();
    QLineEdit* passwordEdit = m_loginPage->findChildren<QLineEdit*>().at(1);
    QPushButton* loginButton = m_loginPage->findChildren<QPushButton*>().first();
    
    // Set text in the fields for our pre-registered user
    usernameEdit->setText("testuser");
    passwordEdit->setText("Password123");
    
    // Set up signal spy
    QSignalSpy spy(m_loginPage, SIGNAL(loginSuccessful(QString)));
    
    // Click login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Verify login signal was emitted
    QCOMPARE(spy.count(), 1);
    QList<QVariant> arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).toString(), QString("testuser"));
}

void TestLoginPage::testLoginFailure() {
    // Find UI elements
    QLineEdit* usernameEdit = m_loginPage->findChild<QLineEdit*>();
    QLineEdit* passwordEdit = m_loginPage->findChildren<QLineEdit*>().at(1);
    QPushButton* loginButton = m_loginPage->findChildren<QPushButton*>().first();
    QLabel* statusLabel = m_loginPage->findChild<QLabel*>();
    
    // Clear any previous status
    statusLabel->clear();
    
    // Set text in the fields with wrong password
    usernameEdit->setText("testuser");
    passwordEdit->setText("WrongPassword");
    
    // Set up signal spy
    QSignalSpy spy(m_loginPage, SIGNAL(loginSuccessful(QString)));
    
    // Click login button
    QTest::mouseClick(loginButton, Qt::LeftButton);
    
    // Verify no login signal was emitted
    QCOMPARE(spy.count(), 0);
    
    // Verify error message appears
    QVERIFY(!statusLabel->text().isEmpty());
}

void TestLoginPage::testClearFields() {
    // Find UI elements
    QLineEdit* usernameEdit = m_loginPage->findChild<QLineEdit*>();
    QLineEdit* passwordEdit = m_loginPage->findChildren<QLineEdit*>().at(1);
    
    // Set text in the fields
    usernameEdit->setText("someuser");
    passwordEdit->setText("somepassword");
    
    // Call clear method
    m_loginPage->clearFields();
    
    // Verify fields are cleared
    QVERIFY(usernameEdit->text().isEmpty());
    QVERIFY(passwordEdit->text().isEmpty());
}