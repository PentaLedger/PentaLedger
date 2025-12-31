#pragma once
#include <cstdint>

namespace pentaledger {

typedef uint64_t RPTR;

struct RecordPointer {

    //! \brief First record pointer
    //! Pointer to first record in the file
    RPTR first_record;

    //! \brief Next record pointer
    //! Pointer to next available position for new record
    RPTR next_record;
};

};
