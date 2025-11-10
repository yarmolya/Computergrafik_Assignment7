#pragma once
//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include "gl.hh"
#include "glmath.hh"
#include <filesystem>

//=============================================================================

/// shader class for easy handling of the shader
class Shader
{
public:

    /// default constructor
    Shader();
    /// default destructor
    ~Shader();

    // Shader objects may not be assigned or copied, or both copies will
    // believe they own the OpenGL objects exclusively.
    Shader(const Shader&) = delete;
    Shader& operator=(const Shader&) = delete;

    /// load (from file), compile, and link vertex and fragment shader,
    /// optionially also a geometry shader
    /// \param vfile string with the adress to the vertex shader
    /// \param ffile string with the adress to the fragment shader
    /// \param gfile optional string with the adress to the geometry shader
    bool load(std::filesystem::path const&  vfile,
              std::filesystem::path const&  ffile,
              std::filesystem::path const&  gfile={});

    /// re-load shaders from files, useful for debugging
    bool reload();

    /// deletes all shader and frees GPU shader capacities
    void cleanup();

    /// enable/bind this shader program
    void use();
    /// disable/unbind this shader program
    void disable();

    /// Upload a typed value for a uniform by name
    /// \param name      string holding the uniform name
    /// \param value     the value for the uniform
    //  \param optional  whether to silently do nothing or report an error if
    //                   the uniform cannot be found.
    //                   Note: if a uniform is unused in the vertex/fragment shader
    //                   it can be optimized away and lead to invalid location errors.
    template<typename T>
    void set_uniform(const char* name, const T &value, bool optional = false);

private:
    /// loads a vertex/fragment/geometry shader from a file and compiles it
    /// \param filename the location and name of the shader
    /// \param type the type of the shader (vertex, geometry, fragment)
    GLint load_and_compile(std::filesystem::path const& filename, GLenum type);

private:
    std::filesystem::path vfile_, ffile_, gfile_;
    /// id of the linked shader program
    GLint pid_ = 0;
    /// id of the vertex shader
    GLint vid_ = 0;
    /// id of the fragment shader
    GLint fid_ = 0;
    /// id of the geometry shader
    GLint gid_ = 0;
};

inline void set_uniform_by_location(int loc, bool         val) { glUniform1i       (loc, static_cast<int>(val));          }
inline void set_uniform_by_location(int loc, float        val) { glUniform1f       (loc, val);                            }
inline void set_uniform_by_location(int loc, int          val) { glUniform1i       (loc, val);                            }
inline void set_uniform_by_location(int loc, const vec3  &val) { glUniform3f       (loc, val[0], val[1], val[2]);         }
inline void set_uniform_by_location(int loc, const vec4  &val) { glUniform4f       (loc, val[0], val[1], val[2], val[3]); }
inline void set_uniform_by_location(int loc, const mat3  &val) { glUniformMatrix3fv(loc, 1, false, val.data());           }
inline void set_uniform_by_location(int loc, const mat4  &val) { glUniformMatrix4fv(loc, 1, false, val.data());           }

template<typename T>
void Shader::set_uniform(const char* name, const T &value, bool optional) {
    if (!pid_) return;
    int location = glGetUniformLocation(pid_, name);
    if (location == -1) {
        if (!optional)
            std::cerr << "Invalid uniform location for: " << name << std::endl;
        return;
    }
    set_uniform_by_location(location, value);
}

