#pragma once
//=============================================================================
//
//   Exercise code for "Introduction to Computer Graphics"
//     by Julian Panetta, EPFL
//
//=============================================================================

#include "gl.hh"
#include <array>

struct Quad {
    void initialize();
    void draw();
    ~Quad();

private:
    enum { VTX_BUFFER = 0, NORMAL_BUFFER = 1, INDEX_BUFFER = 2 };
    GLuint vao = 0;
    const size_t n_indices = 6;
    std::array<GLuint, 3> bufferObjects{{0, 0, 0}};
};
