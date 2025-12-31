/*
 * PentaLedger
 * Copyright (C) 2025  Joe Turner <joe@agavemountain.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "../../include/pentaledger/data_file.hpp"
#include "../../include/pentaledger/types.hpp"
#include <cstring>
#include <stdexcept>
#include <vector>
#include <ctime>
#include <iostream>
#include <iomanip>

namespace pentaledger {

DataFile DataFile::create(const std::string& path, uint32_t record_length) {
    DataFile df;
    df.initialize(path, record_length);
    return df;
}

DataFile DataFile::open(const std::string& path) {
    DataFile df;
    df.load(path);
    return df;
}

DataFile::~DataFile() {
    close();
}

void DataFile::initialize(const std::string& path, uint32_t record_length) {
    file_path_ = path;
    
    // Create new file
    file_.open(path, std::ios::out | std::ios::binary);
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to create data file: " + path);
    }
    file_.close();
    
    // Open for read/write
    file_.open(path, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to open created data file: " + path);
    }
    
    // Initialize header
    uint64_t current_time = static_cast<uint64_t>(std::time(nullptr));
    header_.magic_number = PLDB_MAGIC;
    header_.version = PLDB_VERSION;
    header_.creation_time = current_time;
    header_.last_update_time = current_time;
    header_.first_record = 0;
    header_.next_record = 1; // Start records after header
    header_.record_length = record_length;
    
    write_header();
}

void DataFile::load(const std::string& path) {
    file_path_ = path;
    
    // Open existing file
    file_.open(path, std::ios::in | std::ios::out | std::ios::binary);
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::FILE_NOT_FOUND, "Data file not found: " + path);
    }
    
    read_header();
}

void DataFile::read_header() {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    file_.seekg(0, std::ios::beg);
    if (!file_.read(reinterpret_cast<char*>(&header_), HEADER_SIZE)) {
        throw DatabaseException(ErrorCode::FILE_CORRUPTED, "Failed to read data file header");
    }
    
    // Validate magic number
    if (header_.magic_number != PLDB_MAGIC) {
        throw DatabaseException(ErrorCode::FILE_CORRUPTED, "Invalid magic number - not a PentaLedger data file");
    }
    
    // Validate version (warn if version mismatch, but allow reading)
    if (header_.version != PLDB_VERSION) {
        // Could throw an exception here if strict version checking is desired
        // For now, we'll allow reading but the version mismatch should be noted
    }
}

void DataFile::write_header() {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    // Check if file stream is in a bad state
    if (file_.bad() || file_.fail()) {
        file_.clear(); // Clear error flags
    }
    
    // Update timestamp whenever header is written
    header_.last_update_time = static_cast<uint64_t>(std::time(nullptr));
    
    // Seek to beginning of file
    file_.seekp(0, std::ios::beg);
    if (file_.fail()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to seek to beginning of file for header write");
    }
    
    // Write the header
    file_.write(reinterpret_cast<const char*>(&header_), HEADER_SIZE);
    if (file_.fail() || file_.bad()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to write data file header");
    }
    
    file_.flush();
    if (file_.fail() || file_.bad()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to flush data file header");
    }
}

size_t DataFile::locate_offset(RPTR record_number) const {
    size_t offset = HEADER_SIZE + ((record_number - 1) * header_.record_length);
   
    return offset;
}

void DataFile::read_record(RPTR record_number, uint8_t* buffer) {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    if (record_number > header_.next_record) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Invalid record pointer");
    }
    
    file_.seekg(locate_offset(record_number), std::ios::beg);
    if (!file_.read(reinterpret_cast<char*>(buffer), header_.record_length)) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to read record at pointer: " + std::to_string(record_number));
    }
}

void DataFile::write_record(RPTR record_number, const uint8_t* buffer) {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    if (record_number > header_.next_record) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Invalid record pointer");
    }
    
    // Check if file stream is in a bad state
    if (file_.bad() || file_.fail()) {
        file_.clear(); // Clear error flags
    }
    
    // Calculate offset and seek to record location
    size_t offset = locate_offset(record_number);
    file_.seekp(offset, std::ios::beg);
    if (file_.fail()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to seek to record " + std::to_string(record_number) + " at offset " + std::to_string(offset));
    }
    
    // Write the record
    file_.write(reinterpret_cast<const char*>(buffer), header_.record_length);
    if (file_.fail() || file_.bad()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to write record at pointer: " + std::to_string(record_number) + " (offset: " + std::to_string(offset) + ")");
    }
}

RPTR DataFile::new_record(void* data) {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    if (data == nullptr) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Data pointer is null");
    }
    
    RPTR record_number;

    if (header_.first_record) {
        record_number = header_.first_record;
        // Allocate a buffer and read the record
        DataFileHeader *df;
        std::vector<uint8_t> buffer(header_.record_length);
        read_record(record_number, buffer.data());
        df = reinterpret_cast<DataFileHeader*>(buffer.data());
        header_.first_record = df->next_record;
    }
    else 
    {
        record_number = header_.next_record++;
    }
    
    dump_header();

    // Write the data to the allocated record
    write_record(record_number, reinterpret_cast<const uint8_t*>(data));
    
    // Return the record pointer
    return record_number;
}

int DataFile::delete_record(RPTR record_number) {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    if (record_number > header_.next_record) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Invalid record pointer");
    }
    
    
    // Zero out the record to mark it as deleted
    std::vector<uint8_t> zero_buffer(header_.record_length, 0);
    write_record(record_number, zero_buffer.data());
    
    // Return 0 on success
    return 0;
}

void DataFile::flush() {
    if (file_.is_open()) {
        file_.flush();
    }
}

void DataFile::dump_header() const {
    std::cout << "=== DataFile Header ===" << std::endl;
    std::cout << "Magic Number: 0x" << std::hex << std::setw(8) << std::setfill('0') 
              << header_.magic_number << std::dec;
    if (header_.magic_number == PLDB_MAGIC) {
        std::cout << " (PLDB - Valid)";
    } else {
        std::cout << " (Invalid)";
    }
    std::cout << std::endl;
    
    std::cout << "Version: " << header_.version;
    if (header_.version == PLDB_VERSION) {
        std::cout << " (Current)";
    } else {
        std::cout << " (Mismatch - expected " << PLDB_VERSION << ")";
    }
    std::cout << std::endl;
    
    // Format timestamps
    time_t creation_time_t = static_cast<time_t>(header_.creation_time);
    time_t update_time_t = static_cast<time_t>(header_.last_update_time);
    char time_buf[64];
    
    std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", std::localtime(&creation_time_t));
    std::cout << "Creation Time: " << header_.creation_time << " (" << time_buf << ")" << std::endl;
    
    std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", std::localtime(&update_time_t));
    std::cout << "Last Update Time: " << header_.last_update_time << " (" << time_buf << ")" << std::endl;
    
    std::cout << "First Record: " << header_.first_record;
    if (header_.first_record == INVALID_RPTR) {
        std::cout << " (INVALID)";
    }
    std::cout << std::endl;
    
    std::cout << "Next Record: " << header_.next_record << std::endl;
    std::cout << "Record Length: " << header_.record_length << " bytes" << std::endl;
    std::cout << "Header Size: " << HEADER_SIZE << " bytes" << std::endl;
    std::cout << "======================" << std::endl;
}

void DataFile::hexdump_record(RPTR record_number) {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    if (record_number == INVALID_RPTR) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Invalid record pointer");
    }
    
    // Read the record into a buffer
    std::vector<uint8_t> buffer(header_.record_length);
    read_record(record_number, buffer.data());
    
    // Calculate file offset for display
    size_t file_offset = locate_offset(record_number);
    
    std::cout << "=== Record " << record_number << " (File Offset: 0x" << std::hex << file_offset << std::dec << ") ===" << std::endl;
    
    const size_t bytes_per_line = 16;
    for (size_t i = 0; i < buffer.size(); i += bytes_per_line) {
        // Print offset
        std::cout << std::hex << std::setw(8) << std::setfill('0') << i << ": ";
        
        // Print hex bytes
        for (size_t j = 0; j < bytes_per_line; ++j) {
            if (i + j < buffer.size()) {
                std::cout << std::hex << std::setw(2) << std::setfill('0') 
                          << static_cast<unsigned int>(buffer[i + j]) << " ";
            } else {
                std::cout << "   ";
            }
        }
        
        // Print ASCII representation
        std::cout << " |";
        for (size_t j = 0; j < bytes_per_line && (i + j) < buffer.size(); ++j) {
            uint8_t byte = buffer[i + j];
            if (byte >= 32 && byte < 127) {
                std::cout << static_cast<char>(byte);
            } else {
                std::cout << ".";
            }
        }
        std::cout << "|" << std::dec << std::endl;
    }
    
    std::cout << "Total: " << buffer.size() << " bytes" << std::endl;
    std::cout << "=========================" << std::endl;
}

void DataFile::close() {
    if (file_.is_open()) {
        write_header(); // Save header before closing
        file_.close();
    }
}

} // namespace pentaledger

