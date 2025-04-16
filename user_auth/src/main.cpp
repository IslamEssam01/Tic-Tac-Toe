#include "auth/user_auth.h"
#include <iostream>
#include <string>
#include <limits>

// Clear input buffer to prevent issues with getline
void clearInputBuffer() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

// Prompt for and read a password
std::string getPassword() {
    std::string password;
    std::cout << "Enter password: ";
    std::getline(std::cin, password);
    return password;
}

int main() {
    // Initialize UserAuth with SQLite database
    UserAuth auth("users.db");
    std::string username, password;
    int choice;

    // Main loop for user interaction
    while (true) {
        // Display menu
        std::cout << "\n=== User Authentication System ===\n";
        std::cout << "1. Register\n";
        std::cout << "2. Login\n";
        std::cout << "3. Exit\n";
        std::cout << "Enter choice (1-3): ";

        // Read and validate menu choice
        if (!(std::cin >> choice)) {
            std::cout << "Invalid input. Please enter a number.\n";
            clearInputBuffer();
            continue;
        }

        // Clear buffer for getline
        clearInputBuffer();

        // Handle exit
        if (choice == 3) {
            std::cout << "Exiting...\n";
            break;
        }

        // Validate choice
        if (choice != 1 && choice != 2) {
            std::cout << "Invalid choice. Please select 1, 2, or 3.\n";
            continue;
        }

        // Read username
        std::cout << "Enter username: ";
        std::getline(std::cin, username);

        // Handle menu options
        switch (choice) {
            case 1: // Register
                password = getPassword();
                if (auth.registerUser(username, password)) {
                    std::cout << "User " << username << " registered successfully.\n";
                } else {
                    std::cout << "Registration failed. Username may already exist.\n";
                }
                break;

            case 2: // Login
                password = getPassword();
                if (auth.login(username, password)) {
                    std::cout << "Login successful for " << username << ".\n";
                } else {
                    std::cout << "Login failed. Invalid username or password.\n";
                }
                break;
        }
    }

    return 0;
}