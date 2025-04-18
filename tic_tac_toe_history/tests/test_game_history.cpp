#include <gtest/gtest.h>
#include "game_history.h"

// Test case for starting a new game. It ensures the game ID is incremented correctly.
TEST(GameHistoryTest, StartNewGame) {
    GameHistory game;
    int game_id = game.startNewGame();
    EXPECT_EQ(game_id, 1); 
    game_id = game.startNewGame();
    EXPECT_EQ(game_id, 2); 
}

// Test case for adding a valid move. Verifies the move is saved correctly.
TEST(GameHistoryTest, AddMoveValid) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1); // Add a move for player X at position 1
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 1); // Ensure there is one move
    EXPECT_EQ(moves[0].player, 'X');
    EXPECT_EQ(moves[0].position, 1);
    EXPECT_EQ(moves[0].move_number, 1); // Move number should be 1
}

// Test case for invalid position. Ensures the game throws an error for invalid positions.
TEST(GameHistoryTest, AddMoveInvalidPosition) {
    GameHistory game;
    int game_id = game.startNewGame();
    EXPECT_THROW(game.addMove(game_id, 'X', 10), std::runtime_error); // Position 10 is invalid
}

// Test case for when a position is already taken. Ensures the game prevents it.
TEST(GameHistoryTest, AddMovePositionTaken) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1); // Add a move at position 1
    EXPECT_THROW(game.addMove(game_id, 'O', 1), std::runtime_error); // Position 1 is already taken
}

// Test case for invalid player. Ensures only 'X' and 'O' are valid players.
TEST(GameHistoryTest, AddMoveInvalidPlayer) {
    GameHistory game;
    int game_id = game.startNewGame();
    EXPECT_THROW(game.addMove(game_id, 'Z', 1), std::runtime_error); // 'Z' is not a valid player
}

// Test case for adding a move to a non-existent game. Ensures proper error handling.
TEST(GameHistoryTest, AddMoveNonExistentGame) {
    GameHistory game;
    EXPECT_THROW(game.addMove(999, 'X', 1), std::runtime_error); // Game ID 999 does not exist
}

// Test case for a full board. Ensures the game handles a full board correctly and prevents further moves.
TEST(GameHistoryTest, FullBoard) {
    GameHistory game;
    int game_id = game.startNewGame();
    for (int pos = 1; pos <= 9; ++pos) {
        game.addMove(game_id, (pos % 2 == 1) ? 'X' : 'O', pos); // Alternating moves between 'X' and 'O'
    }
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 9); // There should be 9 moves
    for (int i = 0; i < 9; ++i) {
        EXPECT_EQ(moves[i].position, i + 1); // Ensure the positions match
        EXPECT_EQ(moves[i].player, (i % 2 == 0) ? 'X' : 'O'); // Ensure alternating players
        EXPECT_EQ(moves[i].move_number, i + 1); // Move numbers should be sequential
    }
    EXPECT_THROW(game.addMove(game_id, 'X', 1), std::runtime_error); // Can't add move after the board is full
}

// Test case for player case sensitivity. Ensures that 'x' and 'o' are treated as 'X' and 'O'.
TEST(GameHistoryTest, PlayerCaseSensitivity) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'x', 1); // Lowercase 'x' should be treated as 'X'
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 1);
    EXPECT_EQ(moves[0].player, 'X'); // Player should be 'X'
    game.addMove(game_id, 'o', 2); // Lowercase 'o' should be treated as 'O'
    moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 2);
    EXPECT_EQ(moves[1].player, 'O'); // Player should be 'O'
}

// Test case for negative or zero position. Ensures the game throws an error for such positions.
TEST(GameHistoryTest, NegativeOrZeroPosition) {
    GameHistory game;
    int game_id = game.startNewGame();
    EXPECT_THROW(game.addMove(game_id, 'X', 0), std::runtime_error); // Position 0 is invalid
    EXPECT_THROW(game.addMove(game_id, 'X', -1), std::runtime_error); // Negative position is invalid
}

// Test case for handling multiple games. Verifies that moves are correctly tracked for different games.
TEST(GameHistoryTest, MultipleGames) {
    GameHistory game;
    int game1_id = game.startNewGame();
    int game2_id = game.startNewGame();
    game.addMove(game1_id, 'X', 1); // First move in game 1
    game.addMove(game2_id, 'O', 1); // First move in game 2
    auto moves1 = game.getGameMoves(game1_id);
    auto moves2 = game.getGameMoves(game2_id);
    ASSERT_EQ(moves1.size(), 1); // Game 1 has 1 move
    ASSERT_EQ(moves2.size(), 1); // Game 2 has 1 move
    EXPECT_EQ(moves1[0].player, 'X');
    EXPECT_EQ(moves1[0].position, 1);
    EXPECT_EQ(moves2[0].player, 'O');
    EXPECT_EQ(moves2[0].position, 1); // Both games had a move at position 1
}

