//=============================================================================
//
//   Exercise code for "Introduction to Computer Graphics"
//     by Julian Panetta, EPFL
//
//=============================================================================
#include "ShadowViewer.hh"
#include "dumpOpenGLBuffer.hh"
#include "Quad.hh"
#include <cassert>

void ShadowViewer::initialize() {
    m_initialized = true;
    m_quad.initialize();
    m_updateMesh();
    m_cubeMapVisualization.initialize();

    m_phong_shader        .load(SHADER_PATH "/phong_shadow.vert",  SHADER_PATH "/phong_shadow.frag");
    m_solid_color_shader  .load(SHADER_PATH "/solid_color.vert",   SHADER_PATH "/solid_color.frag");
    m_shadowmap_gen_shader.load(SHADER_PATH "/shadowmap_gen.vert", SHADER_PATH "/shadowmap_gen.frag");

    m_shadowMap = std::unique_ptr<PointLightShadowMap>(new PointLightShadowMap());
}

// Replace the mesh with one read from m_meshPath
void ShadowViewer::m_updateMesh() {
    if (!m_initialized || m_meshPath.empty()) return; // We can only build the mesh after OpenGL has been initialized.
    if (!m_mesh) m_mesh = std::unique_ptr<Mesh>(new Mesh(m_meshPath));
    else         m_mesh->read(m_meshPath);
}

// Construct the viewing matrix needed to render cube face 'cube_face' of the
// shadow cube map for light 'li'.
mat4 ShadowViewer::m_constructLightViewMatrix(size_t li, size_t cube_face) const {
    assert((li < m_numActiveLights) && (cube_face < 6));
    mat4 scene_view_matrix = m_constructSceneViewMatrix();
    /** \todo
    * Construct the viewing matrix for rendering the scene from the perspective
    * of the light looking through cube face number cube_face.
    * Recall that the cube faces are indexed in the order: +x, -x, +y, -y, +z, -z
    * See the assignment handout for an illustration of how to orient the "light camera".
    * Note, for compatibility with the Phong lighting calculation done in eye
    * space, the cube face should be oriented relative to the eye coordinates
    * defined by scene_view_matrix.
    * Hint: use mat4::look_at
    **/
    // light position in eye coordinates
    vec3 light_pos_eye = scene_view_matrix * m_light[li].position();

    // look direction and up vector for each cube face
    // cube faces: 0=+x, 1=-x, 2=+y, 3=-y, 4=+z, 5=-z
    vec3 direction, up;

    switch(cube_face) {
    case 0: // +X face
        direction = vec3(1.0f, 0.0f, 0.0f);
        up = vec3(0.0f, 1.0f, 0.0f);  // Changed from -1 to +1
        break;
    case 1: // -X face
        direction = vec3(-1.0f, 0.0f, 0.0f);
        up = vec3(0.0f, 1.0f, 0.0f);  // Changed from -1 to +1
        break;
    case 2: // +Y face
        direction = vec3(0.0f, 1.0f, 0.0f);
        up = vec3(0.0f, 0.0f, -1.0f);  // Changed from +1 to -1
        break;
    case 3: // -Y face
        direction = vec3(0.0f, -1.0f, 0.0f);
        up = vec3(0.0f, 0.0f, 1.0f);  // Changed from -1 to +1
        break;
    case 4: // +Z face
        direction = vec3(0.0f, 0.0f, 1.0f);
        up = vec3(0.0f, 1.0f, 0.0f);  // Changed from -1 to +1
        break;
    case 5: // -Z face
        direction = vec3(0.0f, 0.0f, -1.0f);
        up = vec3(0.0f, 1.0f, 0.0f);  // Changed from -1 to +1
        break;
    }

    // center is light position + direction
    vec3 center = light_pos_eye + direction;

    mat4 look_eye = mat4::look_at(light_pos_eye, center, up);
    return look_eye * scene_view_matrix;
}

mat4 ShadowViewer::m_constructLightProjectionMatrix() const {
    /** \todo
    * Construct the projection matrix for rendering the scene from the perspective
    * of the light to generate shadow maps.
    **/
    float fov = 90.0f;  // 90 degrees for cube map faces
    float aspect = 1.0f; // square cube map faces
    float near = 0.1f;   // near clipping plane
    float far = 6.0f;  // far clipping plane

    return mat4::perspective(fov, aspect, near, far);
    return mat4::identity();
}

