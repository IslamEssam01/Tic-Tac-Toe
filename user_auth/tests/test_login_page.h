#ifndef TEST_LOGIN_PAGE_H
#define TEST_LOGIN_PAGE_H

#include <QTest>
#include <QSignalSpy>
#include "../src/login_page.h"
#include "../src/auth/user_auth.h"

// Mock UserAuth for testing
class MockUserAuth : public UserAuth {
public:
    MockUserAuth() : UserAuth(":memory:") {} // Use in-memory SQLite database

    // Override methods for testing
    bool registerUser(const std::string& username, const std::string& password) override {
        if (username == "existinguser") {
            return false; // Simulate user already exists
        }
        if (password.length() < 5) {
            return false; // Password too short
        }
        
        bool hasLetter = false, hasDigit = false;
        for (char c : password) {
            if (std::isalpha(static_cast<unsigned char>(c))) hasLetter = true;
            if (std::isdigit(static_cast<unsigned char>(c))) hasDigit = true;
        }
        if (!hasLetter || !hasDigit) {
            return false; // Password missing letter or digit
        }
        
        m_registered_users[username] = password;
        return true;
    }

    bool login(const std::string& username, const std::string& password) override {
        auto it = m_registered_users.find(username);
        if (it == m_registered_users.end()) {
            return false; // User not found
        }
        return it->second == password; // Check password
    }

private:
    std::map<std::string, std::string> m_registered_users;
};

class TestLoginPage : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testRegisterSuccess();
    void testRegisterFailureExistingUser();
    void testRegisterFailureInvalidPassword();
    void testLoginSuccess();
    void testLoginFailure();
    void testClearFields();

private:
    LoginPage* m_loginPage;
    MockUserAuth* m_mockAuth;
};

#endif