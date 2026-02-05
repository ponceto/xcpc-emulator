/*
 * dpy-core.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "dpy-core.h"
#include "ogl-renderer.h"
#include "x11-renderer.h"

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
// <anonymous>::ColorEntry
// ---------------------------------------------------------------------------

namespace {

struct ColorEntry
{
    const char* label;
    uint32_t    pixel;
    uint16_t    r;
    uint16_t    g;
    uint16_t    b;
    uint16_t    l;
};

const ColorEntry color_table[] = {
    { "white"                       , 0, 0x8000, 0x8000, 0x8000, 0x8000 },
    { "white (not official)"        , 0, 0x8000, 0x8000, 0x8000, 0x8000 },
    { "sea green"                   , 0, 0x0000, 0xffff, 0x8000, 0xa4dd },
    { "pastel yellow"               , 0, 0xffff, 0xffff, 0x8000, 0xf168 },
    { "blue"                        , 0, 0x0000, 0x0000, 0x8000, 0x0e97 },
    { "purple"                      , 0, 0xffff, 0x0000, 0x8000, 0x5b22 },
    { "cyan"                        , 0, 0x0000, 0x8000, 0x8000, 0x59ba },
    { "pink"                        , 0, 0xffff, 0x8000, 0x8000, 0xa645 },
    { "purple (not official)"       , 0, 0xffff, 0x0000, 0x8000, 0x5b22 },
    { "pastel yellow (not official)", 0, 0xffff, 0xffff, 0x8000, 0xf168 },
    { "bright yellow"               , 0, 0xffff, 0xffff, 0x0000, 0xe2d0 },
    { "bright white"                , 0, 0xffff, 0xffff, 0xffff, 0xffff },
    { "bright red"                  , 0, 0xffff, 0x0000, 0x0000, 0x4c8b },
    { "bright magenta"              , 0, 0xffff, 0x0000, 0xffff, 0x69ba },
    { "orange"                      , 0, 0xffff, 0x8000, 0x0000, 0x97ad },
    { "pastel magenta"              , 0, 0xffff, 0x8000, 0xffff, 0xb4dc },
    { "blue (not official)"         , 0, 0x0000, 0x0000, 0x8000, 0x0e97 },
    { "sea green (not official)"    , 0, 0x0000, 0xffff, 0x8000, 0xa4dd },
    { "bright green"                , 0, 0x0000, 0xffff, 0x0000, 0x9645 },
    { "bright cyan"                 , 0, 0x0000, 0xffff, 0xffff, 0xb374 },
    { "black"                       , 0, 0x0000, 0x0000, 0x0000, 0x0000 },
    { "bright blue"                 , 0, 0x0000, 0x0000, 0xffff, 0x1d2f },
    { "green"                       , 0, 0x0000, 0x8000, 0x0000, 0x4b23 },
    { "sky blue"                    , 0, 0x0000, 0x8000, 0xffff, 0x6852 },
    { "magenta"                     , 0, 0x8000, 0x0000, 0x8000, 0x34dd },
    { "pastel green"                , 0, 0x8000, 0xffff, 0x8000, 0xcb22 },
    { "lime"                        , 0, 0x8000, 0xffff, 0x0000, 0xbc8b },
    { "pastel cyan"                 , 0, 0x8000, 0xffff, 0xffff, 0xd9ba },
    { "red"                         , 0, 0x8000, 0x0000, 0x0000, 0x2645 },
    { "mauve"                       , 0, 0x8000, 0x0000, 0xffff, 0x4374 },
    { "yellow"                      , 0, 0x8000, 0x8000, 0x0000, 0x7168 },
    { "pastel blue"                 , 0, 0x8000, 0x8000, 0xffff, 0x8e97 }
};

}

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using MonitorType = dpy::MonitorType;
    using RefreshRate = dpy::RefreshRate;
    using State       = dpy::State;
    using Instance    = dpy::Instance;
    using Interface   = dpy::Interface;
    using Renderer    = dpy::Renderer;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::StateTraits
// ---------------------------------------------------------------------------

namespace {

struct StateTraits final
    : public BasicTraits
{
    static inline auto construct(State& state, const MonitorType monitor_type, const RefreshRate refresh_rate) -> void
    {
        state.monitor_type = monitor_type;
        state.refresh_rate = refresh_rate;
    }

    static inline auto destruct(State& state) -> void
    {
        state.monitor_type = XCPC_MONITOR_TYPE_DEFAULT;
        state.refresh_rate = XCPC_REFRESH_RATE_DEFAULT;
    }

    static inline auto reset(State& state) -> void
    {
    }

    static inline auto clock(State& state) -> void
    {
    }

    static inline auto set_monitor_type(State& state, Renderer* renderer, const MonitorType monitor_type) -> void
    {
        dealloc_palette1(state, renderer);
        dealloc_palette0(state, renderer);
        state.monitor_type = monitor_type;
        recompute_visible_area(state, renderer);
        alloc_palette0(state, renderer);
        alloc_palette1(state, renderer);
    }

    static inline auto set_refresh_rate(State& state, Renderer* renderer, const RefreshRate refresh_rate) -> void
    {
        dealloc_palette1(state, renderer);
        dealloc_palette0(state, renderer);
        state.refresh_rate = refresh_rate;
        recompute_visible_area(state, renderer);
        alloc_palette0(state, renderer);
        alloc_palette1(state, renderer);
    }

    static inline auto realize(State& state, Renderer* renderer) -> void
    {
        if(renderer != nullptr) {
            renderer->realize();
        }
        recompute_visible_area(state, renderer);
        alloc_palette0(state, renderer);
        alloc_palette1(state, renderer);
    }

    static inline auto unrealize(State& state, Renderer* renderer) -> void
    {
        dealloc_palette1(state, renderer);
        dealloc_palette0(state, renderer);
        recompute_visible_area(state, renderer);
        if(renderer != nullptr) {
            renderer->unrealize();
        }
    }

    static inline auto resize(State& state, Renderer* renderer, int width, int height) -> void
    {
        state.viewport_w = width;
        state.viewport_h = height;
        if(renderer != nullptr) {
            renderer->resize(state.viewport_w, state.viewport_h);
        }
    }

    static inline auto recompute_visible_area(State& state, Renderer* renderer) -> void
    {
        int visible_x = 0;
        int visible_y = 0;
        int visible_w = 0;
        int visible_h = 0;

        switch(state.refresh_rate) {
            case XCPC_REFRESH_RATE_50HZ:
                visible_x = MONITOR_50HZ_VISIBLE_X;
                visible_y = MONITOR_50HZ_VISIBLE_Y;
                visible_w = MONITOR_50HZ_VISIBLE_WIDTH;
                visible_h = MONITOR_50HZ_VISIBLE_HEIGHT;
                break;
            case XCPC_REFRESH_RATE_60HZ:
                visible_x = MONITOR_60HZ_VISIBLE_X;
                visible_y = MONITOR_60HZ_VISIBLE_Y;
                visible_w = MONITOR_60HZ_VISIBLE_WIDTH;
                visible_h = MONITOR_60HZ_VISIBLE_HEIGHT;
                break;
            default:
                visible_x = MONITOR_50HZ_VISIBLE_X;
                visible_y = MONITOR_50HZ_VISIBLE_Y;
                visible_w = MONITOR_50HZ_VISIBLE_WIDTH;
                visible_h = MONITOR_50HZ_VISIBLE_HEIGHT;
                break;
        }
        if(renderer != nullptr) {
            renderer->set_visible_area(visible_x, visible_y, visible_w, visible_h);
        }
        if(renderer != nullptr) {
            renderer->resize(state.viewport_w, state.viewport_h);
        }
    }

    static inline auto alloc_palette0(State& state, Renderer* renderer) -> void
    {
        int index = 0;
        for(auto& pixel : state.palette0) {
            ColorEntry color = {};
            setup_color(state, color, index, 0);
            if(renderer != nullptr) {
                pixel = renderer->alloc_color(color.r, color.g, color.b);
            }
            ++index;
        }
    }

    static inline auto alloc_palette1(State& state, Renderer* renderer) -> void
    {
        int index = 0;
        for(auto& pixel : state.palette1) {
            ColorEntry color = {};
            setup_color(state, color, index, 1);
            if(renderer != nullptr) {
                pixel = renderer->alloc_color(color.r, color.g, color.b);
            }
            ++index;
        }
    }

    static inline auto dealloc_palette0(State& state, Renderer* renderer) -> void
    {
        for(auto& pixel : state.palette0) {
            if(renderer != nullptr) {
                pixel = renderer->dealloc_color(pixel);
            }
        }
    }

    static inline auto dealloc_palette1(State& state, Renderer* renderer) -> void
    {
        for(auto& pixel : state.palette1) {
            if(renderer != nullptr) {
                pixel = renderer->dealloc_color(pixel);
            }
        }
    }

    static inline auto setup_color(State& state, ColorEntry& color, const int index, const int palette) -> void
    {
        constexpr int min_index = 0;
        constexpr int max_index = 31;

        if((index >= min_index) && (index <= max_index)) {
            const auto& entry = color_table[index];
            color = entry;
            switch(state.monitor_type) {
                case XCPC_MONITOR_TYPE_COLOR:
                case XCPC_MONITOR_TYPE_CTM640:
                case XCPC_MONITOR_TYPE_CTM644:
                case XCPC_MONITOR_TYPE_CM14:
                    color.r = (entry.r | 0);
                    color.g = (entry.g | 0);
                    color.b = (entry.b | 0);
                    break;
                case XCPC_MONITOR_TYPE_GREEN:
                case XCPC_MONITOR_TYPE_GT64:
                case XCPC_MONITOR_TYPE_GT65:
                    color.r = (entry.l & 0);
                    color.g = (entry.l | 0);
                    color.b = (entry.l & 0);
                    break;
                case XCPC_MONITOR_TYPE_GRAY:
                case XCPC_MONITOR_TYPE_MM12:
                    color.r = (entry.l | 0);
                    color.g = (entry.l | 0);
                    color.b = (entry.l | 0);
                    break;
                default:
                    color.r = (entry.r | 0);
                    color.g = (entry.g | 0);
                    color.b = (entry.b | 0);
                    break;
            }
            if(palette == 1) {
                color.r = ((static_cast<uint32_t>(color.r) * 5) / 8);
                color.g = ((static_cast<uint32_t>(color.g) * 5) / 8);
                color.b = ((static_cast<uint32_t>(color.b) * 5) / 8);
            }
        }
    }
};

}

// ---------------------------------------------------------------------------
// dpy::Instance
// ---------------------------------------------------------------------------

namespace dpy {

Instance::Instance(const MonitorType monitor_type, const RefreshRate refresh_rate, Interface& interface)
    : _interface(interface)
    , _state()
    , _renderer()
{
    StateTraits::construct(_state, monitor_type, refresh_rate);

    reset();
}

Instance::~Instance()
{
    unrealize();

    StateTraits::destruct(_state);
}

auto Instance::reset() -> void
{
    StateTraits::reset(_state);
}

auto Instance::clock() -> void
{
    StateTraits::clock(_state);
}

auto Instance::set_monitor_type(const MonitorType monitor_type) -> void
{
    StateTraits::set_monitor_type(_state, _renderer.get(), monitor_type);
}

auto Instance::set_refresh_rate(const RefreshRate refresh_rate) -> void
{
    StateTraits::set_refresh_rate(_state, _renderer.get(), refresh_rate);
}

auto Instance::realize(const RendererType renderer_type, Display* display, Window window, bool xshm) -> void
{
    if(bool(_renderer) == false) {
        switch(renderer_type) {
            case XCPC_RENDERER_TYPE_XIMAGE:
                _renderer = std::make_unique<x11::Renderer>(display, window, xshm);
                break;
            case XCPC_RENDERER_TYPE_OPENGL:
                _renderer = std::make_unique<ogl::Renderer>();
                break;
            default:
                break;
        }
        StateTraits::realize(_state, _renderer.get());
    }
}

auto Instance::unrealize() -> void
{
    if(bool(_renderer) != false) {
        StateTraits::unrealize(_state, _renderer.get());
        _renderer.reset();
    }
}

auto Instance::resize(int width, int height) -> void
{
    StateTraits::resize(_state, _renderer.get(), width, height);
}

auto Instance::expose(const XExposeEvent& event) -> void
{
    if(bool(_renderer) != false) {
        _renderer->expose(event.x, event.y, event.width, event.height);
    }
}

auto Instance::render() -> void
{
    if(bool(_renderer) != false) {
        _renderer->render();
    }
}

auto Instance::set_parameterb(const std::string& parameter, bool value) -> void
{
    if(bool(_renderer) != false) {
        _renderer->set_parameterb(parameter, value);
    }
}

auto Instance::set_parameteri(const std::string& parameter, int value) -> void
{
    if(bool(_renderer) != false) {
        _renderer->set_parameteri(parameter, value);
    }
}

auto Instance::set_parameterf(const std::string& parameter, float value) -> void
{
    if(bool(_renderer) != false) {
        _renderer->set_parameterf(parameter, value);
    }
}

auto Instance::get_image_width() const -> int
{
    if(bool(_renderer) != false) {
        return (*_renderer)->image_width;
    }
    return 0;
}

auto Instance::get_image_height() const -> int
{
    if(bool(_renderer) != false) {
        return (*_renderer)->image_height;
    }
    return 0;
}

auto Instance::get_image_bpp() const -> int
{
    if(bool(_renderer) != false) {
        return (*_renderer)->image_bpp;
    }
    return 0;
}

auto Instance::get_image_bpl() const -> int
{
    if(bool(_renderer) != false) {
        return (*_renderer)->image_bpl;
    }
    return 0;
}

auto Instance::get_image_data() -> uint8_t*
{
    if(bool(_renderer) != false) {
        return (*_renderer)->image_data;
    }
    return nullptr;
}

}

// ---------------------------------------------------------------------------
// dpy::Renderer
// ---------------------------------------------------------------------------

namespace dpy {

Renderer::Renderer()
    : _state()
{
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