// Test case for adding moves in reverse order (checking move_number consistency).
TEST(GameHistoryTest, AddMovesInReverseOrder) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1);
    game.addMove(game_id, 'O', 2);
    game.addMove(game_id, 'X', 3);
    
    // Let's add moves in reverse order
    game.addMove(game_id, 'O', 9);
    game.addMove(game_id, 'X', 8);
    
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 5); // Ensure we have 5 moves
    
    // Check that the move numbers are correctly assigned sequentially
    EXPECT_EQ(moves[0].move_number, 1);
    EXPECT_EQ(moves[1].move_number, 2);
    EXPECT_EQ(moves[2].move_number, 3);
    EXPECT_EQ(moves[3].move_number, 4);
    EXPECT_EQ(moves[4].move_number, 5);
}

// Test case for ensuring move numbers increment properly after a reset (i.e., a new game).
TEST(GameHistoryTest, IncrementMoveNumbersAcrossGames) {
    GameHistory game;
    int game1_id = game.startNewGame();
    game.addMove(game1_id, 'X', 1); // Game 1, Move 1
    game.addMove(game1_id, 'O', 2); // Game 1, Move 2
    
    // Start a new game
    int game2_id = game.startNewGame();
    game.addMove(game2_id, 'X', 3); // Game 2, Move 1
    game.addMove(game2_id, 'O', 4); // Game 2, Move 2
    
    // Assert that move numbering starts from 1 for each game
    auto moves1 = game.getGameMoves(game1_id);
    ASSERT_EQ(moves1.size(), 2);
    EXPECT_EQ(moves1[0].move_number, 1);
    EXPECT_EQ(moves1[1].move_number, 2);
    
    auto moves2 = game.getGameMoves(game2_id);
    ASSERT_EQ(moves2.size(), 2);
    EXPECT_EQ(moves2[0].move_number, 1);
    EXPECT_EQ(moves2[1].move_number, 2);
}

// Test case for ensuring moves are added in a proper sequence with no skipped numbers.
TEST(GameHistoryTest, EnsureNoSkippedMoveNumbers) {
    GameHistory game;
    int game_id = game.startNewGame();
    
    // Add moves in a proper sequence
    game.addMove(game_id, 'X', 1);
    game.addMove(game_id, 'O', 2);
    game.addMove(game_id, 'X', 3);
    
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 3);
    
    // Verify that move numbers are assigned without skipping
    EXPECT_EQ(moves[0].move_number, 1);
    EXPECT_EQ(moves[1].move_number, 2);
    EXPECT_EQ(moves[2].move_number, 3);
}

// Test case for ensuring that no moves are added for a non-existent game (check invalid game ID).
TEST(GameHistoryTest, AddMoveForNonExistentGameID) {
    GameHistory game;
    EXPECT_THROW(game.addMove(9999, 'X', 1), std::runtime_error); // Game ID 9999 does not exist
}

// Test case to ensure that the board state is updated correctly after a move.
TEST(GameHistoryTest, UpdateBoardStateAfterMove) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1); // Add move at position 1
    
    // Validate that the position is marked as occupied (1)
    EXPECT_EQ(game.isValidMove(1), false); // Position 1 should be occupied now
    
    // Add another move
    game.addMove(game_id, 'O', 2); // Add move at position 2
    
    // Validate that the position 2 is now occupied
    EXPECT_EQ(game.isValidMove(2), false); // Position 2 should be occupied now
}

// Test case for trying to add moves after a game has started and is ended (i.e., board is full).
TEST(GameHistoryTest, PreventAddingMovesAfterFullBoard) {
    GameHistory game;
    int game_id = game.startNewGame();
    
    // Fill up the board
    for (int pos = 1; pos <= 9; ++pos) {
        game.addMove(game_id, (pos % 2 == 1) ? 'X' : 'O', pos);
    }
    
    // Try adding a move after the board is full (expecting an error)
    EXPECT_THROW(game.addMove(game_id, 'X', 1), std::runtime_error); // Board is full, can't add more moves
}

