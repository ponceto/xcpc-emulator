/*
 * ogl-renderer.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "ogl-renderer.h"

// ---------------------------------------------------------------------------
// <anonymous>::BasicTraits
// ---------------------------------------------------------------------------

namespace {

struct BasicTraits
{
    using Shader   = ogl::Shader;
    using Program  = ogl::Program;
    using Texture  = ogl::Texture;
    using Renderer = ogl::Renderer;
};

}

// ---------------------------------------------------------------------------
// <anonymous>::RendererTraits
// ---------------------------------------------------------------------------

namespace {

struct RendererTraits final
    : public BasicTraits
{
    static inline auto get_vertex_shader_src(bool crt_emulation) -> const char*
    {
        if(crt_emulation != false) {
            return vertex_shader_crt_src;
        }
        return vertex_shader_std_src;
    }

    static inline auto get_fragment_shader_src(bool crt_emulation) -> const char*
    {
        if(crt_emulation != false) {
            return fragment_shader_crt_src;
        }
        return fragment_shader_std_src;
    }

    static const char* const vertex_shader_std_src;
    static const char* const vertex_shader_crt_src;
    static const char* const fragment_shader_std_src;
    static const char* const fragment_shader_crt_src;
};

const char* const RendererTraits::vertex_shader_std_src = R"(\
#version 330 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;
out vec2 v_texcoord;

void main()
{
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_texcoord = a_texcoord;
}
)";

const char* const RendererTraits::vertex_shader_crt_src = R"(\
#version 330 core
layout(location = 0) in vec2 a_position;
layout(location = 1) in vec2 a_texcoord;
out vec2 v_texcoord;

void main()
{
    gl_Position = vec4(a_position, 0.0, 1.0);
    v_texcoord = a_texcoord;
}
)";

const char* const RendererTraits::fragment_shader_std_src = R"(\
#version 330 core
in  vec2 v_texcoord;
out vec4 v_fragment;
uniform sampler2D u_texture;
uniform vec2 u_texture_size;
uniform vec2 u_visible_position;
uniform vec2 u_visible_size;

void main()
{
    vec2 visible_tc;
    visible_tc.x = (u_visible_position.x + v_texcoord.x * u_visible_size.x) / u_texture_size.x;
    visible_tc.y = (u_visible_position.y + v_texcoord.y * u_visible_size.y) / u_texture_size.y;
    v_fragment = texture(u_texture, visible_tc);
}
)";

const char* const RendererTraits::fragment_shader_crt_src = R"(\
#version 330 core
in  vec2 v_texcoord;
out vec4 v_fragment;
uniform sampler2D u_texture;
uniform vec2 u_texture_size;
uniform vec2 u_visible_position;
uniform vec2 u_visible_size;
uniform float u_curvature;
uniform float u_corner;
uniform float u_dotline;
uniform float u_dotmask;
uniform float u_vignetting;
uniform float u_brightness;

/* barrel distortion for CRT screen */
vec2 crt_curvature(vec2 uv)
{
    vec2 cc = uv - 0.5;
    float r2 = dot(cc, cc);
    return uv + cc * u_curvature * r2;
}

/* corner distortion for CRT screen */
float crt_corner(vec2 uv)
{
    vec2 s = (0.0 + smoothstep(vec2(0.0), vec2(0.0 + u_corner), uv))
           * (1.0 - smoothstep(vec2(1.0 - u_corner), vec2(1.0), uv))
           ;
    return s.x * s.y;
}

void main()
{
    /* apply CRT screen curvature */
    vec2 uv = crt_curvature(v_texcoord);

    /* apply CRT screen corners */
    float corner = crt_corner(uv);
    if(corner < 0.001) {
        v_fragment = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    /* compute visible texture coordinates */
    vec2 visible_tc;
    visible_tc.x = (u_visible_position.x + uv.x * u_visible_size.x) / u_texture_size.x;
    visible_tc.y = (u_visible_position.y + uv.y * u_visible_size.y) / u_texture_size.y;

    /* sample texture with subtle horizontal phosphor spread */
    float dx = 0.5 / u_texture_size.x;
    vec4 color = texture(u_texture, visible_tc) * 0.50
               + texture(u_texture, visible_tc + vec2(dx, 0.0)) * 0.25
               + texture(u_texture, visible_tc - vec2(dx, 0.0)) * 0.25
               ;

    /* apply CRT scanlines */
    float line_pos = mod(floor(gl_FragCoord.y), 2.0);
    color.rgb *= mix(1.0, u_dotline, line_pos);

    /* apply CRT phosphor mask */
    float mask_pos = mod(floor(gl_FragCoord.x), 3.0);
    if(mask_pos < 0.5)      color.rgb *= vec3(1.0, u_dotmask, u_dotmask);
    else if(mask_pos < 1.5) color.rgb *= vec3(u_dotmask, 1.0, u_dotmask);
    else                    color.rgb *= vec3(u_dotmask, u_dotmask, 1.0);

    /* apply CRT vignetting */
    vec2 vig = uv * (1.0 - uv);
    float vignetting = clamp(pow(vig.x * vig.y * 15.0, 0.25), 0.0, 1.0);
    color.rgb *= mix(1.0, vignetting, u_vignetting);

    /* apply CRT brightness */
    color.rgb *= u_brightness;

    v_fragment = vec4(clamp(color.rgb, 0.0, 1.0), color.a);
}
)";

}

