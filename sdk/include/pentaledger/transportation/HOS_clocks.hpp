//! \file HOS_clocks.hpp
//! \author Joe Turner <joe.turner@infinity-surge.com>
//!
//! \copyright
//! Copyright (c) 2026 Joe Turner
//!
//! This program is free software: you can redistribute it and/or modify
//! it under the terms of the GNU General Public License as published by
//! the Free Software Foundation, version 2.
//!
//! This program is distributed in the hope that it will be useful, but
//! WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//! General Public License for more details.
//!
//! You should have received a copy of the GNU General Public License
//! along with this program. If not, see <http://www.gnu.org/licenses/>.
#ifndef _HOS_CLOCKS_HPP_
#define _HOS_CLOCKS_HPP_
#include <time.h>

namespace pentaledger
{
    namespace transportation
    {
        class HOSClock
        {
            time_t break_clock;
            time_t drive_clock;
            time_t shift_clock;
            time_t cycle_clock;
        };
    }
}

#endif