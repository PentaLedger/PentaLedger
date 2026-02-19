//! \brief IRS trip types
//! \file trip_types.hpp
//!
//! \copyright
//!
//! Copyright (c) 2026 Joe Turner.
//!
//! This program is free software: you can redistribute it and/or modify
//! it under the terms of the GNU General Public License as published by
//! the Free Software Foundation, version 3.
//!
//! This program is distributed in the hope that it will be useful, but
//! WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
//! General Public License for more details.
//!
//! You should have received a copy of the GNU General Public License
//! along with this program. If not, see <http://www.gnu.org/licenses/>.
#ifndef _TRIP_TYPES_HPP_
#define _TRIP_TYPES_HPP_

namespace pentaledger
{
    namespace transportation
    {

            //! \brief Trip Type
            //!
            //! IRS regulations make a distinction when accounting for travel and
            //! vehicle expenses.
            enum IRS_TRIP_TYPE
            {
                //! \brief LOCAL trip
                TRIP_LOCAL,

                //! You are traveling away from home if:
                //! - Your duties require you to be away from the general area of 
                //!   your tax home (defined later) substantially longer than an 
                //!   ordinary day's work, and
                //! - You need to sleep or rest to meet the demands of your work 
                //!   while away from home.
                //! 
                //! This rest requirement isn’t satisfied by merely napping in your car. 
                //! You don’t have to be away from your tax home for a whole day or from 
                //! dusk to dawn as long as your relief from duty is long enough to get necessary sleep or rest. 
                TRIP_AWAY,

            };


    }
}

#endif
