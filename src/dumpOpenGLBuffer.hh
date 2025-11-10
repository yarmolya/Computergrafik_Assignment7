#pragma once
//=============================================================================
//
//   Exercise code for "Introduction to Computer Graphics"
//     by Julian Panetta, EPFL
//
//=============================================================================

#include <string>

// Dump the currently active OpenGL frame buffer to a PNG file
void dumpOpenGLBuffer(const std::string &path, size_t width, size_t height);

// Dump the currently active OpenGL frame buffer to a PNG file
void dumpOpenGLDepthBuffer(const std::string &path, size_t width, size_t height);
