#include <gtest/gtest.h>
#include "../src/auth/user_auth.h"
#include <cstdio>

class UserAuthTest : public ::testing::Test {
protected:
    void SetUp() override {
        auth = std::make_unique<UserAuth>("test.db");
    }
    void TearDown() override {
        std::remove("test.db");
    }
    std::unique_ptr<UserAuth> auth;
};

TEST_F(UserAuthTest, RegisterNewUser) {
    EXPECT_TRUE(auth->registerUser("testuser", "testpass1"));
    EXPECT_FALSE(auth->registerUser("testuser", "testpass2"));
}

TEST_F(UserAuthTest, LoginSuccess) {
    EXPECT_TRUE(auth->registerUser("testuser", "testpass1"));
    EXPECT_TRUE(auth->login("testuser", "testpass1"));
}

TEST_F(UserAuthTest, LoginWrongPassword) {
    EXPECT_TRUE(auth->registerUser("testuser", "testpass1"));
    EXPECT_FALSE(auth->login("testuser", "wrongpass"));
}

TEST_F(UserAuthTest, LoginNonExistentUser) {
    EXPECT_FALSE(auth->login("nonexistent", "testpass"));
}

TEST_F(UserAuthTest, RejectEmptyCredentials) {
    EXPECT_FALSE(auth->registerUser("", "pass"));
    EXPECT_FALSE(auth->registerUser("user", ""));
    EXPECT_FALSE(auth->login("", "pass"));
    EXPECT_FALSE(auth->login("user", ""));
}

TEST_F(UserAuthTest, RegisterMultipleDistinctUsers) {
    EXPECT_TRUE(auth->registerUser("user1", "pass1"));
    EXPECT_TRUE(auth->registerUser("user2", "pass2"));
    EXPECT_TRUE(auth->login("user1", "pass1"));
    EXPECT_TRUE(auth->login("user2", "pass2"));
}

TEST_F(UserAuthTest, CaseSensitiveLogin) {
    EXPECT_TRUE(auth->registerUser("AuthUser", "password1"));
    EXPECT_TRUE(auth->login("AuthUser", "password1"));
    EXPECT_FALSE(auth->login("authuser", "password1"));
}

TEST_F(UserAuthTest, SpecialCharacterCredentials) {
    EXPECT_TRUE(auth->registerUser("user!@#", "p@ss!3"));
    EXPECT_TRUE(auth->login("user!@#", "p@ss!3"));
}
