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

#pragma once

#include "types.hpp"
#include "record_pointer.hpp"
#include <string>
#include <fstream>
#include <cstdint>
#include <cstddef>
#include "data_file_header.hpp"

namespace pentaledger {

// Invalid record pointer constant
constexpr RPTR INVALID_RPTR = 0xFFFFFFFFFFFFFFFFULL;

//! \brief Data file class
//! \details This class provides a way to create, open, and manage a data file.
//!
//! This class maintains a simple fixed-length record file.  Each record is a fixed length.
//! This data file contains a linked list of deleted records, which are used to allocate new records.
//! This eliminates the need to pack or compress the data file.
//!
//! \note This class is not: thread-safe, copyable, movable, constructible, or destructible.
class DataFile {
public:
    // Open or create a data file
    static DataFile create(const std::string& path, uint32_t record_length);
    static DataFile open(const std::string& path);
    
    // Non-copyable, movable
    DataFile(const DataFile&) = delete;
    DataFile& operator=(const DataFile&) = delete;
    DataFile(DataFile&&) noexcept = default;
    DataFile& operator=(DataFile&&) noexcept = default;
    
    ~DataFile();
    
    //! \brief Read a record at the given record number
    //! \param record_number The record number to read
    //! \param buffer The buffer to read the record into
    //! \details Reads the record at the given record number into the buffer
    void read_record(RPTR record_number, uint8_t* buffer);
    
    //! \brief Write a record at the logical record number
    //! \param record_number The record number to write
    //! \param buffer The buffer to write the record from
    //! \details Writes the record at the given record number from the buffer
    void write_record(RPTR record_number, const uint8_t* buffer);

    //! \brief New record
    //! \param data The data to write to the new record
    //! \details Creates a new record and writes the data to it
    RPTR new_record(void* data);

    //! \brief Delete a record at the given record number
    //! \param record_number Logical record number to delete
    //! \details Deletes the record at the given record number
    // Delete a record at the given pointer, returns 0 on success
    int delete_record(RPTR record_number);
    
    // Get header information
    const DataFileHeader& header() const { return header_; }
    uint32_t record_length() const { return header_.record_length; }
    RPTR first_record() const { return header_.first_record; }
    RPTR next_record() const { return header_.next_record; }
    uint32_t version() const { return header_.version; }
    uint64_t creation_time() const { return header_.creation_time; }
    uint64_t last_update_time() const { return header_.last_update_time; }
    
    //! \brief Flush all writes to disk
    //! \details Flushes all writes to disk
    void flush();
    
    //! \brief Dump the file header information
    //! \details Prints the file header information to stdout in a human-readable format
    void dump_header() const;
    
    //! \brief Hexdump a record
    //! \param record_number The record number to dump
    //! \details Prints the contents of the specified record in hexdump format
    void hexdump_record(RPTR record_number);
    
    // Close the file
    void close();
    
    bool is_open() const { return file_.is_open(); }

protected:
    //! \brief Calculate the file offset for a given record number
    //! \param record_number The record number to calculate the offset for
    //! \return The offset in bytes from the beginning of the file
    size_t locate_offset(RPTR record_number) const;
    
    //! \brief Initialize the data file
    //! \param path The path to the data file
    //! \param record_length The length of each record in the data file
    //! \details Initializes the data file by creating a new file and writing the header to it
    void initialize(const std::string& path, uint32_t record_length);

private:
    DataFile() = default;
    
    //! \brief Load the data file
    //! \param path The path to the data file
    //! \details Loads the data file by opening the file and reading the header from it
    void load(const std::string& path);

    //! \brief Read the header from the file
    //! \details Reads the header from the file into the header_ member variable
    void read_header();
    
    //! \brief Write the header to the file
    //! \details Writes the header_ member variable to the file
    void write_header();
    
    std::fstream file_;
    std::string file_path_;
    DataFileHeader header_;
    static constexpr size_t HEADER_SIZE = sizeof(DataFileHeader);
};

} // namespace pentaledger

