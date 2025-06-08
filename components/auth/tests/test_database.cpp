#include <gtest/gtest.h>
#include "db/database.h"
#include <cstdio>

class DatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        db = std::make_unique<Database>("test.db");
        db->init();
    }
    void TearDown() override {
        std::remove("test.db");
    }
    std::unique_ptr<Database> db;
};

TEST_F(DatabaseTest, AddAndCheckUser) {
    EXPECT_TRUE(db->addUser("testuser", "hashedpass"));
    EXPECT_TRUE(db->userExists("testuser"));
    EXPECT_FALSE(db->userExists("nonexistent"));
}

TEST_F(DatabaseTest, GetUserPassword) {
    db->addUser("testuser", "hashedpass");
    std::string password;
    EXPECT_TRUE(db->getUserPassword("testuser", password));
    EXPECT_EQ(password, "hashedpass");
    EXPECT_FALSE(db->getUserPassword("nonexistent", password));
}

TEST_F(DatabaseTest, UserDoesNotExistInitially) {
    EXPECT_FALSE(db->userExists("someone"));
}

TEST_F(DatabaseTest, AddDuplicateUserFails) {
    EXPECT_TRUE(db->addUser("dupuser", "pass1"));
    EXPECT_FALSE(db->addUser("dupuser", "pass2"));
}

TEST_F(DatabaseTest, CaseSensitivityOfUsernames) {
    EXPECT_TRUE(db->addUser("CaseUser", "pass"));
    EXPECT_FALSE(db->userExists("caseuser"));
}

TEST_F(DatabaseTest, RejectEmptyUsernameOrPassword) {
    EXPECT_FALSE(db->addUser("", ""));
    EXPECT_FALSE(db->userExists(""));
    std::string pw;
    EXPECT_FALSE(db->getUserPassword("", pw));
}
