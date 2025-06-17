#include "performance_monitor.h"
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>
#include <iomanip>
#include <QCoreApplication>
#include <QObject>
#include <QString>

// Include game components
#include "Board.h"
#include "AI.h"
#include "auth/user_auth.h"
#include "game_history.h"
#include "application.h"
#include "main_window.h"

// Custom optimized auth class for performance testing
class OptimizedUserAuth {
public:
    OptimizedUserAuth(const std::string& dbPath) {
        sqlite3_open(dbPath.c_str(), &db);
        executeQuery("CREATE TABLE IF NOT EXISTS users (username TEXT PRIMARY KEY, password TEXT NOT NULL);");
        executeQuery("BEGIN TRANSACTION;"); // Start transaction for batch operations
    }
    
    ~OptimizedUserAuth() {
        if (db) {
            executeQuery("COMMIT;"); // Commit all operations
            sqlite3_close(db);
        }
    }
    
    bool batchRegisterUsers(int count) {
        for (int i = 0; i < count; ++i) {
            std::string username = "user" + std::to_string(i);
            std::string password = "pass" + std::to_string(i) + "123";
            std::string query = "INSERT OR IGNORE INTO users (username, password) VALUES ('" + username + "', '" + password + "');";
            if (!executeQuery(query)) return false;
        }
        return true;
    }
    
private:
    sqlite3* db = nullptr;
    
    bool executeQuery(const std::string& query) {
        char* errMsg = nullptr;
        int result = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
        if (errMsg) sqlite3_free(errMsg);
        return result == SQLITE_OK;
    }
};

class PerformanceTestSuite {
public:
    PerformanceTestSuite() : monitor_() {}
    
    void runAllTests() {
        std::cout << "=== Tic-Tac-Toe Core Performance Test Suite ===" << std::endl;
        std::cout << "Starting core performance tests..." << std::endl << std::endl;
        
        // Core game logic tests
        testBoardOperations();
        testAIPerformance();
        
        // Database operations tests
        testAuthenticationPerformance();
        testGameHistoryPerformance();
        
        // Memory stress tests
        testMemoryUsage();
        
        // Qt Core performance tests (headless)
        testGUIComponentCreation();
        
        // Realistic user scenarios
        testUserScenarios();
        
        // Concurrency and threading tests
        testConcurrencyPerformance();
        
        // Edge cases and stress testing
        testEdgeCasesAndStress();
        
        // Generate and display results
        generateReport();
    }

private:
    PerformanceMonitor monitor_;
    
    void testBoardOperations() {
        std::cout << "Testing Board Operations..." << std::endl;
        
        // Test 1: Board creation and initialization
        {
            PERFORMANCE_TEST(monitor_, "Board_Creation");
            for (int i = 0; i < 10000; ++i) {
                Board board;
                board.reset();
            }
        }
        
        // Test 2: Move validation
        {
            PERFORMANCE_TEST(monitor_, "Board_MoveValidation");
            Board board;
            for (int i = 0; i < 100000; ++i) {
                board.isValidMove(i % 3, (i / 3) % 3);
            }
        }
        
        // Test 3: Game completion check
        {
            PERFORMANCE_TEST(monitor_, "Board_GameOverCheck");
            Board board;
            // Set up a winning scenario
            board.makeMove(0, 0, Player::X);
            board.makeMove(0, 1, Player::X);
            board.makeMove(0, 2, Player::X);
            
            for (int i = 0; i < 50000; ++i) {
                board.checkWinner();
                board.isGameOver();
            }
        }
        
        // Test 4: Full game simulation
        {
            PERFORMANCE_TEST(monitor_, "Board_FullGameSimulation");
            for (int game = 0; game < 1000; ++game) {
                Board board;
                Player currentPlayer = Player::X;
                
                // Play a complete game
                std::vector<std::pair<int, int>> moves = {
                    {0, 0}, {1, 1}, {0, 1}, {2, 2}, {0, 2} // X wins
                };
                
                for (const auto& move : moves) {
                    if (board.makeMove(move.first, move.second, currentPlayer)) {
                        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
                        if (board.isGameOver()) break;
                    }
                }
            }
        }
    }
    
