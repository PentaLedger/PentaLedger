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

#include "../../include/pentaledger/btree_file.hpp"
#include "../../include/pentaledger/types.hpp"
#include <cstring>
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <ctime>

namespace pentaledger {

BTreeFile BTreeFile::create(const std::string& path, int key_length) {
    BTreeFile btf;
    btf.initialize(path, key_length);
    return btf;
}

BTreeFile BTreeFile::open(const std::string& path) {
    BTreeFile btf;
    btf.load(path);
    return btf;
}

BTreeFile::~BTreeFile() {
    close();
}

void BTreeFile::initialize(const std::string& path, int key_length) {
    file_path_ = path;
    
    // Open file for reading and writing, create if it doesn't exist
    file_.open(path, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!file_.is_open()) {
        // File doesn't exist, create it
        file_.open(path, std::ios::out | std::ios::binary);
        file_.close();
        file_.open(path, std::ios::in | std::ios::out | std::ios::binary);
    }
    
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to open B-tree file: " + path);
    }
    
    // Initialize header
    std::memset(&header_, 0, sizeof(BTreeHeader));
    header_.root_node = 0;
    header_.key_length = key_length;
    header_.max_key_per_node = (NODE - (sizeof(int) * 2) - (ADR * 4)) / (key_length + ADR);
    header_.raised_node = 0;
    header_.locked = 0;
    header_.leftmost_node = 0;
    header_.rightmost_node = 0;
    
    next_node_ptr_ = 1;
    
    // Write header to file
    write_header();
}

void BTreeFile::load(const std::string& path) {
    file_path_ = path;
    
    // Open file for reading and writing
    file_.open(path, std::ios::in | std::ios::out | std::ios::binary);
    
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to open B-tree file: " + path);
    }
    
    // Read header from file
    read_header();
    
    // Calculate next_node_ptr_ from file size
    file_.seekg(0, std::ios::end);
    std::streampos file_size_pos = file_.tellg();
    file_.seekg(0, std::ios::beg);
    
    size_t file_size = static_cast<size_t>(file_size_pos);
    if (file_size > HEADER_SIZE) {
        size_t nodes_in_file = (file_size - HEADER_SIZE) / NODE_SIZE;
        next_node_ptr_ = static_cast<RPTR>(nodes_in_file) + 1;
    } else {
        next_node_ptr_ = 1;
    }
}

void BTreeFile::read_header() {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    file_.seekg(0, std::ios::beg);
    if (!file_.read(reinterpret_cast<char*>(&header_), HEADER_SIZE)) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to read B-tree file header");
    }
}

void BTreeFile::write_header() {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    // Clear any existing error flags before seeking and writing
    file_.clear();
    file_.seekp(0, std::ios::beg);
    if (file_.fail()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to seek to beginning of file for header write");
    }
    
    if (!file_.write(reinterpret_cast<const char*>(&header_), HEADER_SIZE)) {
        if (file_.bad()) {
            throw DatabaseException(ErrorCode::IO_ERROR, "Critical error writing B-tree file header (badbit set)");
        } else if (file_.fail()) {
            throw DatabaseException(ErrorCode::IO_ERROR, "Non-critical error writing B-tree file header (failbit set)");
        } else {
            throw DatabaseException(ErrorCode::IO_ERROR, "Failed to write B-tree file header");
        }
    }
    file_.flush();
    if (file_.fail()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Failed to flush B-tree file header to disk");
    }
}

void BTreeFile::flush() {
    if (file_.is_open()) {
        file_.flush();
    }
}

void BTreeFile::dump_header() const {
    std::cout << "=== BTreeFile Header ===" << std::endl;
    std::cout << "Root Node: " << header_.root_node;
    if (header_.root_node == INVALID_RPTR) {
        std::cout << " (INVALID)";
    }
    std::cout << std::endl;
    
    std::cout << "Key Length: " << header_.key_length << " bytes" << std::endl;
    std::cout << "Max Keys Per Node: " << header_.max_key_per_node << std::endl;
    std::cout << "Raised Node: " << header_.raised_node;
    if (header_.raised_node == INVALID_RPTR) {
        std::cout << " (INVALID)";
    }
    std::cout << std::endl;
    
    std::cout << "Locked: " << (header_.locked ? "Yes" : "No") << std::endl;
    
    std::cout << "Leftmost Node: " << header_.leftmost_node;
    if (header_.leftmost_node == INVALID_RPTR) {
        std::cout << " (INVALID)";
    }
    std::cout << std::endl;
    
    std::cout << "Rightmost Node: " << header_.rightmost_node;
    if (header_.rightmost_node == INVALID_RPTR) {
        std::cout << " (INVALID)";
    }
    std::cout << std::endl;
    
    std::cout << "Header Size: " << HEADER_SIZE << " bytes" << std::endl;
    std::cout << "Node Size: " << NODE_SIZE << " bytes" << std::endl;
    std::cout << "Next Node Pointer: " << next_node_ptr_ << std::endl;
    std::cout << "======================" << std::endl;
}

