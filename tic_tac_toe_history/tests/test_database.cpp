#include <gtest/gtest.h>
#include "database.h"
#include <filesystem>

class DatabaseTest : public ::testing::Test {
protected:
    void SetUp() override {
        db_ = new Database("test.db");
    }
    void TearDown() override {
        delete db_;
        std::filesystem::remove("test.db");
    }
    Database* db_;
};

TEST_F(DatabaseTest, SaveAndRetrieveMove) {
    Move move{1, 'X', 5, 1};
    db_->saveMove(move);
    auto moves = db_->getGameMoves(1);
    ASSERT_EQ(moves.size(), 1);
    EXPECT_EQ(moves[0].game_id, 1);
    EXPECT_EQ(moves[0].player, 'X');
    EXPECT_EQ(moves[0].position, 5);
    EXPECT_EQ(moves[0].move_number, 1);
}

TEST_F(DatabaseTest, RetrieveEmptyGame) {
    auto moves = db_->getGameMoves(999);
    EXPECT_TRUE(moves.empty());
}

TEST_F(DatabaseTest, SaveMultipleMoves) {
    Move move1{1, 'X', 1, 1};
    Move move2{1, 'O', 2, 2};
    db_->saveMove(move1);
    db_->saveMove(move2);
    auto moves = db_->getGameMoves(1);
    ASSERT_EQ(moves.size(), 2);
    EXPECT_EQ(moves[0].position, 1);
    EXPECT_EQ(moves[1].position, 2);
}