cmake_minimum_required(VERSION 3.14)
project(TicTacToeApp)

# Include common settings
include(${CMAKE_SOURCE_DIR}/cmake/CommonSettings.cmake)

# Add component subdirectories
add_subdirectory(${CMAKE_SOURCE_DIR}/components/auth ${CMAKE_BINARY_DIR}/auth_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/components/game ${CMAKE_BINARY_DIR}/game_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/components/game_history ${CMAKE_BINARY_DIR}/game_history_build)
add_subdirectory(${CMAKE_SOURCE_DIR}/components/gui ${CMAKE_BINARY_DIR}/gui_build)

# Create the main application executable
if(WIN32)
    # Generate a minimal resource file for Windows
    set(RC_CONTENT "#include <windows.h>\n")
    set(RC_CONTENT "${RC_CONTENT}// Application version info\n")
    set(RC_CONTENT "${RC_CONTENT}VS_VERSION_INFO VERSIONINFO\n")
    set(RC_CONTENT "${RC_CONTENT}FILEVERSION 1,0,0,0\n")
    set(RC_CONTENT "${RC_CONTENT}PRODUCTVERSION 1,0,0,0\n")
    set(RC_CONTENT "${RC_CONTENT}FILEFLAGSMASK VS_FFI_FILEFLAGSMASK\n")
    set(RC_CONTENT "${RC_CONTENT}FILEFLAGS 0x0L\n")
    set(RC_CONTENT "${RC_CONTENT}FILEOS VOS_NT_WINDOWS32\n")
    set(RC_CONTENT "${RC_CONTENT}FILETYPE VFT_APP\n")
    set(RC_CONTENT "${RC_CONTENT}FILESUBTYPE VFT2_UNKNOWN\n")
    set(RC_CONTENT "${RC_CONTENT}BEGIN\n")
    set(RC_CONTENT "${RC_CONTENT}    BLOCK \"StringFileInfo\"\n")
    set(RC_CONTENT "${RC_CONTENT}    BEGIN\n")
    set(RC_CONTENT "${RC_CONTENT}        BLOCK \"040904b0\"\n")
    set(RC_CONTENT "${RC_CONTENT}        BEGIN\n")
    set(RC_CONTENT "${RC_CONTENT}            VALUE \"FileDescription\", \"Tic-Tac-Toe Game\"\n")
    set(RC_CONTENT "${RC_CONTENT}            VALUE \"ProductName\", \"Tic-Tac-Toe\"\n")
    set(RC_CONTENT "${RC_CONTENT}            VALUE \"FileVersion\", \"1.0.0.0\"\n")
    set(RC_CONTENT "${RC_CONTENT}            VALUE \"ProductVersion\", \"1.0.0.0\"\n")
    set(RC_CONTENT "${RC_CONTENT}        END\n")
    set(RC_CONTENT "${RC_CONTENT}    END\n")
    set(RC_CONTENT "${RC_CONTENT}    BLOCK \"VarFileInfo\"\n")
    set(RC_CONTENT "${RC_CONTENT}    BEGIN\n")
    set(RC_CONTENT "${RC_CONTENT}        VALUE \"Translation\", 0x409, 1200\n")
    set(RC_CONTENT "${RC_CONTENT}    END\n")
    set(RC_CONTENT "${RC_CONTENT}END\n")

    # Write the resource file
    file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/app.rc" "${RC_CONTENT}")

    # On Windows, include resource file and set as GUI application
    add_executable(tictactoe_app WIN32
        src/main.cpp
        "${CMAKE_CURRENT_BINARY_DIR}/app.rc"
    )
else()
    add_executable(tictactoe_app
        src/main.cpp
    )
endif()

# Set additional Windows properties
if(WIN32)
    set_target_properties(tictactoe_app PROPERTIES
        WIN32_EXECUTABLE TRUE
        OUTPUT_NAME "tictactoe_app"
    )
endif()

target_include_directories(tictactoe_app PRIVATE
    ${CMAKE_SOURCE_DIR}/components/gui/src
    ${CMAKE_SOURCE_DIR}/components/game_history/src
)

target_link_libraries(tictactoe_app
    PRIVATE
        gui
        game_history
)
