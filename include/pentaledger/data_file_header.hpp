#pragma once

#include "record_pointer.hpp"
#include <cstdint>

namespace pentaledger {
// Magic number constant: "PLDB" as a 32-bit value (little-endian)
constexpr uint32_t PLDB_MAGIC = static_cast<uint32_t>('P' | ('L' << 8) | ('D' << 16) | ('B' << 24));

// File format version
constexpr uint32_t PLDB_VERSION = 1;

//! \brief File header structure
//! \details This is a fixed size header that is written to the beginning of the data file.
struct DataFileHeader {
    //! \brief MAGIC Number
    //! Magic number to identify the data file.  Should be PLDB_MAGIC.
    uint32_t magic_number; 

    //! \brief File format version
    //! File format version.  Should be PLDB_VERSION.
    uint32_t version; 
    
    //! \brief File creation time
    //! File creation time as Unix timestamp (seconds since epoch)
    uint64_t creation_time; 
 
    //! \brief Last update time
    //! Last update time as Unix timestamp (seconds since epoch)
    uint64_t last_update_time;
 
    //! \brief First record pointer
    //! Pointer to first deleted record in the file
    RPTR first_record;

    //! \brief Next record pointer
    //! Pointer to next available position for new record
    RPTR next_record;

    //! \brief Record Length
    //! \details Fixed length of each record.  This is the length of the data stored in each record.
    uint32_t record_length;
};
}