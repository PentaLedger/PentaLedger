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

#include <gtest/gtest.h>
#include "pentaledger/data_file.hpp"
#include <filesystem>
#include <cstring>
#include <array>

using namespace pentaledger;

// Test helper class to access protected methods
class TestableDataFile : public DataFile {
public:
    using DataFile::locate_offset;
    using DataFile::initialize;
    
    // Inherit move constructor and assignment
    TestableDataFile(TestableDataFile&&) = default;
    TestableDataFile& operator=(TestableDataFile&&) = default;
    
    // Use the static create method to construct
    static TestableDataFile create(const std::string& path, uint32_t record_length) {
        DataFile df = DataFile::create(path, record_length);
        // Move the DataFile into TestableDataFile
        return std::move(*static_cast<TestableDataFile*>(&df));
    }
};

class DataFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_file_ = "test_data_file.dat";
        // Clean up any existing test file
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }
    
    void TearDown() override {
        // Clean up test file
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }
    
    std::string test_file_;
    static constexpr uint32_t RECORD_LENGTH = 256;
};

TEST_F(DataFileTest, CreateDataFile) {
    DataFile df = DataFile::create(test_file_, RECORD_LENGTH);
    
    EXPECT_TRUE(df.is_open());
    EXPECT_EQ(df.record_length(), RECORD_LENGTH);
    EXPECT_EQ(df.first_record(), 0);
    EXPECT_EQ(df.next_record(), 1);
    
    EXPECT_TRUE(std::filesystem::exists(test_file_));
    
    df.close();
}

TEST_F(DataFileTest, OpenExistingDataFile) {
    // Create a file first
    {
        DataFile df = DataFile::create(test_file_, RECORD_LENGTH);
        df.close();
    }
    
    // Open it
    DataFile df = DataFile::open(test_file_);
    
    EXPECT_TRUE(df.is_open());
    EXPECT_EQ(df.record_length(), RECORD_LENGTH);
    
    df.close();
}

TEST_F(DataFileTest, HeaderPersistence) {
    constexpr uint32_t test_record_length = 512;
    RPTR first = 1;
    
    // Create and modify header
    {
        DataFile df = DataFile::create(test_file_, test_record_length);
        // Use new_record to allocate a record (next_record starts at 1)
        std::vector<uint8_t> data(test_record_length, 0xAA);
        first = df.new_record(data.data());
        df.close();
    }
    
    // Reopen and verify
    DataFile df = DataFile::open(test_file_);
    
    EXPECT_EQ(df.record_length(), test_record_length);
    // After allocating one record: next_record starts at 0, so after new_record it's 1

    EXPECT_EQ(df.next_record(), 2);
    
    df.close();
}

TEST_F(DataFileTest, MultipleOpenClose) {
    DataFile df = DataFile::create(test_file_, RECORD_LENGTH);
    
    // Use new_record to allocate and write a record (will allocate record 0 since first_record is 0)
    std::array<uint8_t, RECORD_LENGTH> write_buffer{};
    std::memset(write_buffer.data(), 0x55, RECORD_LENGTH);
    RPTR ptr = df.new_record(write_buffer.data());  // returns this record number
    EXPECT_EQ(ptr, 1); // Should be 0 since next_record starts at 0
    df.close();
    
    // Reopen multiple times
    for (int i = 0; i < 3; ++i) {
        DataFile df2 = DataFile::open(test_file_);
        EXPECT_EQ(df2.next_record(), ptr + 1);
        EXPECT_EQ(df2.record_length(), RECORD_LENGTH);
        
        std::array<uint8_t, RECORD_LENGTH> read_buffer{};
        df2.read_record(ptr, read_buffer.data());
        EXPECT_EQ(read_buffer[0], 0x55);
        
        df2.close();
    }
}

TEST_F(DataFileTest, Flush) {
    std::cout << "test_file_: " << test_file_ << std::endl;
    std::cout << "RECORD_LENGTH: " << RECORD_LENGTH << std::endl;
    DataFile df = DataFile::create(test_file_, RECORD_LENGTH);
    
    // Use new_record to allocate and write a record
    std::array<uint8_t, RECORD_LENGTH> buffer{};
    std::memset(buffer.data(), 0x77, RECORD_LENGTH);
    std::cout << "New record\n";
    RPTR ptr = df.new_record(buffer.data());
    
    // Flush should not throw
    EXPECT_NO_THROW(df.flush());
    
    df.close();
}

TEST_F(DataFileTest, FileNotFound) {
    EXPECT_THROW(DataFile::open("nonexistent_file.dat"), DatabaseException);
}

TEST_F(DataFileTest, MoveSemantics) {
    DataFile df1 = DataFile::create(test_file_, RECORD_LENGTH);
    // Use new_record to allocate a record
    std::vector<uint8_t> data(RECORD_LENGTH, 0x42);
    RPTR ptr = df1.new_record(data.data());
    
    // Move to another DataFile
    DataFile df2 = std::move(df1);
    
    EXPECT_FALSE(df1.is_open());
    EXPECT_TRUE(df2.is_open());
    // After allocating one record: next_record starts at 1, so after new_record it's 2
    std::cout << "next_record: " << df2.next_record() << std::endl;
    EXPECT_EQ(df2.next_record(), static_cast<RPTR>(2));
    
    df2.close();
}

TEST_F(DataFileTest, LocateOffset) {
    // Use TestableDataFile to access protected locate_offset method
    TestableDataFile testable_df = TestableDataFile::create(test_file_, RECORD_LENGTH);
    
    // The function calculates: (record_number - 1 * record_length) + HEADER_SIZE
    // Due to operator precedence: record_number - record_length + HEADER_SIZE
    size_t header_size = sizeof(DataFileHeader);
    
       // Test for record number 1
    RPTR record_1 = 1;
    size_t offset_1 = testable_df.locate_offset(record_1);
    size_t expected_1 = static_cast<size_t>(static_cast<RPTR>(header_size) + ( (static_cast<RPTR>(1) - 1) * static_cast<RPTR>(RECORD_LENGTH)) );
    EXPECT_EQ(offset_1, expected_1) << "Record 1: offset calculation mismatch";
    
    
    testable_df.close();
}

