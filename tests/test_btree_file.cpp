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
#include "pentaledger/btree_file.hpp"
#include <filesystem>
#include <cstring>

using namespace pentaledger;

// Test helper class to access protected methods (if needed in future)
// class TestableBTreeFile : public BTreeFile {
// public:
//     using BTreeFile::locate_offset;
//     using BTreeFile::initialize;
//     
//     // Inherit move constructor and assignment
//     TestableBTreeFile(TestableBTreeFile&&) = default;
//     TestableBTreeFile& operator=(TestableBTreeFile&&) = default;
//     
//     // Use the static create method to construct
//     static TestableBTreeFile create(const std::string& path, int key_length) {
//         BTreeFile btf = BTreeFile::create(path, key_length);
//         // Move the BTreeFile into TestableBTreeFile
//         return std::move(*static_cast<TestableBTreeFile*>(&btf));
//     }
// };

class BTreeFileTest : public ::testing::Test {
protected:
    void SetUp() override {
        test_file_ = "test_btree_file.btree";
        // Clean up any existing test file
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }
    
    void TearDown() override {
        // Clean up test file after each test
        if (std::filesystem::exists(test_file_)) {
            std::filesystem::remove(test_file_);
        }
    }
    
    std::string test_file_;
    static constexpr int KEY_LENGTH = 20;
};

// Test creating a new B-tree file
TEST_F(BTreeFileTest, CreateBTreeFile) {
    BTreeFile btf = BTreeFile::create(test_file_, KEY_LENGTH);
    
    EXPECT_TRUE(btf.is_open());
    EXPECT_EQ(btf.key_length(), KEY_LENGTH);
    EXPECT_EQ(btf.root_node(), 0);
    EXPECT_EQ(btf.leftmost_node(), 0);
    EXPECT_EQ(btf.rightmost_node(), 0);
    EXPECT_GT(btf.max_key_per_node(), 0);
    
    btf.close();
    EXPECT_FALSE(btf.is_open());
}

// Test opening an existing B-tree file
TEST_F(BTreeFileTest, OpenBTreeFile) {
    // First create a file
    {
        BTreeFile btf = BTreeFile::create(test_file_, KEY_LENGTH);
        btf.close();
    }
    
    // Then open it
    BTreeFile btf = BTreeFile::open(test_file_);
    
    EXPECT_TRUE(btf.is_open());
    EXPECT_EQ(btf.key_length(), KEY_LENGTH);
    EXPECT_EQ(btf.root_node(), 0);
    
    btf.close();
}

// Test header persistence
TEST_F(BTreeFileTest, HeaderPersistence) {
    const int test_key_length = 32;
    
    // Create and close
    {
        BTreeFile btf = BTreeFile::create(test_file_, test_key_length);
        btf.close();
    }
    
    // Open and verify header
    BTreeFile btf = BTreeFile::open(test_file_);
    EXPECT_EQ(btf.key_length(), test_key_length);
    EXPECT_EQ(btf.root_node(), 0);
    EXPECT_EQ(btf.leftmost_node(), 0);
    EXPECT_EQ(btf.rightmost_node(), 0);
    
    btf.close();
}

// Test multiple open/close operations
TEST_F(BTreeFileTest, MultipleOpenClose) {
    BTreeFile btf1 = BTreeFile::create(test_file_, KEY_LENGTH);
    EXPECT_TRUE(btf1.is_open());
    btf1.close();
    EXPECT_FALSE(btf1.is_open());
    
    BTreeFile btf2 = BTreeFile::open(test_file_);
    EXPECT_TRUE(btf2.is_open());
    btf2.close();
    EXPECT_FALSE(btf2.is_open());
}

// Test move semantics
TEST_F(BTreeFileTest, MoveSemantics) {
    BTreeFile btf1 = BTreeFile::create(test_file_, KEY_LENGTH);
    EXPECT_TRUE(btf1.is_open());
    
    BTreeFile btf2 = std::move(btf1);
    EXPECT_FALSE(btf1.is_open());
    EXPECT_TRUE(btf2.is_open());
    EXPECT_EQ(btf2.key_length(), KEY_LENGTH);
    
    btf2.close();
}

// Test flush operation
TEST_F(BTreeFileTest, Flush) {
    BTreeFile btf = BTreeFile::create(test_file_, KEY_LENGTH);
    EXPECT_TRUE(btf.is_open());
    
    // Flush should not throw
    EXPECT_NO_THROW(btf.flush());
    
    btf.close();
}

// Test close operation
TEST_F(BTreeFileTest, Close) {
    BTreeFile btf = BTreeFile::create(test_file_, KEY_LENGTH);
    EXPECT_TRUE(btf.is_open());
    
    btf.close();
    EXPECT_FALSE(btf.is_open());
    
    // Closing again should be safe
    EXPECT_NO_THROW(btf.close());
}

