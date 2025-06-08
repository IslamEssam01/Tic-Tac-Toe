#include <gtest/gtest.h>
#include <algorithm>
#include "AI.h"
#include "Board.h"

// Test AI winning immediately (X, row)
TEST(AITest, ImmediateWinXRow) {
    Board board;
    board.makeMove(0, 0, Player::X);
    board.makeMove(1, 0, Player::O);
    board.makeMove(0, 1, Player::X);
    board.makeMove(1, 1, Player::O);
    // Board:
    // X X _
    // O O _
    // _ _ _
    auto move = findBestMove(board, Player::X);
    EXPECT_EQ(move, std::make_pair(0, 2));  // X wins at (0,2)
}

// Test AI winning immediately (O, column)
TEST(AITest, ImmediateWinOCol) {
    Board board;
    board.makeMove(0, 0, Player::X);
    board.makeMove(0, 1, Player::O);
    board.makeMove(2, 2, Player::X);
    board.makeMove(1, 1, Player::O);
    // Board:
    // X O _
    // _ O _
    // _ _ X
    auto move = findBestMove(board, Player::O);
    EXPECT_EQ(move, std::make_pair(2, 1));  // O wins at (2,1)
}

// Test AI blocking opponent (X blocks O)
TEST(AITest, BlockOpponentX) {
    Board board;
    board.makeMove(0, 0, Player::X);
    board.makeMove(1, 1, Player::O);
    board.makeMove(2, 2, Player::X);
    board.makeMove(2, 1, Player::O);
    // Board:
    // X _ _
    // _ O _
    // _ O X
    auto move = findBestMove(board, Player::X);
    EXPECT_EQ(move, std::make_pair(0, 1));  // X blocks O's win at (0,1)
}

// Test AI blocking opponent (O blocks X, diagonal)
TEST(AITest, BlockOpponentODiag) {
    Board board;
    board.makeMove(0, 0, Player::X);
    board.makeMove(1, 0, Player::O);
    board.makeMove(1, 1, Player::X);
    // Board:
    // X _ _
    // O X _
    // _ _ _
    auto move = findBestMove(board, Player::O);
    EXPECT_EQ(move, std::make_pair(2, 2));  // O blocks X's diagonal win at (2,2)
}

// Test AI choosing center on empty board (X)
TEST(AITest, CenterMoveX) {
    Board board;
    // Empty board
    auto move = findBestMove(board, Player::X);
    EXPECT_EQ(move, std::make_pair(1, 1));  // X takes center
}

// Test AI choosing corner after center taken (O)
TEST(AITest, CornerMoveO) {
    Board board;
    board.makeMove(1, 1, Player::X);
    // Board:
    // _ _ _
    // _ X _
    // _ _ _
    auto move = findBestMove(board, Player::O);
    std::vector<std::pair<int, int>> corners = {{0, 0}, {0, 2}, {2, 0}, {2, 2}};
    bool is_corner_move = std::any_of(corners.begin(), corners.end(),
        [&move](const auto& corner) {
            return corner.first == move.first && corner.second == move.second;
        });
    EXPECT_TRUE(is_corner_move);
}

// Test AI preventing fork (O)
TEST(AITest, PreventForkO) {
    Board board;
    board.makeMove(0, 0, Player::X);
    board.makeMove(1, 1, Player::O);
    board.makeMove(2, 2, Player::X);
    // Board:
    // X _ _
    // _ O _
    // _ _ X
    auto move = findBestMove(board, Player::O);
    std::vector<std::pair<int, int>> edges = {{0, 1}, {1, 0}, {1, 2}, {2, 1}};
    bool is_edge_move = std::any_of(edges.begin(), edges.end(),
        [&move](const auto& edge) {
            return edge.first == move.first && edge.second == move.second;
        });
    EXPECT_TRUE(is_edge_move);  // O must play an edge to prevent X's fork
}