void ShadowViewer::m_render_shadow_cubemap(size_t li, const mat4 &plane_m_matrix, const mat4 &mesh_m_matrix) {
    // Render shadow maps to the cube map texture
    // Loop over cube faces in order: +x, -x, +y, -y, +z, -z
    for (size_t cube_face = 0; cube_face < 6; ++cube_face) {
        m_shadowMap->selectCubeFaceTextureAsRenderTarget(cube_face);

        // Clear depth map with high depths
        glClearColor(100.0f, 100.0f, 100.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_shadowmap_gen_shader.use();
        mat4 light_view = m_constructLightViewMatrix(li, cube_face);
        mat4 light_proj = m_constructLightProjectionMatrix();
        m_shadowmap_gen_shader.set_uniform("modelview_matrix",                         light_view * plane_m_matrix, true);
        m_shadowmap_gen_shader.set_uniform("modelview_projection_matrix", light_proj * light_view * plane_m_matrix);
        m_quad.draw();

        m_shadowmap_gen_shader.set_uniform("modelview_matrix",                         light_view * mesh_m_matrix, true);
        m_shadowmap_gen_shader.set_uniform("modelview_projection_matrix", light_proj * light_view * mesh_m_matrix);
        m_mesh->draw();

        // Switch back to rendering to the screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, m_width, m_height);
    }
}

// Draw the scene using view_matrix and projection_matrix.
// Note, view_matrix can differ from m_constructSceneViewMatrix(),
// e.g. when viewing the scene from the perspective of the shadow cube faces
// for debugging.
void ShadowViewer::draw(const mat4 &view_matrix, const mat4 &projection_matrix) {
    if (!m_mesh) throw std::runtime_error("No mesh to draw!");

    // Clear framebuffer and depth buffer first
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL); // We need to re-draw the front-most fragments multiple times
    // (we cannot use the default setting of GL_LESS)
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

    /////////////////////////////////////
    // Transformation/projection matrices
    /////////////////////////////////////
    mat4 plane_m_matrix   = mat4::scale(3.0) * mat4::rotate_x(90.0f);
    mat4 plane_mv_matrix  = view_matrix * plane_m_matrix;
    mat4 plane_mvp_matrix = projection_matrix * plane_mv_matrix;

    mat4 mesh_m_matrix    = m_mesh->modelMatrix();
    mat4 mesh_mv_matrix   = view_matrix * mesh_m_matrix;
    mat4 mesh_mvp_matrix  = projection_matrix * mesh_mv_matrix;


    // \todo Construct the matrices for transforming normals into eye coordinates
    //       You can paste in your solution from assignment 6.
    mat3 plane_n_matrix = transpose(inverse(plane_mv_matrix));
    mat3 mesh_n_matrix  = transpose(inverse(mesh_mv_matrix));

    vec3 ambient_light(0.2, 0.2, 0.2),
        plane_diffuse (0.5, 0.5, 0.7), // used as ambient color too
        plane_specular(0.0, 0.0, 0.0),
        mesh_diffuse (1.0, 0.0, 0.0), // used as ambient color too
        mesh_specular(1.0, 1.0, 1.0);

    // Render the ambient light contribution
    m_solid_color_shader.use();
    m_solid_color_shader.set_uniform("color", ambient_light * plane_diffuse);
    m_solid_color_shader.set_uniform("modelview_projection_matrix", plane_mvp_matrix);
    m_quad.draw();

    m_solid_color_shader.use();
    m_solid_color_shader.set_uniform("color", ambient_light * mesh_diffuse);
    m_solid_color_shader.set_uniform("modelview_projection_matrix", mesh_mvp_matrix);
    m_mesh->draw();

    // Render contribution of each light
    for (int li = 0; li < m_numActiveLights; ++li) {
        m_render_shadow_cubemap(li, plane_m_matrix, mesh_m_matrix);

        /** \todo
        * Configure OpenGL to *add* the specular and diffuse contribution
        * output by our shader to the colors already in the framebuffer.
        * Hint: read the documentation for glBlendFunc
        **/
        glEnable(GL_BLEND);
        glBlendFunc(GL_ONE, GL_ONE);

        m_phong_shader.use();
        m_shadowMap->bind();

        m_phong_shader.set_uniform("shininess", 8.0f); // pass 'optional = true' to avoid 'Invalid uniform location'
        m_phong_shader.set_uniform("shadow_map",   0); // warnings caused by incomplete shader implementation

        /** \todo
         * Draw this light's specular and diffuse contribution on the floor
         * plane (m_quad) and mesh (m_mesh) (taking the shadows into account).
         * You'll need to pass in the light position ***in eye coordinates** as
         * well as the proper material and transformation matrices.
         **/

        //calculate light position in eye coordinates
        vec3 light_pos_from_eye =  view_matrix * m_light[li].position();

        //render m_quad
        //set vert uniforms
        m_phong_shader.set_uniform("modelview_matrix", plane_mv_matrix);
        m_phong_shader.set_uniform("modelview_projection_matrix", plane_mvp_matrix);
        m_phong_shader.set_uniform("normal_matrix", plane_n_matrix);

        //set fragment uniforms
        m_phong_shader.set_uniform("light_position", light_pos_from_eye);
        m_phong_shader.set_uniform("light_color", m_light[li].color);
        m_phong_shader.set_uniform("specular_color", plane_specular);
        m_phong_shader.set_uniform("diffuse_color", plane_diffuse);

        m_quad.draw();

        //render m_mesh
        //set vert uniforms
        m_phong_shader.set_uniform("modelview_matrix", mesh_mv_matrix);
        m_phong_shader.set_uniform("modelview_projection_matrix", mesh_mvp_matrix);
        m_phong_shader.set_uniform("normal_matrix", mesh_n_matrix);

        //set fragment uniforms
        m_phong_shader.set_uniform("light_position", light_pos_from_eye);
        m_phong_shader.set_uniform("light_color", m_light[li].color);
        m_phong_shader.set_uniform("specular_color", mesh_specular);
        m_phong_shader.set_uniform("diffuse_color", mesh_diffuse);

        m_mesh->draw();

        m_shadowMap->unbind();

        // All other shaders should overwrite the framebuffer color, not add to it...
        glDisable(GL_BLEND);
        glCheckError();
    }

    // Draw the lights with solid color spheres
    m_solid_color_shader.use();
    for (int i = 0; i < m_numActiveLights; ++i) {
        mat4 mvp_matrix = projection_matrix * view_matrix * mat4::translate(m_light[i].position()) * mat4::scale(0.05);
        m_solid_color_shader.set_uniform("modelview_projection_matrix", mvp_matrix);
        vec3 color = (m_selectedLight == i) ? vec3(0.5, 1.0, 0.5)
                                            : vec3(0.9, 0.9, 0.9);
        m_solid_color_shader.set_uniform("color", color);
        m_unitSphere.draw();
    }

    // Draw a visualization of the cube map computed for the *last* light
    // (since it's the last light that drew to the shadow map buffer).
    if ((m_numActiveLights > 0) && (m_cubeMapVisualizationMode > 0)) {
        mat4 scene_view = m_constructSceneViewMatrix();
        m_cubeMapVisualization.demoTextures = m_cubeMapVisualizationMode == 1;
        m_cubeMapVisualization.draw(scene_view, view_matrix, projection_matrix,
                                    scene_view * m_light[m_numActiveLights - 1].position(), *m_shadowMap);
    }

    if (m_screenshotRequested) {
        static int screenshot_counter = 0;
        ++screenshot_counter;
        dumpOpenGLBuffer("screenshot_" + std::to_string(screenshot_counter) + ".png", m_width, m_height);
        m_screenshotRequested = false;
    }
}

