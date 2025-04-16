#include <gtest/gtest.h>
#include "game_history.h"

TEST(GameHistoryTest, StartNewGame) {
    GameHistory game;
    int game_id = game.startNewGame();
    EXPECT_EQ(game_id, 1);
    game_id = game.startNewGame();
    EXPECT_EQ(game_id, 2);
}

TEST(GameHistoryTest, AddMoveValid) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1);
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 1);
    EXPECT_EQ(moves[0].player, 'X');
    EXPECT_EQ(moves[0].position, 1);
    EXPECT_EQ(moves[0].move_number, 1);
}

TEST(GameHistoryTest, AddMoveInvalidPosition) {
    GameHistory game;
    int game_id = game.startNewGame();
    EXPECT_THROW(game.addMove(game_id, 'X', 10), std::runtime_error);
}

TEST(GameHistoryTest, AddMovePositionTaken) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1);
    EXPECT_THROW(game.addMove(game_id, 'O', 1), std::runtime_error);
}

TEST(GameHistoryTest, AddMoveInvalidPlayer) {
    GameHistory game;
    int game_id = game.startNewGame();
    EXPECT_THROW(game.addMove(game_id, 'Z', 1), std::runtime_error);
}