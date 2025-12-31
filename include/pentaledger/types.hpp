#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>
#include <optional>

namespace pentaledger {

// Page ID type
using PageId = uint32_t;
constexpr PageId INVALID_PAGE_ID = 0xFFFFFFFF;

// Page size (4KB default)
constexpr size_t PAGE_SIZE = 4096;

// Field types for schema
enum class FieldType {
    INT32,
    INT64,
    FLOAT,
    DOUBLE,
    STRING,
    BLOB
};

// Key type (for now, using int64_t, can be extended)
using Key = int64_t;

// Error codes
enum class ErrorCode {
    SUCCESS,
    FILE_NOT_FOUND,
    FILE_CORRUPTED,
    PAGE_NOT_FOUND,
    INSUFFICIENT_SPACE,
    INVALID_SCHEMA,
    DUPLICATE_KEY,
    KEY_NOT_FOUND,
    IO_ERROR
};

// Exception class for database errors
class DatabaseException : public std::exception {
public:
    DatabaseException(ErrorCode code, const std::string& message)
        : code_(code), message_(message) {}
    
    ErrorCode code() const noexcept { return code_; }
    const char* what() const noexcept override { return message_.c_str(); }

private:
    ErrorCode code_;
    std::string message_;
};

} // namespace pentaledger

