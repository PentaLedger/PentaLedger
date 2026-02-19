// SPDX-License-Identifier: GPL-2.0-only
//! \file operations_type.hpp
//! \author Joe Turner <joe.turner@agavemountain.com>
//!
//! \copyright
//! Copyright (c) 2026 Joe Turner.
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
#ifndef _OPERATIONS_TYPE_HPP_
#define _OPERATIONS_TYPE_HPP_
namespace pentaledger
{
    namespace transportation
    {

        enum eDRIVER_OPERATIONS_MODE
        {
            //! Short Haul Operations - 150 AMR
            eSHORT_HAUL_OPERATIONS,
            //! Transportation of Construction Materials - 75 AMR
            eCONSTRUCTION_MATERIALS,
            //! Agricultural Operations - 150 AMR
            eAGRICULTURAL_OPERATIONS,
            //! Motion Picture Production Site - 100 AMR
            eMOTION_PICTURE_PRODUCTIONS,
            //! OTR
            eOTR,

            //! \brief Oil field operations (Oilfield Exemption)
            //!
            //! (1) In the instance of drivers of CMVs used exclusively in the
            //! transportation of oilfield equipment, including the stringing
            //! and picking up of pip used in pipelines, and servicing of the
            //! field operations of the natural gas and oil industry, any
            //! period of 8 consecutive days may end with the beginning of any
            //! off duty period of 24 or more successive hours.
            //!
            //! (2) In the case of specially trained drivers of CMVs which are
            //! specially constructed to service oil wells, on duty time shall not
            //! include waiting time at a natural gas or oil well site; provided,
            //! that all such time shall be fully and accurately accounted for in
            //! records to be maintained by the motor carrier.  Such records shall
            //! be made availasble upon request of the FMCSA.
            eOILFIELD_OPERATIONS,
        };

    }
}
#endif