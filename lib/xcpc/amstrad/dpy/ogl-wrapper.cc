/*
 * ogl-wrapper.cc - Copyright (c) 2001-2026 - Olivier Poncet
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
#include "ogl-wrapper.h"

// ---------------------------------------------------------------------------
// ogl::Handle
// ---------------------------------------------------------------------------

namespace ogl {

Handle::Handle(GLuint handle)
    : _handle(handle)
{
}

}

// ---------------------------------------------------------------------------
// ogl::Shader
// ---------------------------------------------------------------------------

namespace ogl {

Shader::Shader(GLuint handle)
    : Handle(handle)
{
}

Shader::~Shader()
{
    delete_shader();
}

auto Shader::create_shader(GLenum type) -> void
{
    if(_handle == 0u) {
        _handle = ::glCreateShader(type);
    }
}

auto Shader::delete_shader() -> void
{
    if(_handle != 0u) {
        _handle = (::glDeleteShader(_handle), 0u);
    }
}

auto Shader::compile_shader(const char* source) -> void
{
    if(_handle != 0u) {
        ::glShaderSource(_handle, 1, &source, nullptr);
        ::glCompileShader(_handle);
    }
    if(_handle != 0u) {
        GLint status = GL_FALSE;
        ::glGetShaderiv(_handle, GL_COMPILE_STATUS, &status);
        if(status == GL_FALSE) {
            char what[512];
            ::glGetShaderInfoLog(_handle, sizeof(what), nullptr, what);
            throw std::runtime_error(what);
        }
    }
}

}

// ---------------------------------------------------------------------------
// ogl::Program
// ---------------------------------------------------------------------------

namespace ogl {

Program::Program(GLuint handle)
    : Handle(handle)
{
}

Program::~Program()
{
    delete_program();
}

auto Program::create_program() -> void
{
    if(_handle == 0u) {
        _handle = ::glCreateProgram();
    }
}

auto Program::delete_program() -> void
{
    if(_handle != 0u) {
        _handle = (::glDeleteProgram(_handle), 0u);
    }
}

auto Program::link_program(const Shader& vshader, const Shader& fshader) -> void
{
    if(_handle != 0u) {
        ::glAttachShader(_handle, vshader.get());
        ::glAttachShader(_handle, fshader.get());
        ::glLinkProgram(_handle);
    }
    if(_handle != 0u) {
        GLint status = GL_FALSE;
        ::glGetProgramiv(_handle, GL_LINK_STATUS, &status);
        if(status == GL_FALSE) {
            char what[512];
            ::glGetProgramInfoLog(_handle, sizeof(what), nullptr, what);
            throw std::runtime_error(what);
        }
    }
}

auto Program::use_program() -> void
{
    if(_handle != 0u) {
        ::glUseProgram(_handle);
    }
}

auto Program::unuse_program() -> void
{
    if(_handle != 0u) {
        ::glUseProgram(0u);
    }
}

auto Program::get_uniform_location(const char* name) -> GLint
{
    if(_handle != 0u) {
        return ::glGetUniformLocation(_handle, name);
    }
    return -1;
}

auto Program::set_uniform_1f(GLint location, GLfloat v0) -> void
{
    if(_handle != 0u) {
        ::glUniform1f(location, v0);
    }
}

auto Program::set_uniform_2f(GLint location, GLfloat v0, GLfloat v1) -> void
{
    if(_handle != 0u) {
        ::glUniform2f(location, v0, v1);
    }
}

auto Program::set_uniform_3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2) -> void
{
    if(_handle != 0u) {
        ::glUniform3f(location, v0, v1, v2);
    }
}

auto Program::set_uniform_4f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3) -> void
{
    if(_handle != 0u) {
        ::glUniform4f(location, v0, v1, v2, v3);
    }
}

auto Program::set_uniform_1i(GLint location, GLint v0) -> void
{
    if(_handle != 0u) {
        ::glUniform1i(location, v0);
    }
}

auto Program::set_uniform_2i(GLint location, GLint v0, GLint v1) -> void
{
    if(_handle != 0u) {
        ::glUniform2i(location, v0, v1);
    }
}

auto Program::set_uniform_3i(GLint location, GLint v0, GLint v1, GLint v2) -> void
{
    if(_handle != 0u) {
        ::glUniform3i(location, v0, v1, v2);
    }
}

auto Program::set_uniform_4i(GLint location, GLint v0, GLint v1, GLint v2, GLint v3) -> void
{
    if(_handle != 0u) {
        ::glUniform4i(location, v0, v1, v2, v3);
    }
}

}

// ---------------------------------------------------------------------------
// ogl::Texture
// ---------------------------------------------------------------------------

namespace ogl {

Texture::Texture(GLuint handle)
    : Handle(handle)
{
}

Texture::~Texture()
{
    delete_texture();
}

auto Texture::create_texture() -> void
{
    if(_handle == 0u) {
        ::glGenTextures(1, &_handle);
    }
}

auto Texture::delete_texture() -> void
{
    if(_handle != 0u) {
        _handle = (::glDeleteTextures(1, &_handle), 0u);
    }
}

auto Texture::bind_texture(GLenum target) -> void
{
    if(_handle != 0u) {
        ::glBindTexture(target, _handle);
    }
}

auto Texture::unbind_texture(GLenum target) -> void
{
    if(_handle != 0u) {
        ::glBindTexture(target, 0u);
    }
}

auto Texture::active_texture(GLenum texture) -> void
{
    if(_handle != 0u) {
        ::glActiveTexture(texture);
    }
}

auto Texture::tex_parameterf(GLenum target, GLenum pname, GLfloat param) -> void
{
    if(_handle != 0u) {
        ::glTexParameterf(target, pname, param);
    }
}

auto Texture::tex_parameteri(GLenum target, GLenum pname, GLint param) -> void
{
    if(_handle != 0u) {
        ::glTexParameteri(target, pname, param);
    }
}

auto Texture::tex_image_2d(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* data) -> void
{
    if(_handle != 0u) {
        ::glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
    }
}

auto Texture::tex_sub_image_2d(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* data) -> void
{
    if(_handle != 0u) {
        ::glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, data);
    }
}

}

// ---------------------------------------------------------------------------
// ogl::VertexArray
// ---------------------------------------------------------------------------

namespace ogl {

VertexArray::VertexArray(GLuint handle)
    : Handle(handle)
{
}

VertexArray::~VertexArray()
{
    delete_vertex_array();
}

auto VertexArray::create_vertex_array() -> void
{
    if(_handle == 0u) {
        ::glGenVertexArrays(1, &_handle);
    }
}

auto VertexArray::delete_vertex_array() -> void
{
    if(_handle != 0u) {
        _handle = (::glDeleteVertexArrays(1, &_handle), 0u);
    }
}

auto VertexArray::bind_vertex_array() -> void
{
    if(_handle != 0u) {
        ::glBindVertexArray(_handle);
    }
}

auto VertexArray::unbind_vertex_array() -> void
{
    if(_handle != 0u) {
        ::glBindVertexArray(0u);
    }
}

auto VertexArray::draw_vertex_array(GLenum mode, GLint first, GLsizei count) -> void
{
    if(_handle != 0u) {
        ::glDrawArrays(mode, first, count);
    }
}

auto VertexArray::enable_vertex_attrib_array(GLuint index) -> void
{
    if(_handle != 0u) {
        ::glEnableVertexAttribArray(index);
    }
}

auto VertexArray::disable_vertex_attrib_array(GLuint index) -> void
{
    if(_handle != 0u) {
        ::glDisableVertexAttribArray(index);
    }
}

}

// ---------------------------------------------------------------------------
// ogl::VertexBuffer
// ---------------------------------------------------------------------------

namespace ogl {

VertexBuffer::VertexBuffer(GLuint handle)
    : Handle(handle)
{
}

VertexBuffer::~VertexBuffer()
{
    delete_vertex_buffer();
}

auto VertexBuffer::create_vertex_buffer() -> void
{
    if(_handle == 0u) {
        ::glGenBuffers(1, &_handle);
    }
}

auto VertexBuffer::delete_vertex_buffer() -> void
{
    if(_handle != 0u) {
        _handle = (::glDeleteBuffers(1, &_handle), 0u);
    }
}

auto VertexBuffer::bind_vertex_buffer() -> void
{
    if(_handle != 0u) {
        ::glBindBuffer(GL_ARRAY_BUFFER, _handle);
    }
}

auto VertexBuffer::unbind_vertex_buffer() -> void
{
    if(_handle != 0u) {
        ::glBindBuffer(GL_ARRAY_BUFFER, 0u);
    }
}

auto VertexBuffer::upload_data(GLsizeiptr size, const void* data, GLenum usage) -> void
{
    if(_handle != 0u) {
        ::glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    }
}

auto VertexBuffer::vertex_attrib_pointer(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) -> void
{
    if(_handle != 0u) {
        ::glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }
}

}

// ---------------------------------------------------------------------------
// End-Of-File
// ---------------------------------------------------------------------------
