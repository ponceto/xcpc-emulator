/*
 * gtk3-bin.cc - Copyright (c) 2001-2024 - Olivier Poncet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <climits>
#include <cassert>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include "gtk3-bin.h"

// ---------------------------------------------------------------------------
// gtk3::BinTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct BinTraits
    : BasicTraits
{
    static GtkWidget* create_bin()
    {
        return nullptr;
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::BinTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Bin
// ---------------------------------------------------------------------------

namespace gtk3 {

Bin::Bin()
    : Bin(traits::create_bin())
{
}

Bin::Bin(GtkWidget* instance)
    : Container(instance)
{
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
