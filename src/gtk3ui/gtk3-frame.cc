/*
 * gtk3-frame.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "gtk3-frame.h"

// ---------------------------------------------------------------------------
// gtk3::FrameTraits
// ---------------------------------------------------------------------------

namespace gtk3 {

struct FrameTraits
    : BasicTraits
{
    static auto create_frame(const std::string& string = "label") -> GtkWidget*
    {
        return ::gtk_frame_new(string.c_str());
    }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::traits
// ---------------------------------------------------------------------------

namespace {

using traits = gtk3::FrameTraits;

}

// ---------------------------------------------------------------------------
// gtk3::Frame
// ---------------------------------------------------------------------------

namespace gtk3 {

Frame::Frame()
    : Frame(traits::create_frame())
{
}

Frame::Frame(GtkWidget* instance)
    : Bin(instance)
{
}

auto Frame::create_frame(const std::string& string) -> void
{
    if(_instance == nullptr) {
        _instance = traits::create_frame(string);
        traits::register_widget_instance(_instance);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