// Test case for checking that moves are correctly recorded when switching players.
TEST(GameHistoryTest, SwitchPlayersAfterEachMove) {
    GameHistory game;
    int game_id = game.startNewGame();
    
    // Add moves alternately for 'X' and 'O'
    game.addMove(game_id, 'X', 1); // Player 'X'
    game.addMove(game_id, 'O', 2); // Player 'O'
    game.addMove(game_id, 'X', 3); // Player 'X'
    game.addMove(game_id, 'O', 4); // Player 'O'
    
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 4);
    
    // Verify that the players alternate between moves
    EXPECT_EQ(moves[0].player, 'X');
    EXPECT_EQ(moves[1].player, 'O');
    EXPECT_EQ(moves[2].player, 'X');
    EXPECT_EQ(moves[3].player, 'O');
}

// Test case to check for moves when the game has just been initialized (before any moves).
TEST(GameHistoryTest, NoMovesAtStart) {
    GameHistory game;
    int game_id = game.startNewGame();
    
    // Ensure no moves are recorded initially
    auto moves = game.getGameMoves(game_id);
    EXPECT_TRUE(moves.empty()); // No moves should have been made yet
}

// Test case for creating a large number of games. Ensures game IDs are assigned correctly.
TEST(GameHistoryTest, MaximumGames) {
    GameHistory game;
    const int num_games = 1000; // Create 1000 games
    for (int i = 1; i <= num_games; ++i) {
        int game_id = game.startNewGame();
        EXPECT_EQ(game_id, i); // Verify game ID increments correctly
    }
}

// Test case for retrieving moves from a non-existent game. Ensures an empty vector is returned.
TEST(GameHistoryTest, InvalidGameIDRetrieval) {
    GameHistory game;
    auto moves = game.getGameMoves(9999); // Game ID 9999 does not exist
    EXPECT_TRUE(moves.empty()); // Expect an empty move list
}

// Test case for board state consistency. Verifies isValidMove reflects the board state.
TEST(GameHistoryTest, BoardStateConsistency) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1); // Occupy position 1
    game.addMove(game_id, 'O', 2); // Occupy position 2
    game.addMove(game_id, 'X', 3); // Occupy position 3
    
    // Check occupied positions
    EXPECT_EQ(game.isValidMove(1), false); // Position 1 is taken
    EXPECT_EQ(game.isValidMove(2), false); // Position 2 is taken
    EXPECT_EQ(game.isValidMove(3), false); // Position 3 is taken
    
    // Check unoccupied positions
    EXPECT_EQ(game.isValidMove(4), true); // Position 4 is free
    EXPECT_EQ(game.isValidMove(9), true); // Position 9 is free
}

// Test case for enforcing alternating players. Ensures consecutive moves by the same player are rejected.
TEST(GameHistoryTest, EnforceAlternatingPlayers) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1); // Player X moves
    EXPECT_THROW(game.addMove(game_id, 'X', 2), std::runtime_error); // Player X can't move again
    game.addMove(game_id, 'O', 2); // Player O moves
    EXPECT_THROW(game.addMove(game_id, 'O', 3), std::runtime_error); // Player O can't move again
}

// Test case for move retrieval order. Ensures moves are returned in ascending move_number order.
TEST(GameHistoryTest, MoveRetrievalOrder) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 5); // Move 1
    game.addMove(game_id, 'O', 2); // Move 2
    game.addMove(game_id, 'X', 9); // Move 3
    
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 3);
    
    // Verify moves are sorted by move_number
    EXPECT_EQ(moves[0].position, 5);
    EXPECT_EQ(moves[0].move_number, 1);
    EXPECT_EQ(moves[1].position, 2);
    EXPECT_EQ(moves[1].move_number, 2);
    EXPECT_EQ(moves[2].position, 9);
    EXPECT_EQ(moves[2].move_number, 3);
}

// Test case for game ID behavior after multiple games. Ensures IDs increment without reuse.
TEST(GameHistoryTest, GameIDReuseAfterReset) {
    GameHistory game;
    int game1_id = game.startNewGame(); // Game 1
    game.addMove(game1_id, 'X', 1); // Add a move to game 1
    int game2_id = game.startNewGame(); // Game 2
    int game3_id = game.startNewGame(); // Game 3
    EXPECT_EQ(game1_id, 1); // First game ID
    EXPECT_EQ(game2_id, 2); // Second game ID
    EXPECT_EQ(game3_id, 3); // Third game ID, no reuse
}

