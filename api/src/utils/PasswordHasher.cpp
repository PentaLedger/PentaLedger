#include "utils/PasswordHasher.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <sstream>
#include <iomanip>
#include <stdexcept>

std::string PasswordHasher::hash(const std::string &password)
{
    // Generate random salt
    unsigned char salt[16];
    if (RAND_bytes(salt, sizeof(salt)) != 1)
    {
        throw std::runtime_error("Failed to generate salt");
    }

    // Hash password with salt using SHA-256
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        throw std::runtime_error("Failed to create hash context");
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx, salt, sizeof(salt)) != 1 ||
        EVP_DigestUpdate(ctx, password.c_str(), password.length()) != 1 ||
        EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1)
    {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to hash password");
    }
    EVP_MD_CTX_free(ctx);

    // Encode salt and hash as hex
    std::ostringstream oss;
    for (size_t i = 0; i < sizeof(salt); ++i)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)salt[i];
    }
    for (size_t i = 0; i < hashLen; ++i)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return oss.str();
}

bool PasswordHasher::verify(const std::string &password, const std::string &hash)
{
    if (hash.length() < 32) // Minimum length for salt (16 bytes = 32 hex chars)
    {
        return false;
    }

    // Extract salt (first 32 hex chars = 16 bytes)
    std::string saltHex = hash.substr(0, 32);
    unsigned char salt[16];
    for (size_t i = 0; i < 16; ++i)
    {
        salt[i] = static_cast<unsigned char>(std::stoi(saltHex.substr(i * 2, 2), nullptr, 16));
    }

    // Hash password with extracted salt
    unsigned char computedHash[EVP_MAX_MD_SIZE];
    unsigned int hashLen;
    
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx)
    {
        return false;
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1 ||
        EVP_DigestUpdate(ctx, salt, sizeof(salt)) != 1 ||
        EVP_DigestUpdate(ctx, password.c_str(), password.length()) != 1 ||
        EVP_DigestFinal_ex(ctx, computedHash, &hashLen) != 1)
    {
        EVP_MD_CTX_free(ctx);
        return false;
    }
    EVP_MD_CTX_free(ctx);

    // Compare with stored hash
    std::string computedHashHex;
    std::ostringstream oss;
    for (size_t i = 0; i < hashLen; ++i)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)computedHash[i];
    }
    computedHashHex = oss.str();

    // Compare with stored hash (skip salt part)
    std::string storedHash = hash.substr(32);
    return computedHashHex == storedHash;
}

