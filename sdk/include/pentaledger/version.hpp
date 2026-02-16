/*
 * PentaLedger
 * Copyright (C) 2025  Joe Turner <joe@agavemountain.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#pragma once

// ------------------------------------------------------------
// PentaLedger SDK Version Information
// ------------------------------------------------------------
//
// Semantic Versioning: MAJOR.MINOR.PATCH
//
// MAJOR - Breaking API changes
// MINOR - Backwards-compatible feature additions
// PATCH - Bug fixes
//
// ------------------------------------------------------------

#define PENTALEDGER_VERSION_MAJOR 0
#define PENTALEDGER_VERSION_MINOR 0
#define PENTALEDGER_VERSION_PATCH 1

// Optional: ABI version (increment only when ABI breaks)
#define PENTALEDGER_VERSION_ABI 0

// ------------------------------------------------------------
// String Version
// ------------------------------------------------------------

#define PENTALEDGER_VERSION_STRING "0.0.1"

// ------------------------------------------------------------
// Packed Numeric Version
// Useful for compile-time comparison
// Format: 0xMMmmpp
// Example: 1.2.3 -> 0x010203
// ------------------------------------------------------------

#define PENTALEDGER_VERSION_HEX          \
    ((PENTALEDGER_VERSION_MAJOR << 16) | \
     (PENTALEDGER_VERSION_MINOR << 8) |  \
     PENTALEDGER_VERSION_PATCH)

// ------------------------------------------------------------
// C++ Namespace Version Access
// ------------------------------------------------------------

namespace pentaledger
{

    //! \brief Version information
    //! \details This struct provides access to the version information for the PentaLedger library.
    //! \return The version information
    struct Version
    {
        //! \brief Major version
        //! \return The major version
        static constexpr int major() noexcept { return PENTALEDGER_VERSION_MAJOR; }

        //! \brief Minor version
        //! \return The minor version
        static constexpr int minor() noexcept { return PENTALEDGER_VERSION_MINOR; }
        
        //! \brief Patch version
        //! \return The patch version
        static constexpr int patch() noexcept { return PENTALEDGER_VERSION_PATCH; }

        //! \brief ABI version
        //! \return The ABI version
        static constexpr int abi() noexcept { return PENTALEDGER_VERSION_ABI; }

        //! \brief Packed version
        //! \return The packed version
        //! \details The packed version is the packed version of the version information.
        static constexpr int packed() noexcept { return PENTALEDGER_VERSION_HEX; }

        //! \brief String version
        //! \return The string version
        //! \details The string version is the string version of the version information.
        //! \return The string version
        static constexpr const char *string() noexcept
        {
            return PENTALEDGER_VERSION_STRING;
        }
    };

} // namespace pentaledger
