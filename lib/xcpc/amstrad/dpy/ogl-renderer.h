/*
 * ogl-renderer.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_OGL_RENDERER_H__
#define __XCPC_OGL_RENDERER_H__

#include <xcpc/amstrad/dpy/dpy-core.h>
#include <xcpc/amstrad/dpy/ogl-wrapper.h>

// ---------------------------------------------------------------------------
// ogl::Renderer
// ---------------------------------------------------------------------------

namespace ogl {

class Renderer final
    : public dpy::Renderer
{
public: // public interface
    Renderer();

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

    auto get_image_data() -> uint8_t*
    {
        return _state.image_data;
    }

    auto get_image_bpp() const -> int
    {
        return _state.image_bpp;
    }

    auto get_image_bpl() const -> int
    {
        return _state.image_bpl;
    }

private: // private interface
    auto create_image() -> void;

    auto delete_image() -> void;

    auto create_program() -> void;

    auto delete_program() -> void;

    auto create_texture() -> void;

    auto delete_texture() -> void;

    auto create_geometry() -> void;

    auto delete_geometry() -> void;

private: // private interface
    auto ogl_create_program() -> void;

    auto ogl_delete_program() -> void;

    auto ogl_create_texture() -> void;

    auto ogl_delete_texture() -> void;

    auto ogl_upload_texture() -> void;

    auto ogl_update_texture() -> void;

    auto ogl_create_vertex_array() -> void;

    auto ogl_delete_vertex_array() -> void;

    auto ogl_bind_vertex_array() -> void;

    auto ogl_unbind_vertex_array() -> void;

    auto ogl_render_vertex_array() -> void;

    auto ogl_create_vertex_buffer() -> void;

    auto ogl_delete_vertex_buffer() -> void;

    auto ogl_bind_vertex_buffer() -> void;

    auto ogl_unbind_vertex_buffer() -> void;

    auto ogl_upload_geometry() -> void;

private: // private types
    struct Parameters
    {
        bool  crt_emulation  = false;
        bool  dirty_program  = false;
        bool  dirty_uniforms = false;
        float u_hsampling    = 0.75f;
        float u_vsampling    = 0.25f;
        float u_curvature    = 0.10f;
        float u_corner       = 0.15f;
        float u_dotline      = 0.30f;
        float u_dotmask      = 0.10f;
        float u_vignetting   = 1.00f;
        float u_brightness   = 1.30f;
    };

private: // private data
    Parameters   _parameters;
    Program      _program;
    Texture      _texture; 
    VertexArray  _vao;
    VertexBuffer _vbo;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_OGL_RENDERER_H__ */
