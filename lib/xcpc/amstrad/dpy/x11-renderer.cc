/*
 * x11-renderer.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "x11-renderer.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Renderer = x11::Renderer;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::RendererTraits
// ---------------------------------------------------------------------------

namespace {

struct RendererTraits final
    : public BasicTraits
{
};

}

// ---------------------------------------------------------------------------
// <anonymous>::MonitorArea
// ---------------------------------------------------------------------------

namespace {

struct MonitorArea
{
    int x1;
    int y1;
    int x2;
    int y2;
};

}

// ---------------------------------------------------------------------------
// x11::Renderer
// ---------------------------------------------------------------------------

namespace x11 {

Renderer::Renderer(Display* display, Window window, bool try_xshm)
    : dpy::Renderer()
    , _parameters()
    , _display(display)
    , _screen(nullptr)
    , _visual(nullptr)
    , _window(window)
    , _colormap(None)
    , _gc(nullptr)
    , _image(nullptr)
    , _depth(0)
{
    _parameters.crt_emulation = false;
    _parameters.try_xshm      = try_xshm;
    _parameters.has_xshm      = false;
    _parameters.use_xshm      = false;
}

Renderer::~Renderer()
{
    unrealize();
}

auto Renderer::realize() -> void
{
    Display* display  = _display;
    Window   window   = _window;
    bool     try_xshm = _parameters.try_xshm;

    auto do_realize_renderer = [&]() -> void
    {
        XWindowAttributes attributes;
        Status status = XGetWindowAttributes(display, window, &attributes);
        if(status != 0) {
            _parameters.try_xshm = try_xshm;
            _parameters.has_xshm = false;
            _parameters.use_xshm = false;
            _display             = display;
            _screen              = attributes.screen;
            _visual              = attributes.visual;
            _window              = window;
            _colormap            = attributes.colormap;
            _gc                  = DefaultGCOfScreen(attributes.screen);
            _depth               = attributes.depth;
        }
        if(status != 0) {
            _state.viewport_w = attributes.width;
            _state.viewport_h = attributes.height;
        }
    };

    auto do_realize_std_image = [&]() -> void
    {
        if(_image == nullptr) {
            _image = XcpcCreateStdImage ( _display
                                        , _visual
                                        , _depth
                                        , ZPixmap
                                        , DISPLAY_WIDTH
                                        , DISPLAY_HEIGHT );
        }
    };

    auto do_realize_shm_image = [&]() -> void
    {
        if(_image != nullptr) {
            return;
        }
        if(_parameters.try_xshm != false) {
            _parameters.has_xshm = (XcpcQueryShmExtension(_display) != False ? true : false);
        }
        if(_parameters.has_xshm != false) {
            _image = XcpcCreateShmImage ( _display
                                        , _visual
                                        , _depth
                                        , ZPixmap
                                        , DISPLAY_WIDTH
                                        , DISPLAY_HEIGHT );
        }
        if(_image != nullptr) {
            _parameters.use_xshm = XcpcAttachShmImage(_display, _image);
            if(_parameters.use_xshm == false) {
                _image = (XcpcDestroyShmImage(_image), nullptr);
            }
        }
    };

    auto do_realize_state = [&]() -> void
    {
        if(_image != nullptr) {
            _state.image_x      = 0;
            _state.image_y      = 0;
            _state.image_width  = _image->width;
            _state.image_height = _image->height;
            _state.image_bpp    = _image->bits_per_pixel;
            _state.image_bpl    = _image->bytes_per_line;
            _state.image_data   = reinterpret_cast<uint8_t*>(_image->data);
        }
        if(_state.viewport_w >= _state.visible_w) {
            _state.image_x = +((_state.viewport_w - _state.visible_w) / 2);
        }
        else {
            _state.image_x = -((_state.visible_w - _state.viewport_w) / 2);
        }
        if(_state.viewport_h >= _state.visible_h) {
            _state.image_y = +((_state.viewport_h - _state.visible_h) / 2);
        }
        else {
            _state.image_y = -((_state.visible_h - _state.viewport_h) / 2);
        }
    };

    auto do_clear_area = [&]() -> void
    {
        static_cast<void>(XClearArea(_display, _window, 0, 0, 32767, 32767, True));
        static_cast<void>(XFlush(_display));
    };

    auto do_realize = [&]() -> void
    {
        do_realize_renderer();
        do_realize_shm_image();
        do_realize_std_image();
        do_realize_state();
        do_clear_area();
    };

    return do_realize();
}
 
auto Renderer::unrealize() -> void
{
    Display* display  = _display;
    Window   window   = _window;
    bool     try_xshm = _parameters.try_xshm;

    auto do_unrealize_renderer = [&]() -> void
    {
        _parameters.try_xshm = try_xshm;
        _parameters.has_xshm = false;
        _parameters.use_xshm = false;
        _display             = display;
        _screen              = nullptr;
        _visual              = nullptr;
        _window              = window;
        _colormap            = None;
        _gc                  = nullptr;
        _depth               = 0;
    };

    auto do_unrealize_state = [&]() -> void
    {
        _state.image_x      = 0;
        _state.image_y      = 0;
        _state.image_width  = 0;
        _state.image_height = 0;
        _state.image_bpp    = 0;
        _state.image_bpl    = 0;
        _state.image_data   = nullptr;
        _state.viewport_w   = 0;
        _state.viewport_h   = 0;
    };

    auto do_unrealize_std_image = [&]() -> void
    {
        if(_image != nullptr) {
            _image = (XcpcDestroyStdImage(_image), nullptr);
        }
    };

    auto do_unrealize_shm_image = [&]() -> void
    {
        if(_image == nullptr) {
            return;
        }
        if(_parameters.use_xshm != false) {
            _parameters.use_xshm = (XcpcDetachShmImage(_display, _image), false);
            _image = (XcpcDestroyShmImage(_image), nullptr);
        }
    };

    auto do_unrealize = [&]() -> void
    {
        do_unrealize_state();
        do_unrealize_shm_image();
        do_unrealize_std_image();
        do_unrealize_renderer();
    };

    return do_unrealize();
}

auto Renderer::resize(int width, int height) -> void
{
    /* update display size */ {
        _state.viewport_w = width;
        _state.viewport_h = height;
    }
    /* compute image_x */ {
        if(_state.viewport_w >= _state.visible_w) {
            _state.image_x = +((_state.viewport_w - _state.visible_w) / 2);
        }
        else {
            _state.image_x = -((_state.visible_w - _state.viewport_w) / 2);
        }
    }
    /* compute image_y */ {
        if(_state.viewport_h >= _state.visible_h) {
            _state.image_y = +((_state.viewport_h - _state.visible_h) / 2);
        }
        else {
            _state.image_y = -((_state.visible_h - _state.viewport_h) / 2);
        }
    }
}

