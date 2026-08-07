/*
 * dpy-core.h - Copyright (c) 2001-2026 - Olivier Poncet
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
class  Renderer;

}

// ---------------------------------------------------------------------------
// type aliases
// ---------------------------------------------------------------------------

namespace dpy {

using MonitorType  = xcpc::MonitorType;
using RefreshRate  = xcpc::RefreshRate;
using RendererType = xcpc::RendererType;
using RendererPtr  = std::unique_ptr<Renderer>;

}

// ---------------------------------------------------------------------------
// dpy::State
// ---------------------------------------------------------------------------

namespace dpy {

struct State
{
    MonitorType monitor_type;
    RefreshRate refresh_rate;
    int         viewport_w;
    int         viewport_h;
    uint32_t    palette0[32];
    uint32_t    palette1[32];
};

}

// ---------------------------------------------------------------------------
// dpy::Instance
// ---------------------------------------------------------------------------

namespace dpy {

class Instance
{
public: // public interface
    Instance(Interface& interface);

    Instance(Instance&&) = delete;

    Instance(const Instance&) = delete;

    Instance& operator=(Instance&&) = delete;

    Instance& operator=(const Instance&) = delete;

    virtual ~Instance();

    auto reset() -> void;

    auto clock() -> void;

    auto set_monitor_type(const MonitorType monitor_type) -> void;

    auto set_refresh_rate(const RefreshRate refresh_rate) -> void;

    auto realize(const RendererType renderer_type, Display* display, Window window, bool xshm) -> void;

    auto unrealize() -> void;

    auto resize(int width, int height) -> void;

    auto expose(const XExposeEvent& event) -> void;

    auto render() -> void;

    auto set_parameterb(const std::string& parameter, bool value) -> void;

    auto set_parameteri(const std::string& parameter, int value) -> void;

    auto set_parameterf(const std::string& parameter, float value) -> void;

    auto get_image_width() const -> int;

    auto get_image_height() const -> int;

    auto get_image_bpp() const -> int;

    auto get_image_bpl() const -> int;

    auto get_image_data() -> uint8_t*;

    auto operator->() -> State*
    {
        return &_state;
    }

public: // public types
    static constexpr int DISPLAY_WIDTH  = 1024;
    static constexpr int DISPLAY_HEIGHT =  768;

protected: // protected data
    Interface&  _interface;
    State       _state;
    RendererPtr _renderer;
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
// dpy::Renderer
// ---------------------------------------------------------------------------

namespace dpy {

class Renderer
{
public: // public interface
    Renderer();

    Renderer(Renderer&&) = delete;

    Renderer(const Renderer&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    Renderer& operator=(const Renderer&) = delete;

    virtual ~Renderer() = default;

    virtual auto realize() -> void = 0;

    virtual auto unrealize() -> void = 0;

    virtual auto resize(int width, int height) -> void = 0;

    virtual auto expose(int x, int y, int width, int height) -> void = 0;

    virtual auto render() -> void = 0;

    virtual auto set_visible_area(int x, int y, int w, int h) -> void = 0;

    virtual auto alloc_color(uint16_t r, uint16_t g, uint16_t b) -> uint32_t = 0;

    virtual auto dealloc_color(uint32_t color) -> uint32_t = 0;

    virtual auto set_parameterb(const std::string& parameter, bool value) -> void = 0;

    virtual auto set_parameteri(const std::string& parameter, int value) -> void = 0;

    virtual auto set_parameterf(const std::string& parameter, float value) -> void = 0;

public: // public interface
    static constexpr int DISPLAY_WIDTH  = Instance::DISPLAY_WIDTH;
    static constexpr int DISPLAY_HEIGHT = Instance::DISPLAY_HEIGHT;

    struct State
    {
        int      image_x;
        int      image_y;
        int      image_width;
        int      image_height;
        int      image_bpp;
        int      image_bpl;
        uint8_t* image_data;
        int      visible_x;
        int      visible_y;
        int      visible_w;
        int      visible_h;
        int      viewport_w;
        int      viewport_h;
    };

    auto operator->() -> State*
    {
        return &_state;
    }

protected: // protected data
    State _state;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_DPY_CORE_H__ */
