//! \brief Hours of Service (HOS) duty status
//! \file HOS_types.hpp
//!
//! \copyright
//!
//! This file is part of the PentaLedger distribution (https://github.com/xxxx or http://xxx.github.io).
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
#ifndef _HOS_TYPES_HPP_
#define _HOS_TYPES_HPP_

namespace pentaledger
{
    namespace transportation
    {
        //! \brief Hours of Service (HOS) Duty Status
        //!
        //! “Hours of service” refers to the maximum amount of time drivers are permitted
        //! to be on duty including driving time, and specifies number and length of
        //! rest periods, to help ensure that drivers stay awake and alert. In general,
        //! all carriers and drivers operating commercial motor vehicles (CMVs) must
        //! comply with HOS regulations found in 49 CFR 395.
        //! There are four possible duty status.
        enum HOS_DUTY_STATUS
        {
            //! “Off-duty” means the driver has been relieved of all responsibilities for
            //! the vehicle and its cargo or passengers and the driver is free to pursue
            //! activities of his/her own choosing.
            HOS_OFF_DUTY,

            //! \brief On-duty status
            //! "On-duty” time is all time a driver spends performing work or being
            //! ready to work, until being relieved by the carrier of all responsibility.
            //! “On-duty” time also includes any compensated work performed by
            //! the driver for a carrier or non-motor carrier entity.
            HOS_ON_DUTY,

            //! Driving” time is all time spent at the driving controls of a commercial
            //! motor vehicle in operation
            HOS_DRIVING,

            //! “Sleeper Berth” is all time spent resting in a sleeper berth as defined
            //! in 49 CFR Section 393.76
            HOS_SLEEPER_BERTH,

        };
    }
}

#endif