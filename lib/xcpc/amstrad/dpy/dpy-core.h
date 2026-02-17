/*
 * dpy-core.h - Copyright (c) 2001-2025 - Olivier Poncet
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
#ifndef __XCPC_DPY_CORE_H__
#define __XCPC_DPY_CORE_H__

// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

namespace dpy {

struct State;
class  Instance;
class  Interface;

}

// ---------------------------------------------------------------------------
// dpy::Type
// ---------------------------------------------------------------------------

namespace dpy {

enum Type
{
    TYPE_INVALID = -1,
    TYPE_DEFAULT =  0,
    TYPE_COLOR   =  1,
    TYPE_GREEN   =  2,
    TYPE_GRAY    =  3,
    TYPE_CTM640  =  4,
    TYPE_CTM644  =  5,
    TYPE_GT64    =  6,
    TYPE_GT65    =  7,
    TYPE_CM14    =  8,
    TYPE_MM12    =  9,
};

}

// ---------------------------------------------------------------------------
// dpy::State
// ---------------------------------------------------------------------------

namespace dpy {

struct State
{
    uint8_t  type;
    uint8_t  rate;
    Display* display;
    Screen*  screen;
    Visual*  visual;
    XImage*  image;
    GC       gc;
    Window   window;
    Colormap colormap;
    int      depth;
    int      image_x;
    int      image_y;
    int      total_w;
    int      total_h;
    int      visible_x;
    int      visible_y;
    int      visible_w;
    int      visible_h;
    bool     try_xshm;
    bool     has_xshm;
    bool     use_xshm;
    XColor   palette0[32];
    XColor   palette1[32];
};

}

// ---------------------------------------------------------------------------
// dpy::Instance
// ---------------------------------------------------------------------------

namespace dpy {

class Instance
{
public: // public interface
    Instance(const Type type, Interface& interface);

    Instance(const Instance&) = delete;

    Instance& operator=(const Instance&) = delete;

    virtual ~Instance();

    auto reset() -> void;

    auto clock() -> void;

    auto set_type(const Type type) -> void;

    auto set_rate(const uint8_t rate) -> void;

    auto realize(Display* display, Window window, bool try_xshm) -> void;

    auto unrealize() -> void;

    auto expose(const XExposeEvent& event) -> void;

    auto resize(const XConfigureEvent& event) -> void;

    auto put_image() -> void;

    auto operator->() -> State*
    {
        return &_state;
    }

protected: // protected data
    Interface& _interface;
    State      _state;
};

}

// ---------------------------------------------------------------------------
// dpy::Interface
// ---------------------------------------------------------------------------

namespace dpy {

class Interface
{
public: // public interface
    Interface() = default;

    Interface(const Interface&) = default;

    Interface& operator=(const Interface&) = default;

    virtual ~Interface() = default;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_DPY_CORE_H__ */
