/*
 * dpy-device.cc - Copyright (c) 2001-2024 - Olivier Poncet
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
#include <cstdarg>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <xcpc/libxcpc-priv.h>
#include "dpy-device.h"

#define MONITOR_50HZ_TOTAL_WIDTH    1024
#define MONITOR_50HZ_TOTAL_HEIGHT    768
#define MONITOR_50HZ_VISIBLE_X       224
#define MONITOR_50HZ_VISIBLE_Y        48
#define MONITOR_50HZ_VISIBLE_WIDTH   768
#define MONITOR_50HZ_VISIBLE_HEIGHT  576

#define MONITOR_60HZ_TOTAL_WIDTH    1024
#define MONITOR_60HZ_TOTAL_HEIGHT    768
#define MONITOR_60HZ_VISIBLE_X       224
#define MONITOR_60HZ_VISIBLE_Y        44
#define MONITOR_60HZ_VISIBLE_WIDTH   768
#define MONITOR_60HZ_VISIBLE_HEIGHT  480

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct MonitorArea
{
    int x1;
    int y1;
    int x2;
    int y2;
};

struct ColorEntry
{
    const char* label;
    uint16_t    red;
    uint16_t    green;
    uint16_t    blue;
    uint16_t    luminance;
};

const ColorEntry color_table[] = {
    { "white"                       , 0x8000, 0x8000, 0x8000, 0x8000 },
    { "white (not official)"        , 0x8000, 0x8000, 0x8000, 0x8000 },
    { "sea green"                   , 0x0000, 0xffff, 0x8000, 0xa4dd },
    { "pastel yellow"               , 0xffff, 0xffff, 0x8000, 0xf168 },
    { "blue"                        , 0x0000, 0x0000, 0x8000, 0x0e97 },
    { "purple"                      , 0xffff, 0x0000, 0x8000, 0x5b22 },
    { "cyan"                        , 0x0000, 0x8000, 0x8000, 0x59ba },
    { "pink"                        , 0xffff, 0x8000, 0x8000, 0xa645 },
    { "purple (not official)"       , 0xffff, 0x0000, 0x8000, 0x5b22 },
    { "pastel yellow (not official)", 0xffff, 0xffff, 0x8000, 0xf168 },
    { "bright yellow"               , 0xffff, 0xffff, 0x0000, 0xe2d0 },
    { "bright white"                , 0xffff, 0xffff, 0xffff, 0xffff },
    { "bright red"                  , 0xffff, 0x0000, 0x0000, 0x4c8b },
    { "bright magenta"              , 0xffff, 0x0000, 0xffff, 0x69ba },
    { "orange"                      , 0xffff, 0x8000, 0x0000, 0x97ad },
    { "pastel magenta"              , 0xffff, 0x8000, 0xffff, 0xb4dc },
    { "blue (not official)"         , 0x0000, 0x0000, 0x8000, 0x0e97 },
    { "sea green (not official)"    , 0x0000, 0xffff, 0x8000, 0xa4dd },
    { "bright green"                , 0x0000, 0xffff, 0x0000, 0x9645 },
    { "bright cyan"                 , 0x0000, 0xffff, 0xffff, 0xb374 },
    { "black"                       , 0x0000, 0x0000, 0x0000, 0x0000 },
    { "bright blue"                 , 0x0000, 0x0000, 0xffff, 0x1d2f },
    { "green"                       , 0x0000, 0x8000, 0x0000, 0x4b23 },
    { "sky blue"                    , 0x0000, 0x8000, 0xffff, 0x6852 },
    { "magenta"                     , 0x8000, 0x0000, 0x8000, 0x34dd },
    { "pastel green"                , 0x8000, 0xffff, 0x8000, 0xcb22 },
    { "lime"                        , 0x8000, 0xffff, 0x0000, 0xbc8b },
    { "pastel cyan"                 , 0x8000, 0xffff, 0xffff, 0xd9ba },
    { "red"                         , 0x8000, 0x0000, 0x0000, 0x2645 },
    { "mauve"                       , 0x8000, 0x0000, 0xffff, 0x4374 },
    { "yellow"                      , 0x8000, 0x8000, 0x0000, 0x7168 },
    { "pastel blue"                 , 0x8000, 0x8000, 0xffff, 0x8e97 }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Type      = dpy::Type;
    using State     = dpy::State;
    using Device    = dpy::Device;
    using Interface = dpy::Interface;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::StateTraits
// ---------------------------------------------------------------------------

namespace {

struct StateTraits final
    : public BasicTraits
{
    static inline auto construct(State& state, const Type type) -> void
    {
        state.type = type;
    }

    static inline auto destruct(State& state) -> void
    {
        state.type = Type::TYPE_INVALID;
    }

    static inline auto reset(State& state) -> void
    {
    }

    static inline auto clock(State& state) -> void
    {
    }

    static inline auto set_type(State& state, const Type type) -> void
    {
        state.type = type;
        if((state.display != nullptr) && (state.window != None)) {
            fini_palette(state, 1);
            fini_palette(state, 0);
            init_palette(state, 0);
            init_palette(state, 1);
        }
    }

    static inline auto set_rate(State& state, const uint8_t rate) -> void
    {
        state.rate = rate;
        if((state.display != nullptr) && (state.window != None)) {
            realize(state, state.display, state.window, state.try_xshm);
        }
    }

    static inline auto realize(State& state, Display* display, Window window, bool try_xshm) -> void
    {
        unrealize(state);
        if((state.display == nullptr) && (state.window == None)) {
            init_attributes(state, display, window, try_xshm);
            init_palette(state, 0);
            init_palette(state, 1);
            init_image(state);
        }
    }

    static inline auto unrealize(State& state) -> void
    {
        if((state.display != nullptr) && (state.window != None)) {
            fini_image(state);
            fini_palette(state, 1);
            fini_palette(state, 0);
            fini_attributes(state);
        }
    }

    static inline auto expose(State& state, const XExposeEvent& event) -> void
    {
        Display* display = state.display;
        Window   window  = state.window;
        XImage*  image   = state.image;
        GC       gc      = state.gc;

        if((display == nullptr) || (window == None) || (image == nullptr)) {
            return;
        }
        MonitorArea monitor;
        MonitorArea refresh;
        /* init monitor area */ {
            monitor.x1 = state.image_x;
            monitor.y1 = state.image_y;
            monitor.x2 = ((monitor.x1 + state.visible_w) - 1);
            monitor.y2 = ((monitor.y1 + state.visible_h) - 1);
        }
        /* init refresh area */ {
            refresh.x1 = event.x;
            refresh.y1 = event.y;
            refresh.x2 = ((refresh.x1 + event.width ) - 1);
            refresh.y2 = ((refresh.y1 + event.height) - 1);
        }
        /* intersect both areas */ {
            if((refresh.x1 > monitor.x2)
            || (refresh.x2 < monitor.x1)
            || (refresh.y1 > monitor.y2)
            || (refresh.y2 < monitor.y1)) {
                return;
            }
            if(refresh.x1 < monitor.x1) {
                refresh.x1 = monitor.x1;
            }
            if(refresh.x2 > monitor.x2) {
                refresh.x2 = monitor.x2;
            }
            if(refresh.y1 < monitor.y1) {
                refresh.y1 = monitor.y1;
            }
            if(refresh.y2 > monitor.y2) {
                refresh.y2 = monitor.y2;
            }
        }
        /* put image */ {
            const int src_x = state.visible_x + (refresh.x1 - state.image_x);
            const int src_y = state.visible_y + (refresh.y1 - state.image_y);
            const int dst_x = refresh.x1;
            const int dst_y = refresh.y1;
            const int dst_w = ((refresh.x2 - refresh.x1) + 1);
            const int dst_h = ((refresh.y2 - refresh.y1) + 1);
            (void) XSync(display, False);
            (void) XcpcPutImage ( display
                                , window
                                , gc
                                , image
                                , src_x
                                , src_y
                                , dst_x
                                , dst_y
                                , dst_w
                                , dst_h
                                , state.use_xshm ? True : False
                                , False );
            (void) XFlush(display);
        }
    }

    static inline auto resize(State& state, const XConfigureEvent& event) -> void
    {
        /* compute image_x */ {
            const int event_width   = event.width;
            const int visible_width = state.visible_w;
            if(event_width >= visible_width) {
                state.image_x = +((event_width - visible_width) / 2);
            }
            else {
                state.image_x = -((visible_width - event_width) / 2);
            }
        }
        /* compute image_y */ {
            const int event_height   = event.height;
            const int visible_height = state.visible_h;
            if(event_height >= visible_height) {
                state.image_y = +((event_height - visible_height) / 2);
            }
            else {
                state.image_y = -((visible_height - event_height) / 2);
            }
        }
    }

    static inline auto put_image(State& state) -> void
    {
        Display* display = state.display;
        Window   window  = state.window;
        XImage*  image   = state.image;
        GC       gc      = state.gc;

        if((display == nullptr) || (window == None) || (image == nullptr)) {
            return;
        }
        /* put image */ {
            const int src_x = state.visible_x;
            const int src_y = state.visible_y;
            const int dst_x = state.image_x;
            const int dst_y = state.image_y;
            const int dst_w = state.visible_w;
            const int dst_h = state.visible_h;
            (void) XSync(display, False);
            (void) XcpcPutImage ( display
                                , window
                                , gc
                                , image
                                , src_x
                                , src_y
                                , dst_x
                                , dst_y
                                , dst_w
                                , dst_h
                                , state.use_xshm ? True : False
                                , False );
            (void) XFlush(display);
        }
    }

    static inline auto init_attributes(State& state, Display* display, Window window, bool try_xshm) -> void
    {
        XWindowAttributes attributes;
        Status status = XGetWindowAttributes(display, window, &attributes);
        if(status != 0) {
            state.display   = display;
            state.screen    = attributes.screen;
            state.visual    = attributes.visual;
            state.image     = nullptr;
            state.gc        = DefaultGCOfScreen(attributes.screen);
            state.window    = window;
            state.colormap  = attributes.colormap;
            state.depth     = attributes.depth;
            state.image_x   = 0;
            state.image_y   = 0;
            state.total_w   = 0;
            state.total_h   = 0;
            state.visible_x = 0;
            state.visible_y = 0;
            state.visible_w = 0;
            state.visible_h = 0;
            state.try_xshm  = try_xshm;
            state.has_xshm  = false;
            state.use_xshm  = false;
            switch(state.rate) {
                case 50:
                    state.total_w   = MONITOR_50HZ_TOTAL_WIDTH;
                    state.total_h   = MONITOR_50HZ_TOTAL_HEIGHT;
                    state.visible_x = MONITOR_50HZ_VISIBLE_X;
                    state.visible_y = MONITOR_50HZ_VISIBLE_Y;
                    state.visible_w = MONITOR_50HZ_VISIBLE_WIDTH;
                    state.visible_h = MONITOR_50HZ_VISIBLE_HEIGHT;
                    break;
                case 60:
                    state.total_w   = MONITOR_60HZ_TOTAL_WIDTH;
                    state.total_h   = MONITOR_60HZ_TOTAL_HEIGHT;
                    state.visible_x = MONITOR_60HZ_VISIBLE_X;
                    state.visible_y = MONITOR_60HZ_VISIBLE_Y;
                    state.visible_w = MONITOR_60HZ_VISIBLE_WIDTH;
                    state.visible_h = MONITOR_60HZ_VISIBLE_HEIGHT;
                    break;
                default:
                    state.total_w   = MONITOR_50HZ_TOTAL_WIDTH;
                    state.total_h   = MONITOR_50HZ_TOTAL_HEIGHT;
                    state.visible_x = MONITOR_50HZ_VISIBLE_X;
                    state.visible_y = MONITOR_50HZ_VISIBLE_Y;
                    state.visible_w = MONITOR_50HZ_VISIBLE_WIDTH;
                    state.visible_h = MONITOR_50HZ_VISIBLE_HEIGHT;
                    break;
            }
            if(state.try_xshm != false) {
                state.has_xshm = (XcpcQueryShmExtension(state.display) != False ? true : false);
            }
            /* compute image_x */ {
                const int window_width  = attributes.width;
                const int visible_width = state.visible_w;
                if(window_width >= visible_width) {
                    state.image_x = +((window_width - visible_width) / 2);
                }
                else {
                    state.image_x = -((visible_width - window_width) / 2);
                }
            }
            /* compute image_y */ {
                const int window_height  = attributes.height;
                const int visible_height = state.visible_h;
                if(window_height >= visible_height) {
                    state.image_y = +((window_height - visible_height) / 2);
                }
                else {
                    state.image_y = -((visible_height - window_height) / 2);
                }
            }
            /* resize window */ {
                (void) XClearArea(state.display, state.window, 0, 0, 32767, 32767, True);
                (void) XFlush(state.display);
            }
        }
    }

    static inline auto fini_attributes(State& state) -> void
    {
        state.display   = nullptr;
        state.screen    = nullptr;
        state.visual    = nullptr;
        state.image     = nullptr;
        state.gc        = nullptr;
        state.window    = None;
        state.colormap  = None;
        state.depth     = 0;
        state.image_x   = 0;
        state.image_y   = 0;
        state.total_w   = 0;
        state.total_h   = 0;
        state.visible_x = 0;
        state.visible_y = 0;
        state.visible_w = 0;
        state.visible_h = 0;
        state.try_xshm  = false;
        state.has_xshm  = false;
        state.use_xshm  = false;
    }

    static inline auto init_image(State& state) -> void
    {
        /* create xshm image */ {
            if(state.image == nullptr) {
                if(state.has_xshm != false) {
                    state.image = XcpcCreateShmImage ( state.display
                                                     , state.visual
                                                     , state.depth
                                                     , ZPixmap
                                                     , state.total_w
                                                     , state.total_h );
                    if(state.image != nullptr) {
                        state.use_xshm = XcpcAttachShmImage(state.display, state.image);
                        if(state.use_xshm == false) {
                            state.image = (XDestroyImage(state.image), nullptr);
                        }
                    }
                }
            }
        }
        /* create normal image */ {
            if(state.image == nullptr) {
                state.image = XcpcCreateImage ( state.display
                                              , state.visual
                                              , state.depth
                                              , ZPixmap
                                              , state.total_w
                                              , state.total_h );
            }
        }
#if 0
        /* resize window */ {
            if(state.image != nullptr) {
                (void) XResizeWindow(state.display, state.window, state.visible_w, state.visible_h);
            }
        }
#endif
    }

    static inline auto fini_image(State& state) -> void
    {
        if(state.use_xshm != false) {
            state.use_xshm = (XcpcDetachShmImage(state.display, state.image), false);
        }
        state.image = (XDestroyImage(state.image), nullptr);
    }

    static inline auto init_palette(State& state, const int palette) -> void
    {
        auto alloc_palette0 = [&]() -> void
        {
            int index = 0;
            for(auto& color : state.palette0) {
                setup_color(state, color, index, palette);
                alloc_color(state, color);
                ++index;
            }
        };

        auto alloc_palette1 = [&]() -> void
        {
            int index = 0;
            for(auto& color : state.palette1) {
                setup_color(state, color, index, palette);
                alloc_color(state, color);
                ++index;
            }
        };

        switch(palette) {
            case 0:
                alloc_palette0();
                break;
            case 1:
                alloc_palette1();
                break;
            default:
                break;
        }
    }

    static inline auto fini_palette(State& state, const int palette) -> void
    {
        auto dealloc_palette0 = [&]() -> void
        {
            for(auto& color : state.palette0) {
                dealloc_color(state, color);
            }
        };

        auto dealloc_palette1 = [&]() -> void
        {
            for(auto& color : state.palette1) {
                dealloc_color(state, color);
            }
        };

        switch(palette) {
            case 0:
                dealloc_palette0();
                break;
            case 1:
                dealloc_palette1();
                break;
            default:
                break;
        }
    }

    static inline auto setup_color(State& state, XColor& color, const int index, const int palette) -> void
    {
        constexpr int min_index = 0;
        constexpr int max_index = 31;

        if((index >= min_index) && (index <= max_index)) {
            const auto& entry = color_table[index];
            color.pixel = 0UL;
            color.flags = (DoRed | DoGreen | DoBlue);
            color.pad   = 0;
            switch(state.type) {
                case Type::TYPE_COLOR:
                case Type::TYPE_CTM640:
                case Type::TYPE_CTM644:
                case Type::TYPE_CM14:
                    color.red   = (entry.red   | 0);
                    color.green = (entry.green | 0);
                    color.blue  = (entry.blue  | 0);
                    break;
                case Type::TYPE_GREEN:
                case Type::TYPE_GT64:
                case Type::TYPE_GT65:
                    color.red   = (entry.luminance & 0);
                    color.green = (entry.luminance | 0);
                    color.blue  = (entry.luminance & 0);
                    break;
                case Type::TYPE_GRAY:
                case Type::TYPE_MM12:
                    color.red   = (entry.luminance | 0);
                    color.green = (entry.luminance | 0);
                    color.blue  = (entry.luminance | 0);
                    break;
                default:
                    color.red   = (entry.red   | 0);
                    color.green = (entry.green | 0);
                    color.blue  = (entry.blue  | 0);
                    break;
            }
            if(palette == 1) {
                color.red   = ((static_cast<uint32_t>(color.red  ) * 5) / 8);
                color.green = ((static_cast<uint32_t>(color.green) * 5) / 8);
                color.blue  = ((static_cast<uint32_t>(color.blue ) * 5) / 8);
            }
        }
    }

    static inline auto alloc_color(State& state, XColor& color) -> void
    {
        static_cast<void>(XAllocColor(state.display, state.colormap, &color));
    }

    static inline auto dealloc_color(State& state, XColor& color) -> void
    {
        static_cast<void>(XFreeColors(state.display, state.colormap, &color.pixel, 1, 0));
    }
};

}

// ---------------------------------------------------------------------------
// dpy::Device
// ---------------------------------------------------------------------------

namespace dpy {

Device::Device(const Type type, Interface& interface)
    : _interface(interface)
    , _state()
{
    StateTraits::construct(_state, type);

    reset();
}

Device::~Device()
{
    StateTraits::destruct(_state);
}

auto Device::reset() -> void
{
    StateTraits::reset(_state);
}

auto Device::clock() -> void
{
    StateTraits::clock(_state);
}

auto Device::set_type(const Type type) -> void
{
    StateTraits::set_type(_state, type);
}

auto Device::set_rate(const uint8_t rate) -> void
{
    StateTraits::set_rate(_state, rate);
}

auto Device::realize(Display* display, Window window, bool try_xshm) -> void
{
    StateTraits::realize(_state, display, window, try_xshm);
}

auto Device::unrealize() -> void
{
    StateTraits::unrealize(_state);
}

auto Device::expose(const XExposeEvent& event) -> void
{
    StateTraits::expose(_state, event);
}

auto Device::resize(const XConfigureEvent& event) -> void
{
    StateTraits::resize(_state, event);
}

auto Device::put_image() -> void
{
    StateTraits::put_image(_state);
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
