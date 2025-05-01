# Tests CMakeLists.txt
include(FetchContent) # Use FetchContent for GTest
FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.16.0 # Or a specific tag/commit you prefer
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE) # Needed on Windows, harmless elsewhere
FetchContent_MakeAvailable(googletest)

find_package(Qt6 COMPONENTS Core Widgets Test REQUIRED)

# Create the test_user_auth executable
add_executable(test_user_auth test_user_auth.cpp ../src/auth/user_auth.cpp ../src/db/database.cpp)
target_link_libraries(test_user_auth
    GTest::gtest       # Link GTest libraries provided by FetchContent
    GTest::gtest_main
    GTest::gmock       # Uncomment if using GMock
    GTest::gmock_main  # Uncomment if using GMock
    pthread            # Keep existing links
    SQLite::SQLite3
    OpenSSL::Crypto
)

# Create the test_database executable
add_executable(test_database test_database.cpp ../src/db/database.cpp)
target_link_libraries(test_database
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    pthread
    SQLite::SQLite3
    OpenSSL::Crypto
)

# Create the test_login_page executable
add_executable(test_login_page
    test_login_page.cpp
    ../src/login_page.cpp
    ../src/background_widget.cpp
    ../src/auth/user_auth.cpp
    ../src/db/database.cpp
)
target_link_libraries(test_login_page
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    pthread
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    SQLite::SQLite3
    OpenSSL::Crypto
)


# Create the test_main_window executable
add_executable(test_main_window
    test_main_window.cpp
    ../src/main_window.cpp
    ../src/login_page.cpp
    ../src/background_widget.cpp
    ../src/auth/user_auth.cpp # Add auth sources needed by main_window
    ../src/db/database.cpp    # Add db sources needed by main_window
)
# Explicitly add include directories needed by main_window.cpp and its headers
target_include_directories(test_main_window PRIVATE
    ../../Game/GUI/src
    ../../Game/Board/src
    ../../Game/AI/src
    ../../Game/Globals
)
target_link_libraries(test_main_window
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    pthread
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
    SQLite::SQLite3
    OpenSSL::Crypto
    # Link the game libraries instead of adding sources
    gui
    board
    ai
    globals
)

# Add the tests using GoogleTest discovery
include(GoogleTest)
gtest_discover_tests(test_user_auth)
gtest_discover_tests(test_database)
gtest_discover_tests(test_login_page)
gtest_discover_tests(test_main_window)

# Remove old add_test commands
# add_test(NAME UserAuthTest COMMAND test_user_auth)
# add_test(NAME DatabaseTest COMMAND test_database)
# add_test(NAME LoginPageTest COMMAND test_login_page)
# add_test(NAME MainWindowTest COMMAND test_main_window)
