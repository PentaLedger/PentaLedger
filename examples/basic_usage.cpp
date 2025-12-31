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

#include "pentaledger/data_file.hpp"
#include <iostream>
#include <filesystem>
#include <cstring>
#include <array>

using namespace pentaledger;

int main() {
    const std::string data_file = "example.dat";
    constexpr uint32_t RECORD_LENGTH = 256;
    
    // Clean up any existing file
    if (std::filesystem::exists(data_file)) {
        std::filesystem::remove(data_file);
    }
    
    // Create data file
    std::cout << "Creating data file..." << std::endl;
    DataFile df = DataFile::create(data_file, RECORD_LENGTH);
    
    std::cout << "Record length: " << df.record_length() << std::endl;
    std::cout << "First record pointer: " << df.first_record() << std::endl;
    std::cout << "Next record pointer: " << df.next_record() << std::endl;
    
    // Allocate and write a record
    std::cout << "\nAllocating and writing records..." << std::endl;
    std::cout << "Allocated record at pointer: " << ptr1 << std::endl;
    
    std::array<uint8_t, RECORD_LENGTH> buffer1{};
    const char* data1 = "First record data";
    std::memcpy(buffer1.data(), data1, std::strlen(data1));
    df.write_record(ptr1, buffer1.data());
    
    // // Allocate and write another record
    // RPTR ptr2 = df.allocate_record();
    // std::cout << "Allocated record at pointer: " << ptr2 << std::endl;
    
    // std::array<uint8_t, RECORD_LENGTH> buffer2{};
    // const char* data2 = "Second record data";
    // std::memcpy(buffer2.data(), data2, std::strlen(data2));
    // df.write_record(ptr2, buffer2.data());
    
    // // Set first record pointer
    // df.set_first_record(ptr1);
    // std::cout << "\nFirst record pointer set to: " << df.first_record() << std::endl;
    
    // // Read records back
    // std::cout << "\nReading records back..." << std::endl;
    // std::array<uint8_t, RECORD_LENGTH> read_buffer{};
    
    // df.read_record(ptr1, read_buffer.data());
    // std::cout << "Record 1: " << read_buffer.data() << std::endl;
    
    // df.read_record(ptr2, read_buffer.data());
    // std::cout << "Record 2: " << read_buffer.data() << std::endl;
    
    // // Close and reopen to verify persistence
    // std::cout << "\nClosing and reopening file..." << std::endl;
    // df.close();
    
    DataFile df2 = DataFile::open(data_file);
    std::cout << "First record pointer: " << df2.first_record() << std::endl;
    std::cout << "Next record pointer: " << df2.next_record() << std::endl;
    
    // Read first record
    df2.read_record(df2.first_record(), read_buffer.data());
    std::cout << "First record after reopen: " << read_buffer.data() << std::endl;
    
    df2.close();
    std::cout << "\nData file closed." << std::endl;
    
    return 0;
}
