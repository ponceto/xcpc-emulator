/*
 * ogl-wrapper.h - Copyright (c) 2001-2026 - Olivier Poncet
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
#ifndef __XCPC_OGL_WRAPPER_H__
#define __XCPC_OGL_WRAPPER_H__

#include <epoxy/gl.h>

// ---------------------------------------------------------------------------
// ogl::Vertex2d
// ---------------------------------------------------------------------------

namespace ogl {

struct Vertex2d
{
    struct {
        float x;
        float y;
    } position;
    struct {
        float u;
        float v;
    } texcoord;
};

}

// ---------------------------------------------------------------------------
// ogl::Handle
// ---------------------------------------------------------------------------

namespace ogl {

class Handle
{
public: // public interface
    Handle(GLuint handle);

    Handle(Handle&&) = delete;

    Handle(const Handle&) = delete;

    Handle& operator=(Handle&&) = delete;

    Handle& operator=(const Handle&) = delete;

    virtual ~Handle() = default;

    operator bool() const
    {
        return _handle != 0u;
    }

    auto get() const -> GLuint
    {
        return _handle;
    }

    auto release() -> GLuint
    {
        const GLuint shader = _handle;

        return (_handle = 0u, shader);
    }

protected: // protected data
    GLuint _handle;
};

}

// ---------------------------------------------------------------------------
// ogl::Shader
// ---------------------------------------------------------------------------

namespace ogl {

class Shader final
    : public Handle
{
public: // public interface
    Shader(GLuint handle = 0u);

    Shader(Shader&&) = delete;

    Shader(const Shader&) = delete;

    Shader& operator=(Shader&&) = delete;

    Shader& operator=(const Shader&) = delete;

    virtual ~Shader();

    auto create_shader(GLenum type) -> void;

    auto delete_shader() -> void;

    auto compile_shader(const char* source) -> void;
};

}

// ---------------------------------------------------------------------------
// ogl::Program
// ---------------------------------------------------------------------------

namespace ogl {

class Program final
    : public Handle
{
public: // public interface
    Program(GLuint handle = 0u);

    Program(Program&&) = delete;

    Program(const Program&) = delete;

    Program& operator=(Program&&) = delete;

    Program& operator=(const Program&) = delete;

    virtual ~Program();

    auto create_program() -> void;

    auto delete_program() -> void;

    auto link_program(const Shader& vshader, const Shader& fshader) -> void;

    auto use_program() -> void;

    auto unuse_program() -> void;

    auto get_uniform_location(const char* name) -> GLint;

    auto set_uniform_1f(GLint location, GLfloat v0) -> void;

    auto set_uniform_2f(GLint location, GLfloat v0, GLfloat v1) -> void;

    auto set_uniform_3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) -> void;

    auto set_uniform_4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) -> void;

    auto set_uniform_1i(GLint location, GLint v0) -> void;

    auto set_uniform_2i(GLint location, GLint v0, GLint v1) -> void;

    auto set_uniform_3i(GLint location, GLint v0, GLint v1, GLint v2) -> void;

    auto set_uniform_4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) -> void;
};

}

// ---------------------------------------------------------------------------
// ogl::Texture
// ---------------------------------------------------------------------------

namespace ogl {

class Texture final
    : public Handle
{
public: // public interface
    Texture(GLuint handle = 0u);

    Texture(Texture&&) = delete;

    Texture(const Texture&) = delete;

    Texture& operator=(Texture&&) = delete;

    Texture& operator=(const Texture&) = delete;

    virtual ~Texture();

    auto create_texture() -> void;

    auto delete_texture() -> void;

    auto bind_texture(GLenum target) -> void;

    auto unbind_texture(GLenum target) -> void;

    auto active_texture(GLenum texture) -> void;

    auto tex_parameterf(GLenum target, GLenum pname, GLfloat param) -> void;

    auto tex_parameteri(GLenum target, GLenum pname, GLint param) -> void;

    auto tex_image_2d(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data) -> void;

    auto tex_sub_image_2d(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data) -> void;
};

}

// ---------------------------------------------------------------------------
// ogl::VertexArray
// ---------------------------------------------------------------------------

namespace ogl {

class VertexArray final
    : public Handle
{
public: // public interface
    VertexArray(GLuint handle = 0u);

    VertexArray(VertexArray&&) = delete;

    VertexArray(const VertexArray&) = delete;

    VertexArray& operator=(VertexArray&&) = delete;

    VertexArray& operator=(const VertexArray&) = delete;

    virtual ~VertexArray();

    auto create_vertex_array() -> void;

    auto delete_vertex_array() -> void;

    auto bind_vertex_array() -> void;

    auto unbind_vertex_array() -> void;

    auto draw_vertex_array(GLenum mode, GLint first, GLsizei count) -> void;

    auto enable_vertex_attrib_array(GLuint index) -> void;

    auto disable_vertex_attrib_array(GLuint index) -> void;
};

}

// ---------------------------------------------------------------------------
// ogl::VertexBuffer
// ---------------------------------------------------------------------------

namespace ogl {

class VertexBuffer final
    : public Handle
{
public: // public interface
    VertexBuffer(GLuint handle = 0u);

    VertexBuffer(VertexBuffer&&) = delete;

    VertexBuffer(const VertexBuffer&) = delete;

    VertexBuffer& operator=(VertexBuffer&&) = delete;

    VertexBuffer& operator=(const VertexBuffer&) = delete;

    virtual ~VertexBuffer();

    auto create_vertex_buffer() -> void;

    auto delete_vertex_buffer() -> void;

    auto bind_vertex_buffer() -> void;

    auto unbind_vertex_buffer() -> void;

    auto upload_data(GLsizeiptr size, const void* data, GLenum usage) -> void;

    auto vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) -> void;
};

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------

#endif /* __XCPC_OGL_WRAPPER_H__ */