mat4 ShadowViewer::m_constructSceneViewMatrix() const {
    vec3 center(0.0f, 1.0f, 0.0f); // object's center
    float view_radius = sqrt(3.0);
    mat3 rot = mat4::rotate_y(m_viewParameters.y_angle) *
               mat4::rotate_x(m_viewParameters.x_angle);
    vec3 eye = rot * vec3(0, 0, m_viewParameters.dist_factor * view_radius) + vec3(0.0f, 1.0f, 0.0f);
    vec3 up =  rot * vec3(0, 1, 0);
    return mat4::look_at(eye, center, up);
}

void ShadowViewer::paint() {
    mat4 view_matrix = m_constructSceneViewMatrix();
    mat4 projection_matrix = mat4::perspective(m_viewParameters.fovy,
                                               float(m_width) / float(m_height),
                                               m_viewParameters.near,
                                               m_viewParameters.far);

    // For debugging, we can render the scene from the viewpoint of the last light,
    // as computed for rendering the requested shadow cube map face.
    if ((m_numActiveLights > 0) && (m_viewLightCubeFace < NONE)) {
        view_matrix       = m_constructLightViewMatrix(m_selectedLight, m_viewLightCubeFace);
        projection_matrix = m_constructLightProjectionMatrix();
    }

    draw(view_matrix, projection_matrix);
    glCheckError();
}

