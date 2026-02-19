// SPDX-License-Identifier: GPL-2.0-only
//! \file fuel_efficiency.hpp
//! \author Joe Turner <joe.agavemountain.com>
//!
//! \copyright
//!
//! This file is part of the PentaLedger distribution.
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
#ifndef _FUEL_EFFICIENCY_HPP_
#define _FUEL_EFFICIENCY_HPP_

namespace pentaledger
{
    namespace transportation
    {

        //! \brief Calculate Fuel Efficiency (MPG or KPL)
        //!
        //! \param distance Distance travelled, in miles or kilometers.
        //! \param fuel_consumed  Amount of fuel consumed, in gallons or liters.
        //!
        //! \return
        //! This function will return the calculated miles per gallon (or Kilometers 
        //! per Liter). 
        //! If fuel_consumed is less than or equal to zero, the result is zero (0).
        template <class T>
        T calculate_fuel_efficiency(T distance, T fuel_consumed)
        {
            T result = 0;
            if (fuel_consumed > 0 && distance > 0)
            {
                result = distance / fuel_consumed;
            }
            else
            {
                throw std::invalid_argument("Invalid fuel consumption or distance");
            }
            return result;
        }

        //! \brief Calculate Fuel Required 
        //!
        //! \param distance Distance travelled, in miles or kilometers.
        //! \param efficiency fuel efficiency, in MPG or KPL.
        //! \return
        //! This function returns the total number of gallons (or Liters)
        //! required to cover the distance.
        template <class T>
        T calculate_fuel_required(T distance, T efficiency)
        {
            T result = 0;
            
            if (efficiency > 0 && distance > 0)
            {
                result = distance / efficiency;
            }
            else
            {
                throw std::invalid_argument("Invalid fuel efficiency or distance");
            }
            return result;
        }
    }
}

#endif