# Tests CMakeLists.txt
find_package(GTest REQUIRED)
find_package(Qt6 COMPONENTS Core Widgets Test REQUIRED)

# Include Google Test and Google Mock directories
include_directories(${GTEST_INCLUDE_DIRS})

# Create the test_user_auth executable
add_executable(test_user_auth test_user_auth.cpp ../src/auth/user_auth.cpp ../src/db/database.cpp)
target_link_libraries(test_user_auth
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    pthread
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

# Create the test_welcome_page executable
add_executable(test_welcome_page
    test_welcome_page.cpp
    ../src/welcome_page.cpp
    ../src/background_widget.cpp 
)
target_link_libraries(test_welcome_page
    GTest::gtest
    GTest::gtest_main
    GTest::gmock
    GTest::gmock_main
    pthread
    Qt6::Core
    Qt6::Widgets
    Qt6::Test
)

# Create the test_main_window executable
add_executable(test_main_window
    test_main_window.cpp
    ../src/main_window.cpp
    ../src/login_page.cpp
    ../src/welcome_page.cpp
    ../src/background_widget.cpp 
    ../src/auth/user_auth.cpp
    ../src/db/database.cpp
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
)

# Add the tests
add_test(NAME UserAuthTest COMMAND test_user_auth)
add_test(NAME DatabaseTest COMMAND test_database)
add_test(NAME LoginPageTest COMMAND test_login_page)
add_test(NAME WelcomePageTest COMMAND test_welcome_page)
add_test(NAME MainWindowTest COMMAND test_main_window)