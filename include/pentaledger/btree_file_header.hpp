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
#include "record_pointer.hpp"
#include <cstdint>

namespace pentaledger
{

#define NODE 512
#define MAX_KEY_LENGTH 80
#define ADR sizeof(RPTR)

    struct BTreeNode
    {
        int nonleaf;
        RPTR parent_node;
        RPTR left_sibling;
        RPTR right_sibling;
        int key_count;
        RPTR key0;
        char keyspace[NODE - ((sizeof(int) * 2) * (ADR * 4))];
        char spil[MAX_KEY_LENGTH];
    };

    struct BTreeHeader
    {
        RPTR root_node;
        int key_length;
        int max_key_per_node;
        RPTR raised_node;
        int locked;
        RPTR leftmost_node;
        RPTR rightmost_node;
    };

} // namespace pentaledger