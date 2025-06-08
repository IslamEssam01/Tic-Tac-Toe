#include "user_auth.h"
#include <openssl/evp.h>
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
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;

    // Create a new EVP_MD_CTX
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        return "";
    }

    // Initialize the digest context for SHA256
    if (!EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr)) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    // Add password data to the context
    if (!EVP_DigestUpdate(ctx, password.c_str(), password.length())) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    // Generate the hash
    if (!EVP_DigestFinal_ex(ctx, hash, &hashLen)) {
        EVP_MD_CTX_free(ctx);
        return "";
    }

    // Clean up the context
    EVP_MD_CTX_free(ctx);

    // Convert to hex string
    std::stringstream ss;
    for (unsigned int i = 0; i < hashLen; i++) {
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