    void testAIPerformance() {
        std::cout << "Testing AI Performance..." << std::endl;
        
        // Test 1: AI move calculation on empty board
        {
            PERFORMANCE_TEST(monitor_, "AI_EmptyBoardMove");
            for (int i = 0; i < 100; ++i) {
                Board board;
                auto move = findBestMove(board, Player::X);
                (void)move; // Suppress unused variable warning
            }
        }
        
        // Test 2: AI move calculation on partially filled board
        {
            PERFORMANCE_TEST(monitor_, "AI_PartialBoardMove");
            for (int i = 0; i < 100; ++i) {
                Board board;
                board.makeMove(1, 1, Player::X);
                board.makeMove(0, 0, Player::O);
                auto move = findBestMove(board, Player::X);
                (void)move;
            }
        }
        
        // Test 3: AI move calculation in endgame scenarios
        {
            PERFORMANCE_TEST(monitor_, "AI_EndgameMove");
            for (int i = 0; i < 100; ++i) {
                Board board;
                // Set up near-endgame scenario
                board.makeMove(0, 0, Player::X);
                board.makeMove(1, 1, Player::O);
                board.makeMove(0, 1, Player::X);
                board.makeMove(2, 2, Player::O);
                board.makeMove(1, 0, Player::X);
                board.makeMove(0, 2, Player::O);
                
                auto move = findBestMove(board, Player::X);
                (void)move;
            }
        }
        
        // Test 4: Multiple AI vs AI games
        {
            PERFORMANCE_TEST(monitor_, "AI_FullGameAIvsAI");
            for (int game = 0; game < 50; ++game) {
                Board board;
                Player currentPlayer = Player::X;
                
                while (!board.isGameOver()) {
                    auto move = findBestMove(board, currentPlayer);
                    board.makeMove(move.first, move.second, currentPlayer);
                    currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
                }
            }
        }
    }
    
