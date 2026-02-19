// SPDX-License-Identifier: GPL-2.0-only
//! \file MileageCatagories.hpp
//! \author Joe Turner <joe.turner@agavemountain.com>
//!
//! \copyright
//! Copyright (c) 2026 Joe Turner.
//!
//! This program is free software; you can redistribute it and/or
//! modify it under the terms of the GNU General Public License
//! as published by the Free Software Foundation; either version 2
//! of the License, or (at your option) any later version.
//!
//! This program is distributed in the hope that it will be useful,
//! but WITHOUT ANY WARRANTY; without even the implied warranty of
//! MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//! GNU General Public License for more details.
//!
//! You should have received a copy of the GNU General Public License
//! along with this program; If not, see <http://www.gnu.org/licenses/>.
#ifndef _MILEAGE_CATAGORIES_HPP_
#define _MILEAGE_CATAGORIES_HPP_

namespace pentaledger
{
    namespace transportation
    {

        //! \brief Mileage Categories
        //!
        //! Tracked vehicle mileage can be broadly categorized into the following
        //! categories: that which can be deducted, that which can be reimbursed,
        //! and that which cannot be deducted or reimbursed.  Further, how we
        //! categorize the trips is important because each category is valued
        //! at a different cent per mile.
        //!
        enum eMileageCatgories
        {
            //! \brief Medical mileage
            //!
            //! Trips to the doctor, as well as other hospital or dentist visits,
            //! are eligible for medical mileage rate deductions. However, it is
            //! not every medical visit that you can get reimbursement for. The
            //! ones you can are usually not covered by your employer in your
            //! healthcare plan. However, while the medical deduction can save
            //! you a lot of money, the rates are limited by your adjusted
            //! gross income.
            MILEAGE_MEDICAL,

            //! \brief Business mileage
            //!
            //! The IRS defines business mileage as mileage that is driven between two
            //! places of work, permanent or temporary.
            //!
            //! Note, we track mileage that may be attributed to different categrories,
            //! such as side-hustles (Schedule C), and mileage may be reimbursed by
            //! a W-2 job.
            MILEAGE_BUSINESS,

            //! \brief Personal mileage
            //!
            //! Personal usage and commute to/from work.  Basically, this is
            //! anything that isn't covered by the other categories.
            MILEAGE_PERSONAL,

            //! \brief Charity mileage
            //!
            //! Mileage for volunteering for a qualifed non-profit charity
            MILEAGE_CHARITY,

            //! \brief Moving mileage
            //!
            //! It used to be that moving expenses were deductable, but the
            //! tax law changed.  Now:
            //!
            //! Only active-duty members of the military can deduct mileage
            //! related to moving. The move has to be related to a permanent
            //! change of station.
            MILEAGE_MOVING,

            //! \brief ODOMETER CHECK
            //!
            //! At the beginning of the year (or when a vehicle is acquired),
            //! and at the end of the year (or at other specified intervals),
            //! the odometer value should be written down.  This allows us to
            //! calculate the percentage of mileage by category.  While the
            //! IRS is primarily concerned with percentage of business use
            //! versus percentage of personal use, it can be useful in other
            //! regards as well.
            MILEAGE_ODO_CHECK,

            //! \brief Mileage service
            //!
            //! This is informational to link service records to the vehicle.
            MILEAGE_SERVICE,

            //! \brief Fuel Stop
            //!
            //! Fuel stop is where we can book-end fuel stops and calculate
            //! MPG.  Also for trucking, IFTA tax compliance issues would
            //! be calculated from this.
            MILEAGE_FUEL_STOP,

        };

    }
}

#endif
