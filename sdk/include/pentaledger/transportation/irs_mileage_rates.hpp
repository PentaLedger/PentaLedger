//! \brief IRS Mileage Rates
//! \file irs_mileage_rates.hpp
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
#ifndef _IRS_MILEAGE_RATES_
#define _IRS_MILEAGE_RATES_
#include <time.h>
#include "pentaledger/transportation/MileageCategories.hpp"

namespace pentaledger
{
    namespace transportation
    {

        /** @addtogroup MILEAGE_RATES
  
    @{ */

        //! \brief IRS mileage rate
        //!
        //! This struct contains the mileage rates for a given type and date.
        //! Periodically, the IRS updates the mileage rates for each type.
        //! In some cases, the mileage rates have been updated mid year, 
        //! causing us to have to deal with mid-year changes.
        struct MileageRates
        {
            //! \brief Starting date for the mileage rate
            time_t start;
            //! \brief Mileage category (e.g., business, medical, personal)
            eMileageCatgories category;
            //! \brief Mileage rate in cents per mile
            double rate_per_mile;
        };
    };
};

#endif
