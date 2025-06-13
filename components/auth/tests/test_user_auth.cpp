#include <gtest/gtest.h>
#include "auth/user_auth.h"
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

class UserAuthTest : public ::testing::Test {
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
        
        dbName = "test_auth_" + std::to_string(timestamp) + "_" + 
                 std::to_string(process_id) + "_" + std::to_string(thread_id) + "_" + 
                 std::to_string(dis(gen)) + ".db";
        auth = std::make_unique<UserAuth>(dbName);
    }
    void TearDown() override {
        // Properly close database before deletion
        auth.reset();
        
        // Use robust cleanup utility
        if (!robustDatabaseCleanup(dbName)) {
            // Log warning but don't fail test
            std::cerr << "Warning: Failed to clean up test database: " << dbName << std::endl;
        }
    }
    std::unique_ptr<UserAuth> auth;
    std::string dbName;
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