void ShadowViewer::keyboard(int key, int /*scancode*/, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch(key) {
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window_, GL_TRUE);
            break;
        // View controls
        case GLFW_KEY_9:
            m_viewParameters.dist_factor = std::min(m_viewParameters.dist_factor + 0.1f,20.0f);
            break;
        case GLFW_KEY_8:
            m_viewParameters.dist_factor = std::max(m_viewParameters.dist_factor - 0.1f,2.5f);
            break;
        case GLFW_KEY_LEFT:
            m_viewParameters.y_angle -= 10.0;
            break;
        case GLFW_KEY_RIGHT:
            m_viewParameters.y_angle += 10.0;
            break;
        case GLFW_KEY_DOWN:
            m_viewParameters.x_angle += 10.0;
            break;
        case GLFW_KEY_UP:
            m_viewParameters.x_angle -= 10.0;
            break;
        case GLFW_KEY_F:
        {
            size_t direction = (mods & GLFW_MOD_SHIFT) ? 6 /* mod(-1, 7) */ : 1;
            m_viewLightCubeFace = (m_viewLightCubeFace + direction) % 7;
            break;
        }
        case GLFW_KEY_C:
        {
            m_cubeMapVisualizationMode = (m_cubeMapVisualizationMode + 1) % 3;
            break;
        }

        case GLFW_KEY_PERIOD:
            displayConfiguration();
            break;

        case GLFW_KEY_1:
        case GLFW_KEY_2:
        case GLFW_KEY_3:
            loadConfiguration(key - GLFW_KEY_1);
            break;

        // Light management
        case GLFW_KEY_MINUS: // Remove a light from the scene
            m_numActiveLights = std::max(0, m_numActiveLights - 1);
            if (m_numActiveLights > 0)
                m_selectedLight = std::min(m_numActiveLights - 1, m_selectedLight);
            break;
        case GLFW_KEY_EQUAL: // Add a light to the scene
            m_numActiveLights = std::min(int(MAX_LIGHTS), m_numActiveLights + 1);
            m_selectedLight   = m_numActiveLights - 1;
            break;
        case GLFW_KEY_TAB: // Select the next active light for keyboard control
            if (m_numActiveLights > 0)
                m_selectedLight = (m_selectedLight + 1) % m_numActiveLights;
            break;

        // Light controls
        case GLFW_KEY_W: // Light up
            m_light[m_selectedLight].x_angle += 10.0;
            break;
        case GLFW_KEY_S: // Light down
            m_light[m_selectedLight].x_angle -= 10.0;
            break;
        case GLFW_KEY_A: // Light clockwse
            m_light[m_selectedLight].y_angle -= 10.0;
            break;
        case GLFW_KEY_D: // Light counterclockwise
            m_light[m_selectedLight].y_angle += 10.0;
            break;

        // Misc controls
        case GLFW_KEY_P: // Print screen (dump screenshots)
            m_screenshotRequested = true;
            break;

        case GLFW_KEY_J: // Shader hot-reload
            m_phong_shader.reload();
            m_solid_color_shader.reload();
            m_shadowmap_gen_shader.reload();
            break;
        }
    }
}
