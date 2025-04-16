#include <gtest/gtest.h>
#include "../src/auth/user_auth.h"

class UserAuthTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Use a temporary test database
        auth = std::make_unique<UserAuth>("test.db");
    }

    void TearDown() override {
        // Clean up database file
        std::remove("test.db");
    }

    std::unique_ptr<UserAuth> auth;
};

TEST_F(UserAuthTest, RegisterNewUser) {
    EXPECT_TRUE(auth->registerUser("testuser", "testpass"));
    EXPECT_FALSE(auth->registerUser("testuser", "testpass")); // Duplicate user
}

TEST_F(UserAuthTest, LoginSuccess) {
    auth->registerUser("testuser", "testpass");
    EXPECT_TRUE(auth->login("testuser", "testpass"));
}

TEST_F(UserAuthTest, LoginWrongPassword) {
    auth->registerUser("testuser", "testpass");
    EXPECT_FALSE(auth->login("testuser", "wrongpass"));
}

TEST_F(UserAuthTest, LoginNonExistentUser) {
    EXPECT_FALSE(auth->login("nonexistent", "testpass"));
}