// ---------------------------------------------------------------------------
// ogl::Renderer
// ---------------------------------------------------------------------------

namespace ogl {

Renderer::Renderer()
    : dpy::Renderer()
    , _parameters()
    , _program()
    , _texture()
    , _vao()
    , _vbo()
{
}

Renderer::~Renderer()
{
    unrealize();
}

auto Renderer::realize() -> void
{
    create_image();
    create_texture();
    create_program();
    create_geometry();
}

auto Renderer::unrealize() -> void
{
    delete_geometry();
    delete_program();
    delete_texture();
    delete_image();
}

auto Renderer::resize(int width, int height) -> void
{
    _state.viewport_w = width;
    _state.viewport_h = height;
}

auto Renderer::expose(int x, int y, int width, int height) -> void
{
    auto gl_clear = [&]() -> void
    {
        ::glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        ::glClear(GL_COLOR_BUFFER_BIT);
    };

    auto gl_flush = [&]() -> void
    {
        ::glFlush();
    };

    auto gl_viewport = [&]() -> void
    {
        const float visible_aspect  = static_cast<float>(_state.visible_w) / static_cast<float>(_state.visible_h);
        const float viewport_aspect = static_cast<float>(_state.viewport_w) / static_cast<float>(_state.viewport_h);
        GLint   vp_x = 0;
        GLint   vp_y = 0;
        GLsizei vp_w = _state.viewport_w;
        GLsizei vp_h = _state.viewport_h;
        if(viewport_aspect > visible_aspect) {
            vp_w = static_cast<GLsizei>(static_cast<float>(_state.viewport_h) * visible_aspect + 0.5f);
            vp_x = (_state.viewport_w - vp_w) / 2;
        }
        else {
            vp_h = static_cast<GLsizei>(static_cast<float>(_state.viewport_w) / visible_aspect + 0.5f);
            vp_y = (_state.viewport_h - vp_h) / 2;
        }
        ::glViewport(vp_x, vp_y, vp_w, vp_h);
    };

    auto gl_set_u_texture = [&]() -> void
    {
        const GLint u_texture = _program.get_uniform_location("u_texture");
        const GLint v_texture = 0;
        if(u_texture >= 0) {
            _program.set_uniform_1i(u_texture, v_texture);
        }
    };

    auto gl_set_u_texture_size = [&]() -> void
    {
        const GLint   u_texture_size   = _program.get_uniform_location("u_texture_size");
        const GLfloat v_texture_size_w = _state.image_width;
        const GLfloat v_texture_size_h = _state.image_height;
        if(u_texture_size >= 0) {
            _program.set_uniform_2f(u_texture_size, v_texture_size_w, v_texture_size_h);
        }
    };

    auto gl_set_u_visible_position = [&]() -> void
    {
        const GLint   u_visible_position   = _program.get_uniform_location("u_visible_position");
        const GLfloat u_visible_position_x = _state.visible_x;
        const GLfloat u_visible_position_y = _state.visible_y;
        if(u_visible_position >= 0) {
            _program.set_uniform_2f(u_visible_position, u_visible_position_x, u_visible_position_y);
        }
    };

    auto gl_set_u_visible_size = [&]() -> void
    {
        const GLint   u_visible_size   = _program.get_uniform_location("u_visible_size");
        const GLfloat u_visible_size_w = _state.visible_w;
        const GLfloat u_visible_size_h = _state.visible_h;
        if(u_visible_size >= 0) {
            _program.set_uniform_2f(u_visible_size, u_visible_size_w, u_visible_size_h);
        }
    };

    auto gl_set_u_curvature = [&]() -> void
    {
        const GLint   u_curvature = _program.get_uniform_location("u_curvature");
        const GLfloat v_curvature = _parameters.u_curvature;
        if(u_curvature >= 0) {
            _program.set_uniform_1f(u_curvature, v_curvature);
        }
    };

    auto gl_set_u_corner = [&]() -> void
    {
        const GLint   u_corner = _program.get_uniform_location("u_corner");
        const GLfloat v_corner = _parameters.u_corner;
        if(u_corner >= 0) {
            _program.set_uniform_1f(u_corner, v_corner);
        }
    };

    auto gl_set_u_dotline = [&]() -> void
    {
        const GLint   u_dotline = _program.get_uniform_location("u_dotline");
        const GLfloat v_dotline = _parameters.u_dotline;
        if(u_dotline >= 0) {
            _program.set_uniform_1f(u_dotline, v_dotline);
        }
    };

    auto gl_set_u_dotmask = [&]() -> void
    {
        const GLint   u_dotmask = _program.get_uniform_location("u_dotmask");
        const GLfloat v_dotmask = _parameters.u_dotmask;
        if(u_dotmask >= 0) {
            _program.set_uniform_1f(u_dotmask, v_dotmask);
        }
    };

    auto gl_set_u_vignetting = [&]() -> void
    {
        const GLint   u_vignetting = _program.get_uniform_location("u_vignetting");
        const GLfloat v_vignetting = _parameters.u_vignetting;
        if(u_vignetting >= 0) {
            _program.set_uniform_1f(u_vignetting, v_vignetting);
        }
    };

    auto gl_set_u_brightness = [&]() -> void
    {
        const GLint   u_brightness = _program.get_uniform_location("u_brightness");
        const GLfloat v_brightness = _parameters.u_brightness;
        if(u_brightness >= 0) {
            _program.set_uniform_1f(u_brightness, v_brightness);
        }
    };

    auto gl_use_program = [&]() -> void
    {
        if(_parameters.dirty_program != false) {
            _parameters.dirty_program = false;
            if(_program != false) {
                delete_program();
                create_program();
            }
        }
        _program.use_program();
    };

    auto gl_set_parameters = [&]() -> void
    {
        if(_parameters.dirty_uniforms != false) {
            _parameters.dirty_uniforms = false;
            if(_program != false) {
                gl_set_u_texture();
                gl_set_u_texture_size();
                gl_set_u_visible_position();
                gl_set_u_visible_size();
                gl_set_u_curvature();
                gl_set_u_corner();
                gl_set_u_dotline();
                gl_set_u_dotmask();
                gl_set_u_vignetting();
                gl_set_u_brightness();
            }
        }
    };

    auto gl_unuse_program = [&]() -> void
    {
        _program.unuse_program();
    };

    auto gl_draw_geometry = [&]() -> void
    {
        _texture.bind_texture(GL_TEXTURE_2D);
        _texture.active_texture(GL_TEXTURE0);
        _vao.bind_vertex_array();
        _vao.draw_vertex_array(GL_TRIANGLE_STRIP, 0, 4);
        _vao.unbind_vertex_array();
        _texture.unbind_texture(GL_TEXTURE_2D);
    };

    auto gl_expose = [&]() -> void
    {
        if((_state.viewport_w > 0) && (_state.viewport_h > 0)) {
            gl_clear();
            gl_viewport();
            gl_use_program();
            gl_set_parameters();
            gl_draw_geometry();
            gl_unuse_program();
            gl_flush();
        }
    };

    return gl_expose();
}

auto Renderer::render() -> void
{
    ogl_update_texture();
}

auto Renderer::set_visible_area(int x, int y, int w, int h) -> void
{
    const int old_visible_x = _state.visible_x;
    const int old_visible_y = _state.visible_y;
    const int old_visible_w = _state.visible_w;
    const int old_visible_h = _state.visible_h;
    const int new_visible_x = _state.visible_x = x;
    const int new_visible_y = _state.visible_y = y;
    const int new_visible_w = _state.visible_w = w;
    const int new_visible_h = _state.visible_h = h;

    if((new_visible_x != old_visible_x)
    || (new_visible_y != old_visible_y)
    || (new_visible_w != old_visible_w)
    || (new_visible_h != old_visible_h)) {
        _parameters.dirty_program  |= false;
        _parameters.dirty_uniforms |= true;
    }
}

auto Renderer::alloc_color(uint16_t r, uint16_t g, uint16_t b) -> uint32_t
{
    const uint8_t color[sizeof(uint32_t)] = {
        static_cast<uint8_t>((r >> 8) & 0xff),
        static_cast<uint8_t>((g >> 8) & 0xff),
        static_cast<uint8_t>((b >> 8) & 0xff),
        0xff
    };
    return *reinterpret_cast<const uint32_t*>(color);
}

auto Renderer::dealloc_color(uint32_t color) -> uint32_t
{
    return color = 0;
}

auto Renderer::set_parameterb(const std::string& parameter, bool value) -> void
{
    if((parameter == "video.crt_emulation") || (parameter == "video.ogl.crt_emulation")) {
        const bool old_crt_emulation = _parameters.crt_emulation;
        const bool new_crt_emulation = _parameters.crt_emulation = value;
        if(new_crt_emulation != old_crt_emulation) {
            _parameters.dirty_program  |= true;
            _parameters.dirty_uniforms |= true;
        }
    }
}

auto Renderer::set_parameteri(const std::string& parameter, int value) -> void
{
}

auto Renderer::set_parameterf(const std::string& parameter, float value) -> void
{
    if(parameter == "video.ogl.u_curvature") {
        const float old_curvature = _parameters.u_curvature;
        const float new_curvature = _parameters.u_curvature = value;
        if(new_curvature != old_curvature) {
            _parameters.dirty_program  |= false;
            _parameters.dirty_uniforms |= true;
        }
        return;
    }
    if(parameter == "video.ogl.u_corner") {
        const float old_corner = _parameters.u_corner;
        const float new_corner = _parameters.u_corner = value;
        if(new_corner != old_corner) {
            _parameters.dirty_program  |= false;
            _parameters.dirty_uniforms |= true;
        }
        return;
    }
    if(parameter == "video.ogl.u_dotline") {
        const float old_dotline = _parameters.u_dotline;
        const float new_dotline = _parameters.u_dotline = value;
        if(new_dotline != old_dotline) {
            _parameters.dirty_program  |= false;
            _parameters.dirty_uniforms |= true;
        }
        return;
    }
    if(parameter == "video.ogl.u_dotmask") {
        const float old_dotmask = _parameters.u_dotmask;
        const float new_dotmask = _parameters.u_dotmask = value;
        if(new_dotmask != old_dotmask) {
            _parameters.dirty_program  |= false;
            _parameters.dirty_uniforms |= true;
        }
        return;
    }
    if(parameter == "video.ogl.u_vignetting") {
        const float old_vignetting = _parameters.u_vignetting;
        const float new_vignetting = _parameters.u_vignetting = value;
        if(new_vignetting != old_vignetting) {
            _parameters.dirty_program  |= false;
            _parameters.dirty_uniforms |= true;
        }
        return;
    }
    if(parameter == "video.ogl.u_brightness") {
        const float old_brightness = _parameters.u_brightness;
        const float new_brightness = _parameters.u_brightness = value;
        if(new_brightness != old_brightness) {
            _parameters.dirty_program  |= false;
            _parameters.dirty_uniforms |= true;
        }
        return;
    }
}

auto Renderer::create_image() -> void
{
    if(_state.image_data == nullptr) {
        _state.image_width  = DISPLAY_WIDTH;
        _state.image_height = DISPLAY_HEIGHT;
        _state.image_bpp    = 32;
        _state.image_bpl    = _state.image_width * 4;
        _state.image_data   = new uint8_t[_state.image_height * _state.image_bpl];
    }
}

auto Renderer::delete_image() -> void
{
    if(_state.image_data != nullptr) {
        _state.image_width  = 0;
        _state.image_height = 0;
        _state.image_bpp    = 0;
        _state.image_bpl    = 0;
        _state.image_data   = (delete[] _state.image_data, nullptr);
    }
}

auto Renderer::create_program() -> void
{
    ogl_create_program();
    _parameters.dirty_program  = false;
    _parameters.dirty_uniforms = true;
}

auto Renderer::delete_program() -> void
{
    ogl_delete_program();
}

auto Renderer::create_texture() -> void
{
    ogl_create_texture();
    ogl_upload_texture();
}

auto Renderer::delete_texture() -> void
{
    ogl_delete_texture();
}

auto Renderer::create_geometry() -> void
{
    ogl_create_vertex_array();
    ogl_create_vertex_buffer();
    ogl_bind_vertex_array();
    ogl_bind_vertex_buffer();
    ogl_upload_geometry();
    ogl_unbind_vertex_buffer();
    ogl_unbind_vertex_array();
}

auto Renderer::delete_geometry() -> void
{
    ogl_delete_vertex_array();
    ogl_delete_vertex_buffer();
}

auto Renderer::ogl_create_program() -> void
{
    Shader vert_shader;
    Shader frag_shader;

    auto ogl_create_and_compile_vert_shader = [&]() -> void
    {
        vert_shader.create_shader(GL_VERTEX_SHADER);
        vert_shader.compile_shader(RendererTraits::get_vertex_shader_src(_parameters.crt_emulation));
    };

    auto ogl_create_and_compile_frag_shader = [&]() -> void
    {
        frag_shader.create_shader(GL_FRAGMENT_SHADER);
        frag_shader.compile_shader(RendererTraits::get_fragment_shader_src(_parameters.crt_emulation));
    };

    auto ogl_create_and_link_program = [&]() -> void
    {
        _program.create_program();
        _program.link_program(vert_shader, frag_shader);
    };

    ogl_create_and_compile_vert_shader();
    ogl_create_and_compile_frag_shader();
    ogl_create_and_link_program();
}

auto Renderer::ogl_delete_program() -> void
{
    _program.delete_program();
}

auto Renderer::ogl_create_texture() -> void
{
    _texture.create_texture();
}

auto Renderer::ogl_delete_texture() -> void
{
    _texture.delete_texture();
}

auto Renderer::ogl_upload_texture() -> void
{
    constexpr GLint filter = GL_LINEAR;

    if(_texture != false) {
        _texture.bind_texture(GL_TEXTURE_2D);
        _texture.tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
        _texture.tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
        _texture.tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        _texture.tex_parameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        _texture.tex_image_2d(GL_TEXTURE_2D, 0, GL_RGBA, _state.image_width, _state.image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, _state.image_data);
        _texture.unbind_texture(GL_TEXTURE_2D);
    }
}

auto Renderer::ogl_update_texture() -> void
{
    if(_texture != false) {
        _texture.bind_texture(GL_TEXTURE_2D);
        _texture.tex_sub_image_2d(GL_TEXTURE_2D, 0, 0, 0, _state.image_width, _state.image_height, GL_RGBA, GL_UNSIGNED_BYTE, _state.image_data);
        _texture.active_texture(GL_TEXTURE0);
        _texture.unbind_texture(GL_TEXTURE_2D);
    }
}

auto Renderer::ogl_create_vertex_array() -> void
{
    _vao.create_vertex_array();
}

auto Renderer::ogl_delete_vertex_array() -> void
{
    _vao.delete_vertex_array();
}

auto Renderer::ogl_bind_vertex_array() -> void
{
    _vao.bind_vertex_array();
}

auto Renderer::ogl_unbind_vertex_array() -> void
{
    _vao.unbind_vertex_array();
}

auto Renderer::ogl_render_vertex_array() -> void
{
    _vao.bind_vertex_array();
    _vao.draw_vertex_array(GL_TRIANGLE_STRIP, 0, 4);
    _vao.unbind_vertex_array();
}

auto Renderer::ogl_create_vertex_buffer() -> void
{
    _vbo.create_vertex_buffer();
}

auto Renderer::ogl_delete_vertex_buffer() -> void
{
    _vbo.delete_vertex_buffer();
}

auto Renderer::ogl_bind_vertex_buffer() -> void
{
    _vbo.bind_vertex_buffer();
}

auto Renderer::ogl_unbind_vertex_buffer() -> void
{
    _vbo.unbind_vertex_buffer();
}

auto Renderer::ogl_upload_geometry() -> void
{
    static const Vertex2d geometry[] = {
        { { -1.0f, -1.0f }, { 0.0f, 1.0f } }, /* bottom-left  */
        { { +1.0f, -1.0f }, { 1.0f, 1.0f } }, /* bottom-right */
        { { -1.0f, +1.0f }, { 0.0f, 0.0f } }, /* top-left     */
        { { +1.0f, +1.0f }, { 1.0f, 0.0f } }, /* top-right    */
    };

    /* load vertex data */ {
        _vbo.upload_data(sizeof(geometry), geometry, GL_STATIC_DRAW);
    }
    /* position attribute (location = 0) */ {
        _vbo.vertex_attrib_pointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2d), reinterpret_cast<void*>(offsetof(Vertex2d, position)));
        _vao.enable_vertex_attrib_array(0);
    }
    /* texcoord attribute (location = 1) */ {
        _vbo.vertex_attrib_pointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2d), reinterpret_cast<void*>(offsetof(Vertex2d, texcoord)));
        _vao.enable_vertex_attrib_array(1);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
