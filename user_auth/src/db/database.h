#ifndef DATABASE_H
#define DATABASE_H

#include <sqlite3.h>
#include <string>

class Database {
public:
    Database(const std::string& dbPath);
    ~Database();
    void init();
    bool addUser(const std::string& username, const std::string& hashedPassword);
    bool userExists(const std::string& username);
    bool getUserPassword(const std::string& username, std::string& hashedPassword);

private:
    sqlite3* db;
    bool executeQuery(const std::string& query);
};

#endif