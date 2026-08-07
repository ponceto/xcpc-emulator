/*
 * x11-renderer.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_X11_RENDERER_H__
#define __XCPC_X11_RENDERER_H__

#include <xcpc/amstrad/dpy/dpy-core.h>
#include <xcpc/amstrad/dpy/x11-wrapper.h>

// ---------------------------------------------------------------------------
// x11::Renderer
// ---------------------------------------------------------------------------

namespace x11 {

class Renderer final
    : public dpy::Renderer
{
public: // public interface
    Renderer(Display* display, Window window, bool try_xshm);

    Renderer(Renderer&&) = delete;

    Renderer(const Renderer&) = delete;

    Renderer& operator=(Renderer&&) = delete;

    Renderer& operator=(const Renderer&) = delete;

    virtual ~Renderer();

    virtual auto realize() -> void override final;

    virtual auto unrealize() -> void override final;

    virtual auto resize(int width, int height) -> void override final;

    virtual auto expose(int x, int y, int width, int height) -> void override final;

    virtual auto render() -> void override final;

    virtual auto set_visible_area(int x, int y, int w, int h) -> void override final;

    virtual auto alloc_color(uint16_t r, uint16_t g, uint16_t b) -> uint32_t override final;

    virtual auto dealloc_color(uint32_t color) -> uint32_t override final;

    virtual auto set_parameterb(const std::string& parameter, bool value) -> void override final;

    virtual auto set_parameteri(const std::string& parameter, int value) -> void override final;

    virtual auto set_parameterf(const std::string& parameter, float value) -> void override final;

    auto get_image_data() const -> uint8_t*
    {
        if(_image != nullptr) {
            return reinterpret_cast<uint8_t*>(_image->data);
        }
        return nullptr;
    }

    auto get_image_bpp() const -> int
    {
        if(_image != nullptr) {
            return _image->bits_per_pixel;
        }
        return 0;
    }

    auto get_image_bpl() const -> int
    {
        if(_image != nullptr) {
            return _image->bytes_per_line;
        }
        return 0;
    }

private: // private types
    struct Parameters
    {
        bool crt_emulation = false;
        bool try_xshm      = false;
        bool has_xshm      = false;
        bool use_xshm      = false;
    };

private: // private data
    Parameters _parameters;
    Display*   _display;
    Screen*    _screen;
    Visual*    _visual;
    Window     _window;
    Colormap   _colormap;
    GC         _gc;
    XImage*    _image;
    int        _depth;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_X11_RENDERER_H__ */