// Test is_open() method
TEST_F(BTreeFileTest, IsOpen) {
    BTreeFile btf = BTreeFile::create(test_file_, KEY_LENGTH);
    EXPECT_TRUE(btf.is_open());
    
    btf.close();
    EXPECT_FALSE(btf.is_open());
}

// Test header getters
TEST_F(BTreeFileTest, HeaderGetters) {
    BTreeFile btf = BTreeFile::create(test_file_, KEY_LENGTH);
    
    const BTreeHeader& header = btf.header();
    EXPECT_EQ(header.key_length, KEY_LENGTH);
    EXPECT_EQ(header.root_node, 0);
    EXPECT_EQ(header.leftmost_node, 0);
    EXPECT_EQ(header.rightmost_node, 0);
    EXPECT_EQ(header.locked, 0);
    
    EXPECT_EQ(btf.key_length(), header.key_length);
    EXPECT_EQ(btf.root_node(), header.root_node);
    EXPECT_EQ(btf.leftmost_node(), header.leftmost_node);
    EXPECT_EQ(btf.rightmost_node(), header.rightmost_node);
    EXPECT_EQ(btf.max_key_per_node(), header.max_key_per_node);
    
    btf.close();
}

// Test max_key_per_node calculation
TEST_F(BTreeFileTest, MaxKeyPerNode) {
    const int key_len_10 = 10;
    const int key_len_20 = 20;
    const int key_len_50 = 50;
    
    {
        BTreeFile btf1 = BTreeFile::create("test1.btree", key_len_10);
        int max1 = btf1.max_key_per_node();
        btf1.close();
        
        BTreeFile btf2 = BTreeFile::create("test2.btree", key_len_20);
        int max2 = btf2.max_key_per_node();
        btf2.close();
        
        BTreeFile btf3 = BTreeFile::create("test3.btree", key_len_50);
        int max3 = btf3.max_key_per_node();
        btf3.close();
        
        // Smaller key length should allow more keys per node
        EXPECT_GT(max1, max2);
        EXPECT_GT(max2, max3);
        EXPECT_GT(max1, 0);
        EXPECT_GT(max2, 0);
        EXPECT_GT(max3, 0);
        
        // Clean up
        std::filesystem::remove("test1.btree");
        std::filesystem::remove("test2.btree");
        std::filesystem::remove("test3.btree");
    }
}

// Test locate_offset protected method (commented out since locate_offset is not implemented)
// TEST_F(BTreeFileTest, LocateOffset) {
//     // Use TestableBTreeFile to access protected locate_offset method
//     TestableBTreeFile testable_btf = TestableBTreeFile::create(test_file_, KEY_LENGTH);
//     
//     size_t header_size = sizeof(BTreeHeader);
//     size_t node_size = sizeof(BTreeNode);
//     
//     // Test for node pointer 1
//     RPTR node_1 = 1;
//     size_t offset_1 = testable_btf.locate_offset(node_1);
//     size_t expected_1 = header_size + ((node_1 - 1) * node_size);
//     EXPECT_EQ(offset_1, expected_1) << "Node 1: offset calculation mismatch";
//     
//     // Test for node pointer 2
//     RPTR node_2 = 2;
//     size_t offset_2 = testable_btf.locate_offset(node_2);
//     size_t expected_2 = header_size + ((node_2 - 1) * node_size);
//     EXPECT_EQ(offset_2, expected_2) << "Node 2: offset calculation mismatch";
//     
//     // Verify that offset_2 is exactly node_size more than offset_1
//     EXPECT_EQ(offset_2, offset_1 + node_size) << "Offsets should differ by node_size for consecutive node pointers";
//     
//     testable_btf.close();
// }

// Test error handling for invalid file path
TEST_F(BTreeFileTest, InvalidFilePath) {
    // Try to open a non-existent file
    EXPECT_THROW(BTreeFile::open("nonexistent_file.btree"), DatabaseException);
}

// Test file existence check
TEST_F(BTreeFileTest, FileExistence) {
    // File should not exist initially
    EXPECT_FALSE(std::filesystem::exists(test_file_));
    
    // Create file
    {
        BTreeFile btf = BTreeFile::create(test_file_, KEY_LENGTH);
        btf.close();
    }
    
    // File should exist now
    EXPECT_TRUE(std::filesystem::exists(test_file_));
}

// Test that create overwrites existing file
TEST_F(BTreeFileTest, CreateOverwritesExisting) {
    // Create first file
    {
        BTreeFile btf1 = BTreeFile::create(test_file_, 10);
        btf1.close();
    }
    
    // Create again with different key length
    {
        BTreeFile btf2 = BTreeFile::create(test_file_, 20);
        EXPECT_EQ(btf2.key_length(), 20);
        btf2.close();
    }
}