void BTreeFile::close() {
    if (file_.is_open()) {
        header_.locked = 0;
        write_header(); // Write header before closing
        file_.close();
    }
}

RPTR BTreeFile::locate(const char* key) {
    if (!file_.is_open()) {
        throw DatabaseException(ErrorCode::IO_ERROR, "File not open");
    }
    
    if (key == nullptr) {
        throw DatabaseException(ErrorCode::IO_ERROR, "Key pointer is null");
    }
    
    // If there's no root node, the tree is empty
    if (header_.root_node == 0) {
        return INVALID_RPTR;
    }
    
    // Helper function to read a node directly from file
    auto read_node_direct = [this](RPTR node_ptr, BTreeNode& node) -> bool {
        if (node_ptr == 0) {
            return false;
        }
        
        size_t offset = HEADER_SIZE + ((node_ptr - 1) * NODE_SIZE);
        file_.seekg(offset, std::ios::beg);
        if (file_.fail()) {
            return false;
        }
        
        if (!file_.read(reinterpret_cast<char*>(&node), NODE_SIZE)) {
            return false;
        }
        
        return true;
    };
    
    // Helper function to compare keys
    auto compare_key = [this](const char* key1, const char* key2) -> int {
        return std::memcmp(key1, key2, header_.key_length);
    };
    
    // Helper function to get key at index in node
    auto get_key_at = [this](const BTreeNode& node, int index, char* key_buf) -> bool {
        if (index < 0 || index >= node.key_count) {
            return false;
        }
        
        // Calculate offset in keyspace
        // Each entry is (key_length + ADR) bytes
        size_t entry_size = header_.key_length + ADR;
        size_t offset = index * entry_size;
        
        if (offset + header_.key_length > sizeof(node.keyspace)) {
            return false;
        }
        
        std::memcpy(key_buf, node.keyspace + offset, header_.key_length);
        return true;
    };
    
    // Helper function to get RPTR at index in node
    auto get_rptr_at = [this](const BTreeNode& node, int index) -> RPTR {
        if (index < 0 || index >= node.key_count) {
            return INVALID_RPTR;
        }
        
        // Calculate offset in keyspace
        // Each entry is (key_length + ADR) bytes
        size_t entry_size = header_.key_length + ADR;
        size_t offset = index * entry_size + header_.key_length;
        
        if (offset + ADR > sizeof(node.keyspace)) {
            return INVALID_RPTR;
        }
        
        RPTR rptr;
        std::memcpy(&rptr, node.keyspace + offset, ADR);
        return rptr;
    };
    
    // Helper function to get child pointer at index (for non-leaf nodes)
    auto get_child_ptr = [this](const BTreeNode& node, int index) -> RPTR {
        // In a B-tree, child pointers are typically stored between keys
        // This is a simplified version - actual structure may vary
        if (index < 0 || index > node.key_count) {
            return INVALID_RPTR;
        }
        
        // For now, return INVALID_RPTR as this requires understanding the exact layout
        return INVALID_RPTR;
    };
    
    // Start search from root
    RPTR current_node_ptr = header_.root_node;
    BTreeNode node;
    char key_buf[MAX_KEY_LENGTH];
    
    while (current_node_ptr != 0) {
        if (!read_node_direct(current_node_ptr, node)) {
            return INVALID_RPTR;
        }
        
        // Search for key in current node
        int i = 0;
        
        // Search through keyspace
        for (i = 0; i < node.key_count; ++i) {
            if (!get_key_at(node, i, key_buf)) {
                break;
            }
            
            int cmp = compare_key(key, key_buf);
            if (cmp == 0) {
                // Found the key - return its associated RPTR
                RPTR result = get_rptr_at(node, i);
                return result;
            } else if (cmp < 0) {
                // Key is less than current key
                if (node.nonleaf) {
                    // In a non-leaf node, follow the child pointer before this key
                    // For now, return not found as child pointer traversal needs exact structure
                    return INVALID_RPTR;
                } else {
                    // In a leaf node, key not found
                    return INVALID_RPTR;
                }
            }
        }
        
        // If this is a leaf node and we've checked all keys, not found
        if (!node.nonleaf) {
            return INVALID_RPTR;
        }
        
        // For non-leaf nodes, if key is greater than all keys, follow rightmost child
        // For now, return not found as this requires exact B-tree structure knowledge
        return INVALID_RPTR;
    }
    
    return INVALID_RPTR;
}

} // namespace pentaledger

