#include <gtest/gtest.h>
#include "db/database.h"
#include <cstdio>
#include <filesystem>
#include <string>
#include <random>
#include <chrono>
#include <thread>
#ifdef _WIN32
#include <process.h>
#define getpid _getpid
#else
#include <unistd.h>
#endif

// Utility function for robust database cleanup
bool robustDatabaseCleanup(const std::string& dbPath) {
    // Try multiple deletion attempts with different methods
    for (int attempts = 0; attempts < 5; ++attempts) {
        std::error_code ec;
        
        // Method 1: std::filesystem::remove
        std::filesystem::remove(dbPath, ec);
        if (!ec && !std::filesystem::exists(dbPath)) {
            return true;
        }
        
        // Method 2: Traditional C remove
        if (std::remove(dbPath.c_str()) == 0 && !std::filesystem::exists(dbPath)) {
            return true;
        }
        
        // Wait and try again
        std::this_thread::sleep_for(std::chrono::milliseconds(50 * (attempts + 1)));
    }
    
    // Final check - if file doesn't exist, consider it success
    return !std::filesystem::exists(dbPath);
}

class DatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Generate highly unique database filename to avoid conflicts
        auto now = std::chrono::high_resolution_clock::now();
        auto timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
        auto thread_id = std::hash<std::thread::id>{}(std::this_thread::get_id());
        auto process_id = static_cast<unsigned long>(getpid());
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(100000, 999999);
        
        dbName = "test_db_" + std::to_string(timestamp) + "_" + 
                 std::to_string(process_id) + "_" + std::to_string(thread_id) + "_" + 
                 std::to_string(dis(gen)) + ".db";
        
        db = std::make_unique<Database>(dbName);
        db->init();
    }
    void TearDown() override {
        // Properly close database before deletion
        db.reset();
        
        // Use robust cleanup utility
        if (!robustDatabaseCleanup(dbName)) {
            // Log warning but don't fail test
            std::cerr << "Warning: Failed to clean up test database: " << dbName << std::endl;
        }
    }
    std::unique_ptr<Database> db;
    std::string dbName;
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
