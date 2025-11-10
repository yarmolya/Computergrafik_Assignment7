#pragma once
//=============================================================================
//
//   Exercise code for "Introduction to Computer Graphics"
//     by Julian Panetta, EPFL
//
//=============================================================================

#include "gl.hh"
#include "glfw_window.hh"
#include <array>
#include <string>
#include <memory>

#include "shader.hh"
#include "Mesh.hh"
#include "Quad.hh"
#include "sphere.hh"
#include "PointLightShadowMap.hh"
#include "CubeMapVisualization.hh"

class ShadowViewer : public GLFW_window
{
    static constexpr int MAX_LIGHTS = 5;
public:
    using GLFW_window::GLFW_window; // Inherit constructors from GLFW_window

    ShadowViewer(const char *title, int width, int height)
        : GLFW_window(title, width, height), m_width(width), m_height(height) { }

    // Replace the mesh we're viewing with the OFF file at offPath.
    void loadMesh(const std::string &offPath) {
        m_meshPath = offPath;
        m_updateMesh();
    }

    virtual ~ShadowViewer() { }

protected:
    virtual void initialize();
    virtual void paint();
    void draw(const mat4 &view_matrix, const mat4 &projection_matrix);

    virtual void keyboard(int key, int scancode, int action, int mods);

    void displayConfiguration() const {
        for (int l = 0; l < m_numActiveLights; ++l) {
            std::cout << "{" << m_light[l].x_angle << ", "
                             << m_light[l].y_angle << ((l + 1 < m_numActiveLights) ? "}, " : "}\n");
        }
        std::cout << std::endl;
        std::cout << "{" << m_viewParameters.x_angle << ", " << m_viewParameters.y_angle << ", " << m_viewParameters.dist_factor << "}" << std::endl;
    }

    // Basic support for loading pre-set configurations
    void loadConfiguration(size_t i) {
        const std::vector<std::vector<std::array<float, 2>>> activeLightParameters = {
                {{{-40, -70}}, {{-30, 110}}},
                {{{-40, -70}}, {{-30, 110}}, {{-140, 10}}},
                {{{-40, -70}}, {{-30, 110}}, {{-140, 10}}, {{-160, 150}}}
        };

        const std::vector<std::array<float, 3>> viewParameters {
            {{-20, 30, 4.5}},
            {{-20, -100, 4.5}},
            {{-10, -290, 4.5}}
        };

        const auto &alp = activeLightParameters.at(i);
        m_numActiveLights = alp.size();
        for (size_t j = 0; j < alp.size(); ++j)
            m_light[j].set(alp[j]);
        m_viewParameters.set(viewParameters.at(i));
    }

    void resize(int width, int height) {
        glViewport(0, 0, width, height);
        m_width  = width;
        m_height = height;
    }

private:
    ////////////////////////////////////////////////////////////////////////////
    // View configuration
    ////////////////////////////////////////////////////////////////////////////
    struct ViewParameters {
        // Parameters controlling the eye position
        float x_angle      = 0.0f,
              y_angle      = 0.0f,
              dist_factor = 4.5f;
        // Viewing frustum parameters
        float fovy = 45.0f,
              near = 0.01f,
              far  = 20;
        void set(const std::array<float, 3> &a) { x_angle = a[0]; y_angle = a[1]; dist_factor = a[2]; }
    } m_viewParameters;
    float m_width = 0.0f, m_height = 0.0f;

    mat4 m_constructSceneViewMatrix() const;

    ////////////////////////////////////////////////////////////////////////////
    // Light configuration
    ////////////////////////////////////////////////////////////////////////////
    struct Light {
        float x_angle = -90.0f,
              y_angle =   0.0f,
              radius  =   3.0f;
        void set(const std::array<float, 2> &a) { x_angle = a[0]; y_angle = a[1]; }

        vec3 color = vec3(1.0, 1.0, 1.0);

        vec4 position() const {
             return mat4::rotate_y(y_angle) *
                    mat4::rotate_x(x_angle) * vec4(0, 0, radius, 1.0);
        }
    };

    std::array<Light, MAX_LIGHTS> m_light;
    int m_numActiveLights = 1;
    int m_selectedLight = 0;

    std::unique_ptr<PointLightShadowMap> m_shadowMap;
    void m_render_shadow_cubemap(size_t li, const mat4 &plane_m_matrix, const mat4 &mesh_m_matrix);
    mat4 m_constructLightViewMatrix(size_t li, size_t cube_face) const;
    mat4 m_constructLightProjectionMatrix() const;

    Shader m_phong_shader, m_solid_color_shader, m_shadowmap_gen_shader;

    bool m_initialized = false;
    // Flag indicating we should dump screenshots when next drawing the frame.
    bool m_screenshotRequested = false;

    ////////////////////////////////////////////////////////////////////////////
    // Scene geometry
    ////////////////////////////////////////////////////////////////////////////
    Sphere m_unitSphere = Sphere(50); // Sphere used to visualize light positions
    Quad m_quad;
    std::unique_ptr<Mesh> m_mesh;

    std::string m_meshPath;
    void m_updateMesh();

    ////////////////////////////////////////////////////////////////////////////
    // Debugging visualization features
    ////////////////////////////////////////////////////////////////////////////
    // Option to view the scene from the perspective used to draw
    // each face of the active light's shade cube map.
    static constexpr size_t NONE = 6;
    size_t m_viewLightCubeFace = NONE;
    size_t m_cubeMapVisualizationMode = 0; // 0 = off, 1 = demo texture, 2 = shadow map
    CubeMapVisualization m_cubeMapVisualization;
};
