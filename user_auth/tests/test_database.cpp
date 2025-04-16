#include <gtest/gtest.h>
#include "../src/db/database.h"

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