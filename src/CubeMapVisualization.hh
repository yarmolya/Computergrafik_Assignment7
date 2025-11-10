#pragma once
//=============================================================================
//
//   Exercise code for "Introduction to Computer Graphics"
//     by Julian Panetta, EPFL
//
//=============================================================================

#include "Quad.hh"
#include "gl.hh"
#include "glmath.hh"
#include "shader.hh"
#include "PointLightShadowMap.hh"

// Visualization to help debug your shadow cube mapping code
struct CubeMapVisualization {
    void initialize();

    // scene_view_matrix: matrix for the view for which the lighting map was computed.
    void draw(const mat4 &scene_view_matrix,   const mat4 &view_matrix, const mat4 &projection_matrix,
              const vec3 &light_pos_scene_eye, const PointLightShadowMap &sm);

    ~CubeMapVisualization() {
        glDeleteTextures(1, &m_demoTex);
    }

    bool demoTextures = true; // Whether to visualize the shadow map or the demo textures that indicate the coordinate system

private:
    Quad m_quad;
    GLuint m_demoTex;
    Shader m_shader;
};
