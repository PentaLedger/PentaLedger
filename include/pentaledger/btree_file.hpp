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
#include "btree_file_header.hpp"
#include <string>
#include <fstream>
#include <cstdint>
#include <cstddef>

namespace pentaledger {

// Invalid record pointer constant
constexpr RPTR INVALID_RPTR = 0xFFFFFFFFFFFFFFFFULL;

//! \brief B-tree file class
//! \details This class provides a way to create, open, and manage a B-tree index file.
//!
//! This class maintains a B-tree index structure stored in a file. Each node is a fixed size
//! defined by the NODE constant in btree_file_header.hpp.
//!
//! \note This class is not: thread-safe, copyable, movable, constructible, or destructible.
class BTreeFile {
public:
    // Open or create a B-tree file
    static BTreeFile create(const std::string& path, int key_length);
    static BTreeFile open(const std::string& path);
    
    // Non-copyable, movable
    BTreeFile(const BTreeFile&) = delete;
    BTreeFile& operator=(const BTreeFile&) = delete;
    BTreeFile(BTreeFile&&) noexcept = default;
    BTreeFile& operator=(BTreeFile&&) noexcept = default;
    
    ~BTreeFile();
    
    //! \brief Read a node at the given node pointer
    //! \param node_ptr The node pointer to read
    //! \param node The BTreeNode structure to read into
    //! \details Reads the node at the given node pointer into the provided structure
    void read_node(RPTR node_ptr, BTreeNode& node);
    
    //! \brief Write a node at the given node pointer
    //! \param node_ptr The node pointer to write
    //! \param node The BTreeNode structure to write from
    //! \details Writes the node at the given node pointer from the provided structure
    void write_node(RPTR node_ptr, const BTreeNode& node);
    

    // Get header information
    const BTreeHeader& header() const { return header_; }
    RPTR root_node() const { return header_.root_node; }
    int key_length() const { return header_.key_length; }
    int max_key_per_node() const { return header_.max_key_per_node; }
    RPTR leftmost_node() const { return header_.leftmost_node; }
    RPTR rightmost_node() const { return header_.rightmost_node; }
    
    //! \brief Flush all writes to disk
    //! \details Flushes all writes to disk
    void flush();
    
    //! \brief Dump the file header information
    //! \details Prints the file header information to stdout in a human-readable format
    void dump_header() const;
    
    // Close the file
    void close();
    
    bool is_open() const { return file_.is_open(); }
    
    //! \brief Locate a key in the B-tree
    //! \param key Pointer to the key to search for
    //! \return The record pointer (RPTR) associated with the key, or INVALID_RPTR if not found
    //! \details Searches the B-tree for the given key and returns the associated record pointer
    RPTR locate(const char* key);

protected:
    //! \brief Calculate the file offset for a given node pointer
    //! \param node_ptr The node pointer to calculate the offset for
    //! \return The offset in bytes from the beginning of the file
    size_t locate_offset(RPTR node_ptr) const;
    
    //! \brief Initialize the B-tree file
    //! \param path The path to the B-tree file
    //! \param key_length The length of each key in the B-tree
    //! \details Initializes the B-tree file by creating a new file and writing the header to it
    void initialize(const std::string& path, int key_length);

private:
    BTreeFile() = default;
    
    //! \brief Load the B-tree file
    //! \param path The path to the B-tree file
    //! \details Loads the B-tree file by opening the file and reading the header from it
    void load(const std::string& path);

    //! \brief Read the header from the file
    //! \details Reads the header from the file into the header_ member variable
    void read_header();
    
    //! \brief Write the header to the file
    //! \details Writes the header_ member variable to the file
    void write_header();
    
    std::fstream file_;
    std::string file_path_;
    BTreeHeader header_;
    RPTR next_node_ptr_;
    static constexpr size_t HEADER_SIZE = sizeof(BTreeHeader);
    static constexpr size_t NODE_SIZE = sizeof(BTreeNode);
};

} // namespace pentaledger

