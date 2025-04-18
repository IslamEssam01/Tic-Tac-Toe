#include "user_auth.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>

UserAuth::UserAuth(const std::string& dbPath) : db(dbPath) {
    db.init();
}

bool UserAuth::registerUser(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) return false;
    if (db.userExists(username)) {
        return false;
    }
    if (!isValidPassword(password)) return false;
    std::string hashedPassword = hashPassword(password);
    return db.addUser(username, hashedPassword);
}

bool UserAuth::login(const std::string& username, const std::string& password) {
    if (username.empty() || password.empty()) return false;
    std::string storedHash;
    if (!db.getUserPassword(username, storedHash)) {
        return false;
    }

    return verifyPassword(password, storedHash);
}

std::string UserAuth::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, password.c_str(), password.length());
    SHA256_Final(hash, &sha256);

    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

bool UserAuth::verifyPassword(const std::string& password, const std::string& storedHash) {
    std::string hashedPassword = hashPassword(password);
    return hashedPassword == storedHash;
}

bool UserAuth::isValidPassword(const std::string& password) {
    if (password.length() < 5) return false;
    bool hasLetter = false, hasDigit = false;
    for (char c : password) {
        if (std::isalpha(static_cast<unsigned char>(c))) hasLetter = true;
        if (std::isdigit(static_cast<unsigned char>(c))) hasDigit = true;
    }
    return hasLetter && hasDigit;
}