    void testAuthenticationPerformance() {
        std::cout << "Testing Authentication Performance..." << std::endl;
        
        // Create temporary database
        std::string dbPath = "perf_test_auth.db";
        
        // Test 1: User registration performance (optimized batch operations)
        {
            PERFORMANCE_TEST(monitor_, "Auth_UserRegistration_Batch");
            try {
                OptimizedUserAuth auth(dbPath);
                auth.batchRegisterUsers(100);
            } catch (const std::exception& e) {
                std::cerr << "Auth registration test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 1b: Individual user registration (for comparison)
        {
            PERFORMANCE_TEST(monitor_, "Auth_UserRegistration_Individual");
            try {
                UserAuth auth(dbPath + "_individual");
                for (int i = 0; i < 10; ++i) {  // Only 10 for individual operations
                    std::string username = "user" + std::to_string(i);
                    std::string password = "pass" + std::to_string(i) + "123";
                    auth.registerUser(username, password);
                }
            } catch (const std::exception& e) {
                std::cerr << "Auth individual registration test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 2: User login performance
        {
            PERFORMANCE_TEST(monitor_, "Auth_UserLogin");
            try {
                UserAuth auth(dbPath);
                for (int i = 0; i < 50; ++i) {  // Reduced for realistic testing
                    std::string username = "user" + std::to_string(i % 10); // Reuse some users
                    std::string password = "pass" + std::to_string(i % 10) + "123";
                    auth.login(username, password);
                }
            } catch (const std::exception& e) {
                std::cerr << "Auth login test failed: " << e.what() << std::endl;
            }
        }
        
        // Cleanup
        std::remove(dbPath.c_str());
    }
    
    void testGameHistoryPerformance() {
        std::cout << "Testing Game History Performance..." << std::endl;
        
        std::string dbPath = "perf_test_history.db";
        
        // Test 1: Game initialization performance (realistic batch)
        {
            PERFORMANCE_TEST(monitor_, "History_GameInitialization");
            try {
                GameHistory history(dbPath);
                for (int i = 0; i < 20; ++i) {  // Realistic number of games
                    history.initializeGame(1000 + i, 2000 + i);
                }
            } catch (const std::exception& e) {
                std::cerr << "History initialization test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 2: Move recording performance (realistic game moves)
        {
            PERFORMANCE_TEST(monitor_, "History_MoveRecording");
            try {
                GameHistory history(dbPath);
                
                // Simulate 20 complete games with 5-9 moves each
                for (int game = 0; game < 20; ++game) {
                    int currentGameId = history.initializeGame(1001 + game, 1002 + game);
                    for (int move = 0; move < 7; ++move) {  // Average game length
                        history.recordMove(currentGameId, move);
                    }
                }
            } catch (const std::exception& e) {
                std::cerr << "History move recording test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 3: Game retrieval performance
        {
            PERFORMANCE_TEST(monitor_, "History_GameRetrieval");
            try {
                GameHistory history(dbPath);
                
                for (int i = 0; i < 25; ++i) {  // Realistic retrieval frequency
                    auto games = history.getPlayerGames(1000 + (i % 5));
                    (void)games;
                }
            } catch (const std::exception& e) {
                std::cerr << "History retrieval test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 4: Complete game workflow (realistic game simulation)
        {
            PERFORMANCE_TEST(monitor_, "History_CompleteGameWorkflow");
            try {
                GameHistory history(dbPath);
                
                for (int i = 0; i < 10; ++i) {  // 10 complete games
                    int gameId = history.initializeGame(3000 + i, 4000 + i);
                    
                    // Record a realistic game (5 moves)
                    std::vector<int> moves = {4, 0, 1, 8, 7};  // Typical game pattern
                    for (int move : moves) {
                        history.recordMove(gameId, move);
                    }
                    
                    // Set winner
                    history.setWinner(gameId, 3000 + i);
                }
            } catch (const std::exception& e) {
                std::cerr << "History workflow test failed: " << e.what() << std::endl;
            }
        }
        
        // Cleanup
        std::remove(dbPath.c_str());
    }
    
    void testMemoryUsage() {
        std::cout << "Testing Memory Usage..." << std::endl;
        
        // Test 1: Memory usage with multiple board instances
        {
            PERFORMANCE_TEST(monitor_, "Memory_MultipleBoardInstances");
            std::vector<std::unique_ptr<Board>> boards;
            
            for (int i = 0; i < 1000; ++i) {
                boards.push_back(std::make_unique<Board>());
                
                // Use the board a bit
                boards.back()->makeMove(i % 3, (i / 3) % 3, (i % 2 == 0) ? Player::X : Player::O);
            }
            
            // Keep boards alive for a moment to measure peak memory
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        // Test 2: Memory usage with game history (reduced load)
        {
            PERFORMANCE_TEST(monitor_, "Memory_GameHistoryInstances");
            std::string dbPath = "perf_test_memory.db";
            
            try {
                auto history = std::make_unique<GameHistory>(dbPath);
                
                // Create moderate number of games
                for (int i = 0; i < 25; ++i) {  // Realistic memory test
                    int gameId = history->initializeGame(i, i + 1000);
                    
                    // Add moves to each game
                    for (int move = 0; move < 5; ++move) {
                        history->recordMove(gameId, move);
                    }
                    
                    history->setWinner(gameId, i);
                }
                
                // Retrieve games to test memory with loaded data
                for (int i = 0; i < 10; ++i) {  // Realistic retrieval test
                    auto games = history->getPlayerGames(i);
                    (void)games;
                }
                
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } catch (const std::exception& e) {
                std::cerr << "Memory test failed: " << e.what() << std::endl;
            }
            
            std::remove(dbPath.c_str());
        }
    }
    
    void testGUIComponentCreation() {
        std::cout << "Testing GUI Component Creation..." << std::endl;
        
        // Test 1: Basic Qt object creation (headless)
        {
            PERFORMANCE_TEST(monitor_, "GUI_BasicQtObjects");
            std::vector<std::unique_ptr<QObject>> objects;
            
            for (int i = 0; i < 1000; ++i) {
                objects.push_back(std::make_unique<QObject>());
                objects.back()->setObjectName("TestObject" + QString::number(i));
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        // Test 2: QString operations
        {
            PERFORMANCE_TEST(monitor_, "GUI_QStringOperations");
            std::vector<QString> strings;
            
            for (int i = 0; i < 10000; ++i) {
                QString str = QString("Performance Test String %1").arg(i);
                strings.push_back(str.toUpper());
            }
        }
    }
    
    void testUserScenarios() {
        std::cout << "Testing Realistic User Scenarios..." << std::endl;
        
        // Test 1: Complete game session (login → play → save → logout)
        {
            PERFORMANCE_TEST(monitor_, "Scenario_CompleteGameSession");
            try {
                std::string authDbPath = "scenario_auth.db";
                std::string historyDbPath = "scenario_history.db";
                
                // Login phase
                UserAuth auth(authDbPath);
                auth.registerUser("sessionuser", "pass123");
                auth.login("sessionuser", "pass123");
                
                // Game play phase
                Board board;
                Player currentPlayer = Player::X;
                std::vector<std::pair<int, int>> moves = {{1, 1}, {0, 0}, {0, 1}, {2, 2}, {2, 1}};
                
                for (const auto& move : moves) {
                    board.makeMove(move.first, move.second, currentPlayer);
                    if (board.isGameOver()) break;
                    currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
                }
                
                // Save game phase
                GameHistory history(historyDbPath);
                int gameId = history.initializeGame(1001, std::nullopt);
                for (const auto& move : moves) {
                    history.recordMove(gameId, move.first * 3 + move.second);
                }
                history.setWinner(gameId, 1001);
                
                // Cleanup
                std::remove(authDbPath.c_str());
                std::remove(historyDbPath.c_str());
                
            } catch (const std::exception& e) {
                std::cerr << "Complete session test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 2: Multiple consecutive games
        {
            PERFORMANCE_TEST(monitor_, "Scenario_MultipleGames");
            try {
                std::string historyDbPath = "multi_games.db";
                GameHistory history(historyDbPath);
                
                // Simulate a user playing 10 games in succession
                for (int gameNum = 0; gameNum < 10; ++gameNum) {
                    Board board;
                    Player currentPlayer = Player::X;
                    int gameId = history.initializeGame(2001, std::nullopt);
                    
                    // Quick game simulation
                    std::vector<int> quickMoves = {4, 0, 1, 8, 7}; // 5-move game
                    for (size_t moveIdx = 0; moveIdx < quickMoves.size(); ++moveIdx) {
                        int pos = quickMoves[moveIdx];
                        board.makeMove(pos / 3, pos % 3, currentPlayer);
                        history.recordMove(gameId, pos);
                        
                        if (board.isGameOver()) {
                            history.setWinner(gameId, (currentPlayer == Player::X) ? 2001 : -2);
                            break;
                        }
                        currentPlayer = (currentPlayer == Player::X) ? Player::O : Player::X;
                    }
                }
                
                std::remove(historyDbPath.c_str());
                
            } catch (const std::exception& e) {
                std::cerr << "Multiple games test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 3: Game history browsing performance
        {
            PERFORMANCE_TEST(monitor_, "Scenario_HistoryBrowsing");
            try {
                std::string historyDbPath = "browse_history.db";
                GameHistory history(historyDbPath);
                
                // Create some game history first
                for (int i = 0; i < 25; ++i) {
                    int gameId = history.initializeGame(3001, 3002);
                    for (int move = 0; move < 5; ++move) {
                        history.recordMove(gameId, move);
                    }
                    history.setWinner(gameId, 3001);
                }
                
                // Simulate user browsing through history
                for (int browsePage = 0; browsePage < 10; ++browsePage) {
                    auto games = history.getPlayerGames(3001);
                    auto latestGames = history.getLatestGames(10);
                    // Simulate clicking on games to view details
                    for (const auto& game : latestGames) {
                        auto gameDetails = history.getGameById(game.id);
                        (void)gameDetails; // Prevent unused warning
                    }
                }
                
                std::remove(historyDbPath.c_str());
                
            } catch (const std::exception& e) {
                std::cerr << "History browsing test failed: " << e.what() << std::endl;
            }
        }
    }
    
    void testConcurrencyPerformance() {
        std::cout << "Testing Concurrency Performance..." << std::endl;
        
        // Test 1: Simulated concurrent database access
        {
            PERFORMANCE_TEST(monitor_, "Concurrency_DatabaseAccess");
            try {
                std::string dbPath = "concurrent_test.db";
                
                // Simulate multiple "users" accessing database
                std::vector<std::thread> threads;
                std::atomic<int> successCount{0};
                
                for (int i = 0; i < 5; ++i) {
                    threads.emplace_back([&, i]() {
                        try {
                            UserAuth auth(dbPath);
                            std::string username = "user" + std::to_string(i);
                            std::string password = "pass" + std::to_string(i) + "123";
                            
                            if (auth.registerUser(username, password)) {
                                if (auth.login(username, password)) {
                                    successCount++;
                                }
                            }
                        } catch (...) {
                            // Handle thread exceptions silently
                        }
                    });
                }
                
                for (auto& thread : threads) {
                    thread.join();
                }
                
                std::remove(dbPath.c_str());
                
            } catch (const std::exception& e) {
                std::cerr << "Concurrent database test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 2: AI calculation performance under simulated load
        {
            PERFORMANCE_TEST(monitor_, "Concurrency_AIUnderLoad");
            try {
                std::vector<std::thread> threads;
                std::atomic<int> aiMovesCompleted{0};
                
                for (int i = 0; i < 3; ++i) {
                    threads.emplace_back([&]() {
                        Board board;
                        board.makeMove(1, 1, Player::X);
                        board.makeMove(0, 0, Player::O);
                        
                        for (int j = 0; j < 10; ++j) {
                            auto move = findBestMove(board, Player::X);
                            if (move.first >= 0 && move.second >= 0) {
                                aiMovesCompleted++;
                            }
                        }
                    });
                }
                
                for (auto& thread : threads) {
                    thread.join();
                }
                
            } catch (const std::exception& e) {
                std::cerr << "AI under load test failed: " << e.what() << std::endl;
            }
        }
    }
    
    void testEdgeCasesAndStress() {
        std::cout << "Testing Edge Cases and Stress Scenarios..." << std::endl;
        
        // Test 1: Large game history performance
        {
            PERFORMANCE_TEST(monitor_, "Stress_LargeGameHistory");
            try {
                std::string dbPath = "large_history.db";
                GameHistory history(dbPath);
                
                // Create 100 games with full move history
                for (int gameNum = 0; gameNum < 100; ++gameNum) {
                    int gameId = history.initializeGame(4001 + gameNum, 5001 + gameNum);
                    
                    // Full 9-move game
                    std::vector<int> fullGame = {4, 0, 1, 8, 7, 2, 5, 3, 6};
                    for (size_t moveIdx = 0; moveIdx < 7; ++moveIdx) { // Don't fill completely
                        history.recordMove(gameId, fullGame[moveIdx]);
                    }
                    history.setWinner(gameId, (gameNum % 2 == 0) ? 4001 + gameNum : -1);
                }
                
                // Test retrieval performance with large dataset
                auto allGames = history.getAllGames();
                auto playerGames = history.getPlayerGames(4001);
                auto latestGames = history.getLatestGames(50);
                
                (void)allGames; (void)playerGames; (void)latestGames;
                
                std::remove(dbPath.c_str());
                
            } catch (const std::exception& e) {
                std::cerr << "Large history test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 2: Memory usage over extended session
        {
            PERFORMANCE_TEST(monitor_, "Stress_ExtendedSession");
            try {
                std::vector<std::unique_ptr<Board>> boards;
                std::vector<std::unique_ptr<GameHistory>> histories;
                
                // Simulate long gaming session with memory buildup
                for (int session = 0; session < 50; ++session) {
                    boards.push_back(std::make_unique<Board>());
                    
                    std::string dbPath = "session_" + std::to_string(session) + ".db";
                    histories.push_back(std::make_unique<GameHistory>(dbPath));
                    
                    // Play a quick game
                    boards.back()->makeMove(1, 1, Player::X);
                    boards.back()->makeMove(0, 0, Player::O);
                    boards.back()->makeMove(0, 1, Player::X);
                    
                    // Record in history
                    int gameId = histories.back()->initializeGame(session, session + 1000);
                    histories.back()->recordMove(gameId, 4);
                    histories.back()->recordMove(gameId, 0);
                    histories.back()->recordMove(gameId, 1);
                    histories.back()->setWinner(gameId, session);
                }
                
                // Cleanup
                for (size_t i = 0; i < histories.size(); ++i) {
                    std::string dbPath = "session_" + std::to_string(i) + ".db";
                    std::remove(dbPath.c_str());
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Extended session test failed: " << e.what() << std::endl;
            }
        }
        
        // Test 3: Rapid game creation and destruction
        {
            PERFORMANCE_TEST(monitor_, "Stress_RapidGameOperations");
            try {
                for (int cycle = 0; cycle < 100; ++cycle) {
                    // Create board
                    auto board = std::make_unique<Board>();
                    
                    // Quick operations
                    board->makeMove(1, 1, Player::X);
                    board->isGameOver();
                    board->checkWinner();
                    board->reset();
                    
                    // Destroy board (automatic with unique_ptr)
                    board.reset();
                }
                
            } catch (const std::exception& e) {
                std::cerr << "Rapid operations test failed: " << e.what() << std::endl;
            }
        }
    }
    
    void generateReport() {
        std::cout << std::endl << "=== Performance Test Results ===" << std::endl;
        
        auto summary = monitor_.generateSummary();
        
        std::cout << "Total Tests: " << summary.totalTests << std::endl;
        std::cout << "Successful: " << summary.successfulTests << std::endl;
        std::cout << "Failed: " << summary.failedTests << std::endl;
        std::cout << std::endl;
        
        std::cout << "Response Time Statistics:" << std::endl;
        std::cout << "  Average: " << std::fixed << std::setprecision(2) 
                  << summary.avgResponseTime << " ms" << std::endl;
        std::cout << "  Min: " << summary.minResponseTime << " ms" << std::endl;
        std::cout << "  Max: " << summary.maxResponseTime << " ms" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Memory Usage Statistics:" << std::endl;
        std::cout << "  Average: " << std::fixed << std::setprecision(2) 
                  << summary.avgMemoryUsage << " MB" << std::endl;
        std::cout << "  Peak: " << summary.maxMemoryUsage << " MB" << std::endl;
        std::cout << std::endl;
        
        std::cout << "CPU Usage Statistics:" << std::endl;
        std::cout << "  Average: " << std::fixed << std::setprecision(2) 
                  << summary.avgCPUUsage << " %" << std::endl;
        std::cout << "  Peak: " << summary.maxCPUUsage << " %" << std::endl;
        std::cout << std::endl;
        
        // Export detailed results
        monitor_.exportToCSV("performance_results.csv");
        monitor_.exportToJSON("performance_results.json");
        
        std::cout << "Detailed results exported to:" << std::endl;
        std::cout << "  - performance_results.csv" << std::endl;
        std::cout << "  - performance_results.json" << std::endl;
        std::cout << std::endl;
        
        // Print individual test results
        std::cout << "Individual Test Results:" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        std::cout << std::left << std::setw(30) << "Test Name" 
                  << std::setw(15) << "Time (ms)" 
                  << std::setw(15) << "Memory (MB)" 
                  << std::setw(15) << "CPU (%)" 
                  << "Status" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        for (const auto& result : monitor_.getResults()) {
            std::cout << std::left << std::setw(30) << result.testName
                      << std::fixed << std::setprecision(2)
                      << std::setw(15) << result.responseTimeMs
                      << std::setw(15) << result.memoryUsageMB
                      << std::setw(15) << result.cpuUsagePercent
                      << (result.success ? "PASS" : "FAIL");
            
            if (!result.success && !result.errorMessage.empty()) {
                std::cout << " (" << result.errorMessage << ")";
            }
            
            std::cout << std::endl;
        }
    }
};

int main(int argc, char *argv[]) {
    // Initialize QCoreApplication for Qt components (headless)
    QCoreApplication app(argc, argv);
    
    try {
        PerformanceTestSuite testSuite;
        testSuite.runAllTests();
        
        std::cout << std::endl << "Performance testing completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Performance testing failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Performance testing failed with unknown exception!" << std::endl;
        return 1;
    }
}