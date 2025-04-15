#include <gtest/gtest.h>
#include "Board.h"

// Group 1: isCellEmpty
TEST(BoardTest, CellIsInitiallyEmpty) {
    Board board;
    EXPECT_TRUE(board.isCellEmpty(0, 0));
    EXPECT_TRUE(board.isCellEmpty(2, 2));
}

TEST(BoardTest, CellIsNotEmptyAfterMove) {
    Board board;
    board.makeMove(1, 1, Player::X);
    EXPECT_FALSE(board.isCellEmpty(1, 1));
}

// Group 2: isValidMove
TEST(BoardTest, ValidMoveInsideGrid) {
    Board board;
    EXPECT_TRUE(board.isValidMove(0, 0));
}

TEST(BoardTest, InvalidMoveOutOfBounds) {
    Board board;
    EXPECT_FALSE(board.isValidMove(-1, 0));
    EXPECT_FALSE(board.isValidMove(3, 1));
    EXPECT_FALSE(board.isValidMove(0, 3));
}

TEST(BoardTest, InvalidMoveOnOccupiedCell) {
    Board board;
    board.makeMove(0, 0, Player::X);
    EXPECT_FALSE(board.isValidMove(0, 0));
}

// Group 3: makeMove
TEST(BoardTest, MakeValidMoveReturnsTrue) {
    Board board;
    EXPECT_TRUE(board.makeMove(0, 0, Player::X));
}

TEST(BoardTest, MakeInvalidMoveReturnsFalse) {
    Board board;
    board.makeMove(0, 0, Player::X);
    EXPECT_FALSE(board.makeMove(0, 0, Player::O));
}

// Group 4: isFull
TEST(BoardTest, BoardIsNotFullInitially) {
    Board board;
    EXPECT_FALSE(board.isFull());
}

TEST(BoardTest, BoardIsFullAfterAllMoves) {
    Board board;
    Player p = Player::X;
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            board.makeMove(i, j, p);
    EXPECT_TRUE(board.isFull());
}

// Group 5: reset
TEST(BoardTest, ResetEmptiesBoard) {
    Board board;
    board.makeMove(0, 0, Player::X);
    board.reset();
    EXPECT_TRUE(board.isCellEmpty(0, 0));
}

// Group 6: checkWinner - rows
TEST(BoardTest, CheckWinnerRow) {
    Board board;
    board.makeMove(1, 0, Player::O);
    board.makeMove(1, 1, Player::O);
    board.makeMove(1, 2, Player::O);
    WinInfo win = board.checkWinner();
    EXPECT_EQ(win.winner, Player::O);
    EXPECT_EQ(win.type, "row");
    EXPECT_EQ(win.index, 1);
    EXPECT_EQ(win.winCells.size(), 3);
}

// Group 7: checkWinner - columns
TEST(BoardTest, CheckWinnerColumn) {
    Board board;
    board.makeMove(0, 2, Player::X);
    board.makeMove(1, 2, Player::X);
    board.makeMove(2, 2, Player::X);
    WinInfo win = board.checkWinner();
    EXPECT_EQ(win.winner, Player::X);
    EXPECT_EQ(win.type, "col");
    EXPECT_EQ(win.index, 2);
}

// Group 8: checkWinner - main diagonal
TEST(BoardTest, CheckWinnerMainDiagonal) {
    Board board;
    board.makeMove(0, 0, Player::X);
    board.makeMove(1, 1, Player::X);
    board.makeMove(2, 2, Player::X);
    WinInfo win = board.checkWinner();
    EXPECT_EQ(win.winner, Player::X);
    EXPECT_EQ(win.type, "diag");
}

// Group 9: checkWinner - anti diagonal
TEST(BoardTest, CheckWinnerAntiDiagonal) {
    Board board;
    board.makeMove(0, 2, Player::O);
    board.makeMove(1, 1, Player::O);
    board.makeMove(2, 0, Player::O);
    WinInfo win = board.checkWinner();
    EXPECT_EQ(win.winner, Player::O);
    EXPECT_EQ(win.type, "anti-diag");
}

// Group 10: checkWinner - no winner
TEST(BoardTest, NoWinner) {
    Board board;
    board.makeMove(0, 0, Player::X);
    board.makeMove(0, 1, Player::O);
    board.makeMove(0, 2, Player::X);
    WinInfo win = board.checkWinner();
    EXPECT_EQ(win.winner, Player::None);
    EXPECT_EQ(win.type, "none");
    EXPECT_EQ(win.winCells.empty(), true);
}

// Group 11: isGameOver
TEST(BoardTest, GameOverWhenWinnerExists) {
    Board board;
    board.makeMove(2, 0, Player::X);
    board.makeMove(2, 1, Player::X);
    board.makeMove(2, 2, Player::X);
    EXPECT_TRUE(board.isGameOver());
}

TEST(BoardTest, GameOverWhenBoardIsFull) {
    Board board;
    Player moves[3][3] = {
        {Player::X, Player::O, Player::X},
        {Player::X, Player::O, Player::O},
        {Player::O, Player::X, Player::X}
    };
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            board.makeMove(i, j, moves[i][j]);

    EXPECT_TRUE(board.isGameOver());
}

TEST(BoardTest, GameIsNotOverInitially) {
    Board board;
    EXPECT_FALSE(board.isGameOver());
}