#include "database.h"
#include <iostream>

Database::Database(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
        db = nullptr;
    }
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

void Database::init() {
    if (!db) return;
    const char* createTableQuery = 
        "CREATE TABLE IF NOT EXISTS users ("
        "username TEXT PRIMARY KEY, "
        "password TEXT NOT NULL);";
    if (!executeQuery(createTableQuery)) {
        std::cerr << "Failed to create table.\n";
    }
}

bool Database::addUser(const std::string& username, const std::string& hashedPassword) {
    if (username.empty() || hashedPassword.empty()) return false;
    if (!db) return false;
    std::string query = "INSERT INTO users (username, password) VALUES ('" + username + "', '" + hashedPassword + "');";
    return executeQuery(query);
}

bool Database::userExists(const std::string& username) {
    if (username.empty()) return false;
    if (!db) return false;
    std::string query = "SELECT username FROM users WHERE username = '" + username + "';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    bool exists = sqlite3_step(stmt) == SQLITE_ROW;
    sqlite3_finalize(stmt);
    return exists;
}

bool Database::getUserPassword(const std::string& username, std::string& hashedPassword) {
    if (username.empty()) return false;
    if (!db) return false;
    std::string query = "SELECT password FROM users WHERE username = '" + username + "';";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        return false;
    }
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        hashedPassword = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool Database::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}