// Test case for moves on a partially filled board. Ensures correct tracking and validation.
TEST(GameHistoryTest, PartialBoardMoves) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1); // Move 1
    game.addMove(game_id, 'O', 5); // Move 2
    game.addMove(game_id, 'X', 9); // Move 3
    
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 3); // Expect 3 moves
    EXPECT_EQ(moves[0].position, 1);
    EXPECT_EQ(moves[1].position, 5);
    EXPECT_EQ(moves[2].position, 9);
    
    // Verify board state
    EXPECT_EQ(game.isValidMove(1), false); // Position 1 is taken
    EXPECT_EQ(game.isValidMove(5), false); // Position 5 is taken
    EXPECT_EQ(game.isValidMove(9), false); // Position 9 is taken
    EXPECT_EQ(game.isValidMove(2), true); // Position 2 is free
}

// Test case for invalid move after valid moves. Ensures state integrity after an invalid attempt.
TEST(GameHistoryTest, InvalidMoveAfterValid) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1); // Valid move
    game.addMove(game_id, 'O', 2); // Valid move
    EXPECT_THROW(game.addMove(game_id, 'X', 10), std::runtime_error); // Invalid position
    
    // Verify state remains unchanged
    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 2); // Still 2 moves
    EXPECT_EQ(moves[0].position, 1);
    EXPECT_EQ(moves[1].position, 2);
}

// Test case for move counts across multiple games. Ensures correct move tracking.
TEST(GameHistoryTest, MoveCountAfterMultipleGames) {
    GameHistory game;
    int game1_id = game.startNewGame();
    game.addMove(game1_id, 'X', 1); // Game 1: 1 move
    game.addMove(game1_id, 'O', 2); // Game 1: 2 moves
    
    int game2_id = game.startNewGame();
    game.addMove(game2_id, 'X', 3); // Game 2: 1 move
    
    int game3_id = game.startNewGame(); // Game 3: 0 moves
    
    // Verify move counts
    EXPECT_EQ(game.getGameMoves(game1_id).size(), 2); // Game 1 has 2 moves
    EXPECT_EQ(game.getGameMoves(game2_id).size(), 1); // Game 2 has 1 move
    EXPECT_EQ(game.getGameMoves(game3_id).size(), 0); // Game 3 has 0 moves
}

// Test case for invalid position checks with isValidMove. Ensures consistent validation.
TEST(GameHistoryTest, CaseInsensitivePositionCheck) {
    GameHistory game;
    int game_id = game.startNewGame();
    
    // Check invalid positions
    EXPECT_EQ(game.isValidMove(0), false); // Position 0 is invalid
    EXPECT_EQ(game.isValidMove(-1), false); // Negative position is invalid
    EXPECT_EQ(game.isValidMove(10), false); // Position 10 is invalid
    
    // Add a move and recheck
    game.addMove(game_id, 'X', 1);
    EXPECT_EQ(game.isValidMove(1), false); // Position 1 is now taken
    EXPECT_EQ(game.isValidMove(2), true); // Position 2 is still free
}

// Test case for repeated calls to getGameMoves with no new moves. Ensures stability.
TEST(GameHistoryTest, GetGameMovesRepeatedCalls) {
    GameHistory game;
    int game_id = game.startNewGame();
    auto moves1 = game.getGameMoves(game_id);
    auto moves2 = game.getGameMoves(game_id);
    EXPECT_EQ(moves1.size(), moves2.size()); // No new moves added, sizes should match
    EXPECT_TRUE(moves1.empty()); // Should both be empty
}

// Test case for verifying that each game has an independent board state.
TEST(GameHistoryTest, IndependentBoardStates) {
    GameHistory game;
    int game1_id = game.startNewGame();
    int game2_id = game.startNewGame();
    game.addMove(game1_id, 'X', 1); // Occupy position 1 in game 1

    // Position 1 should still be valid in game 2
    EXPECT_NO_THROW(game.addMove(game2_id, 'O', 1));
}

// Test case to verify that positions outside the range (e.g., >9) are invalid.
TEST(GameHistoryTest, AddMoveOutOfBoundsHigh) {
    GameHistory game;
    int game_id = game.startNewGame();
    EXPECT_THROW(game.addMove(game_id, 'X', 15), std::runtime_error); // Invalid position > 9
}

// Test case for alternating move sequence with same player (should allow if rules permit).
TEST(GameHistoryTest, SamePlayerConsecutiveMovesAllowed) {
    GameHistory game;
    int game_id = game.startNewGame();
    game.addMove(game_id, 'X', 1);
    EXPECT_NO_THROW(game.addMove(game_id, 'X', 2)); // 'X' again
}

// Test case for checking that a new game starts with a clean board.
TEST(GameHistoryTest, NewGameStartsWithEmptyBoard) {
    GameHistory game;
    int game_id = game.startNewGame();
    
    for (int pos = 1; pos <= 9; ++pos) {
        EXPECT_TRUE(game.isValidMove(pos)); // All positions should be available
    }
}