auto Renderer::expose(int x, int y, int width, int height) -> void
{
    if((_display == nullptr) || (_window == None) || (_image == nullptr)) {
        return;
    }

    MonitorArea monitor;
    MonitorArea refresh;
    /* init monitor area */ {
        monitor.x1 = _state.image_x;
        monitor.y1 = _state.image_y;
        monitor.x2 = ((monitor.x1 + _state.visible_w) - 1);
        monitor.y2 = ((monitor.y1 + _state.visible_h) - 1);
    }
    /* init refresh area */ {
        refresh.x1 = x;
        refresh.y1 = y;
        refresh.x2 = ((refresh.x1 + width ) - 1);
        refresh.y2 = ((refresh.y1 + height) - 1);
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
        const int src_x = _state.visible_x + (refresh.x1 - _state.image_x);
        const int src_y = _state.visible_y + (refresh.y1 - _state.image_y);
        const int dst_x = refresh.x1;
        const int dst_y = refresh.y1;
        const int dst_w = ((refresh.x2 - refresh.x1) + 1);
        const int dst_h = ((refresh.y2 - refresh.y1) + 1);
        static_cast<void>(XSync(_display, False));
        static_cast<void>(XcpcPutImage ( _display
                                       , _window
                                       , _gc
                                       , _image
                                       , src_x
                                       , src_y
                                       , dst_x
                                       , dst_y
                                       , dst_w
                                       , dst_h
                                       , _parameters.use_xshm ? True : False
                                       , False ));
        static_cast<void>(XFlush(_display));
    }
}

auto Renderer::render() -> void
{
    if((_display != nullptr) && (_window != None) && (_image != nullptr)) {
        const int src_x = _state.visible_x;
        const int src_y = _state.visible_y;
        const int dst_x = _state.image_x;
        const int dst_y = _state.image_y;
        const int dst_w = _state.visible_w;
        const int dst_h = _state.visible_h;
        static_cast<void>(XSync(_display, False));
        static_cast<void>(XcpcPutImage ( _display
                                       , _window
                                       , _gc
                                       , _image
                                       , src_x
                                       , src_y
                                       , dst_x
                                       , dst_y
                                       , dst_w
                                       , dst_h
                                       , _parameters.use_xshm ? True : False
                                       , False ));
        static_cast<void>(XFlush(_display));
    }
}

auto Renderer::set_visible_area(int x, int y, int w, int h) -> void
{
    _state.visible_x = x;
    _state.visible_y = y;
    _state.visible_w = w;
    _state.visible_h = h;
}

auto Renderer::alloc_color(uint16_t r, uint16_t g, uint16_t b) -> uint32_t
{
    XColor color;
    /* initialize color */ {
        color.pixel = 0;
        color.red   = r;
        color.green = g;
        color.blue  = b;
        color.flags = (DoRed | DoGreen | DoBlue);
        color.pad   = 0;
    }
    /* alloc color */ {
        static_cast<void>(XAllocColor(_display, _colormap, &color));
    }
    return color.pixel;
}

auto Renderer::dealloc_color(uint32_t pixel) -> uint32_t
{
    XColor color;
    /* initialize color */ {
        color.pixel = pixel;
        color.red   = 0;
        color.green = 0;
        color.blue  = 0;
        color.flags = 0;
        color.pad   = 0;
    }
    /* dealloc color */ {
        static_cast<void>(XFreeColors(_display, _colormap, &color.pixel, 1, 0));
    }
    return pixel = 0;
}

auto Renderer::set_parameterb(const std::string& parameter, bool value) -> void
{
    if((parameter == "video.crt_emulation") || (parameter == "video.x11.crt_emulation")) {
        const bool old_crt_emulation = _parameters.crt_emulation;
        const bool new_crt_emulation = _parameters.crt_emulation = value;
        if(new_crt_emulation != old_crt_emulation) {
            /* do nothing */
        }
    }
}

auto Renderer::set_parameteri(const std::string& parameter, int value) -> void
{
}

auto Renderer::set_parameterf(const std::string& parameter, float value) -> void
{
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
