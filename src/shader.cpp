//=============================================================================
//
//   Exercise code for the lecture "Introduction to Computer Graphics"
//     by Prof. Mario Botsch, Bielefeld University
//
//   Copyright (C) by Computer Graphics Group, Bielefeld University
//
//=============================================================================

#include "shader.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>

class cannot_compile_shader : public std::runtime_error {
    using std::runtime_error::runtime_error;
};


//=============================================================================


Shader::Shader() :
    pid_(0), vid_(0), fid_(0), gid_(0)
{
}


//-----------------------------------------------------------------------------


Shader::~Shader()
{
    cleanup();
}


//-----------------------------------------------------------------------------


void Shader::cleanup()
{
    if (pid_) glDeleteProgram(pid_);
    if (vid_) glDeleteShader(vid_);
    if (fid_) glDeleteShader(fid_);
    if (gid_) glDeleteShader(gid_);

    pid_ = vid_ = fid_ = gid_ = 0;
}


//-----------------------------------------------------------------------------


bool Shader::load(std::filesystem::path const&  _vfile,
              std::filesystem::path const&  _ffile,
              std::filesystem::path const&  _gfile)
{
    vfile_ = _vfile;
    ffile_ = _ffile;
    gfile_ = _gfile;

    return reload();
}

bool Shader::reload()
{
    cleanup();
    glCheckError();

    // create program
    GLint new_pid = glCreateProgram();
    glCheckError();

    try {
        // vertex shader
        GLint new_vid = load_and_compile(vfile_, GL_VERTEX_SHADER);
        glCheckError();
        if (new_vid)  glAttachShader(new_pid, new_vid);
        glCheckError();

        // fragment shader
        GLint new_fid = load_and_compile(ffile_, GL_FRAGMENT_SHADER);
        if (new_fid)  glAttachShader(new_pid, new_fid);

        glCheckError();

        GLint new_gid = 0;
        // geometry shader
        if (!gfile_.empty())
        {
            new_gid = load_and_compile(gfile_, GL_GEOMETRY_SHADER);
            if (new_gid)  glAttachShader(new_pid, new_gid);
        }


        // link program
        glLinkProgram(new_pid);
        GLint status;
        glGetProgramiv(new_pid, GL_LINK_STATUS, &status);
        if (status == GL_FALSE)
        {
            GLint length;
            glGetProgramiv(new_pid, GL_INFO_LOG_LENGTH, &length);

            std::string info(length + 1, ' ');
            glGetProgramInfoLog(new_pid, length, NULL, &info[0]);
            std::cerr << "Shader: Cannot link program:\n" << info << std::endl;
            throw std::runtime_error("cannot link opengl shader");

            cleanup();

            return false;
        }
        glCheckError();
        pid_ = new_pid;
        vid_ = new_vid;
        fid_ = new_fid;
        gid_ = new_gid;
    } catch (cannot_compile_shader &) {
        return false;
    }

    return true;
}


//-----------------------------------------------------------------------------


GLint Shader::load_and_compile(std::filesystem::path const& filename, GLenum type)
{
    // read file to string
    std::ifstream  ifs(filename);
    if (!ifs)
    {
        std::cerr << "Shader: Cannot open file \""  << filename << "\"\n";
        return 0;
    }
    std::stringstream  ss;
    ss << ifs.rdbuf();
    std::string str = ss.str();
    ifs.close();

    const char* source = str.c_str();

    // create shader
    GLint id = glCreateShader(type);
    if (!id)
    {
        std::cerr << "Shader: Cannot create shader object for \""  << filename.string() << "\"\n";
        return 0;
    }


    // compile vertex shader
    glShaderSource(id, 1, &source, NULL);
    glCompileShader(id);


    // check compile status
    GLint status;
    glGetShaderiv(id, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        GLint length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

        std::string info(length + 1, ' ');
        glGetShaderInfoLog(id, length, NULL, &info[0]);

        std::cerr << "Shader: Cannot compile shader \""  << filename.string() << "\"\n" << info << std::endl;
        glDeleteShader(id);
        throw cannot_compile_shader("cannot compile opengl shader: " + info);

        return 0;
    }


    return id;
}


//-----------------------------------------------------------------------------


void Shader::use()
{
    if (pid_) glUseProgram(pid_);
}


//-----------------------------------------------------------------------------


void Shader::disable()
{
    glUseProgram(0);
}

//-----------------------------------------------------------------------------