// Test case to ensure that game IDs are independent from moves.
TEST(GameHistoryTest, GameIDIndependentFromMoves) {
    GameHistory game;
    int game1_id = game.startNewGame();
    game.addMove(game1_id, 'X', 1);
    int game2_id = game.startNewGame(); // Should still return game2_id = 2
    EXPECT_EQ(game2_id, 2);
}

// Test case for checking behavior when no games have been started yet.
TEST(GameHistoryTest, NoGameStartedYet) {
    GameHistory game;
    EXPECT_THROW(game.getGameMoves(1), std::runtime_error); // No game has been started
    EXPECT_THROW(game.addMove(1, 'X', 1), std::runtime_error); // Can't add to a non-existent game
}

// Test case to ensure isValidMove returns false for invalid positions.
TEST(GameHistoryTest, InvalidPositionsInBoardCheck) {
    GameHistory game;
    EXPECT_FALSE(game.isValidMove(0));  // Position 0 is invalid
    EXPECT_FALSE(game.isValidMove(10)); // Position 10 is invalid
    EXPECT_FALSE(game.isValidMove(-5)); // Negative position is invalid
}

// Test case to verify all moves in a full game are unique in position.
TEST(GameHistoryTest, UniqueMovePositions) {
    GameHistory game;
    int game_id = game.startNewGame();

    for (int pos = 1; pos <= 9; ++pos) {
        game.addMove(game_id, (pos % 2 == 0) ? 'O' : 'X', pos);
    }

    auto moves = game.getGameMoves(game_id);
    std::set<int> unique_positions;
    for (const auto& move : moves) {
        unique_positions.insert(move.position);
    }

    EXPECT_EQ(unique_positions.size(), 9); // All 9 positions should be unique
}

// Test case to verify that alternating players can make valid moves.
TEST(GameHistoryTest, AlternatingPlayersCanMove) {
    GameHistory game;
    int game_id = game.startNewGame();

    EXPECT_NO_THROW(game.addMove(game_id, 'X', 1));
    EXPECT_NO_THROW(game.addMove(game_id, 'O', 2));
    EXPECT_NO_THROW(game.addMove(game_id, 'X', 3));

    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 3);
    EXPECT_EQ(moves[0].player, 'X');
    EXPECT_EQ(moves[1].player, 'O');
    EXPECT_EQ(moves[2].player, 'X');
}

// Test case to ensure that adding a move to a finished game throws an exception
TEST(GameHistoryTest, AddMoveToFinishedGameThrows) {
    GameHistory game;
    int game_id = game.startNewGame();

    for (int pos = 1; pos <= 9; ++pos) {
        game.addMove(game_id, (pos % 2 == 0 ? 'O' : 'X'), pos);
    }

    // Board is full now; adding another move should fail
    EXPECT_THROW(game.addMove(game_id, 'X', 1), std::runtime_error);
}

// Test case for requesting moves of a non-existent game
TEST(GameHistoryTest, GetMovesOfNonexistentGameThrows) {
    GameHistory game;
    EXPECT_THROW(game.getGameMoves(42), std::runtime_error); // Game 42 doesn't exist
}

// Test that move history is stored in correct order
TEST(GameHistoryTest, MovesStoredInCorrectOrder) {
    GameHistory game;
    int game_id = game.startNewGame();

    game.addMove(game_id, 'X', 1);
    game.addMove(game_id, 'O', 5);
    game.addMove(game_id, 'X', 9);

    auto moves = game.getGameMoves(game_id);
    ASSERT_EQ(moves.size(), 3);
    EXPECT_EQ(moves[0].position, 1);
    EXPECT_EQ(moves[1].position, 5);
    EXPECT_EQ(moves[2].position, 9);
}

// Test that multiple games can run independently
TEST(GameHistoryTest, MultipleGamesRunIndependently) {
    GameHistory game;
    int game1 = game.startNewGame();
    int game2 = game.startNewGame();

    game.addMove(game1, 'X', 1);
    game.addMove(game2, 'O', 5);

    auto moves1 = game.getGameMoves(game1);
    auto moves2 = game.getGameMoves(game2);

    ASSERT_EQ(moves1.size(), 1);
    ASSERT_EQ(moves2.size(), 1);
    EXPECT_EQ(moves1[0].position, 1);
    EXPECT_EQ(moves2[0].position, 5);
    EXPECT_EQ(moves1[0].player, 'X');
    EXPECT_EQ(moves2[0].player, 'O');
}
