#ifndef USER_AUTH_H
#define USER_AUTH_H

#include "../db/database.h"
#include <string>

class UserAuth {
public:
    UserAuth(const std::string& dbPath);
    bool registerUser(const std::string& username, const std::string& password);
    bool login(const std::string& username, const std::string& password);

private:
    Database db;
    std::string hashPassword(const std::string& password);
    bool verifyPassword(const std::string& password, const std::string& storedHash);
    bool isValidPassword(const std::string& password);
};

#endif
