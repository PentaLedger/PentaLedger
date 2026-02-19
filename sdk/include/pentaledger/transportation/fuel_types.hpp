//! \brief Fuel types
//! \file fuel_types.hpp
//!
//! \copyright
//!
//! This file is part of the PentaLedger distribution.
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
#ifndef _FUEL_TYPES_HPP_
#define _FUEL_TYPES_HPP_

namespace pentaledger
{
    namespace transportation
    {

        //! \brief Fuel type
        //! Fuel type needs to be recorded during fuel stops for
        //! both cost analysis and IFTA tax reporting.
        enum FUEL_TYPE
        {
            //! \brief Invalid fuel type
            FUEL_INVALID = -1,

            //! \brief Gasoline
            //! Gas stations in the United States generally offer three octane 
            //! grades: regular (usually 87 octane), mid-grade 
            //! (usually 89 octane), and premium (usually 91 or 93 gas). 
            FUEL_GASOLINE,
            
            //! \brief Diesel
            //! Diesel or bio-diesel
            FUEL_DIESEL,
            
            //! \brief Ethenol
            //! E85 or E10
            FUEL_ETHENOL,

            //! \brief Compressed Natural Gas (CNG)
            FUEL_CNG,

        };

    }; // transportation namespace
};     // ioi namespace

#endif
