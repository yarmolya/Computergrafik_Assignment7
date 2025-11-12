//=============================================================================
//
//   Exercise code for "Introduction to Computer Graphics"
//     by Julian Panetta, EPFL
//
//=============================================================================
#version 140

// Eye-space fragment position and normals from vertex shader.
in vec3 v2f_normal;
in vec3 v2f_ec_vertex;

uniform vec3 light_position; // Eye-space light position
uniform vec3 light_color;

// Material parameters
uniform vec3 diffuse_color;
uniform vec3 specular_color;
uniform float shininess;

uniform samplerCube shadow_map; // Distances in the shadow map can be accessed with texture(shadow_map, direction).r

out vec4 f_light_contribution;

void main()
{
    //vec3 color = vec3(0.0f);
    vec3 color = light_color;
    // Orient the normal so it always points opposite the camera rays:
    vec3 N = -sign(dot(v2f_normal, v2f_ec_vertex)) *
             normalize(v2f_normal);

    /** \todo
    * Compute this light's diffuse and specular contributions.
    * You should be able to copy your phong lighting code from assignment 6 mostly as-is,
    * though notice that the light and view vectors need to be computed from scratch
    * here; this time, they are not passed from the vertex shader.
    *
    * The light should only contribute to this fragment if the fragment is not occluded
    * by another object in the scene. You need to check this by comparing the distance
    * from the fragment to the light against the distance recorded for this
    * light ray in the shadow map.
    *
    * To prevent "shadow acne" and minimize aliasing issues, we need a rather large
    * tolerance on the distance comparison. It's recommended to use a *multiplicative*
    * instead of additive tolerance: compare the fragment's distance to 1.01x the
    * distance from the shadow map.
    ***/
    vec3 light_ray = light_position - v2f_ec_vertex;

    //camera position is at origin! therefore v is vector from fragment position to origin
    vec3 v2f_view = -(v2f_ec_vertex);

    vec3 L = normalize(light_ray);
    vec3 V = normalize (v2f_view);
    vec3 R = reflect(-L,N);

    vec3 fragment_to_light = v2f_ec_vertex - light_position;
    float shadowMapDist = texture(shadow_map, fragment_to_light).r;
    float fragDist = length(light_ray);

    float offset = 1.01; //prevents shadow acne
    bool isShadowed = fragDist > shadowMapDist * offset;

    // diffuse and specular component
    vec3 diffuse = vec3(0.0);
    vec3 specular = vec3(0.0);
    float cos_theta = dot(N,L);

    if (!isShadowed) {
        if (cos_theta > 0) {
        diffuse = light_color * diffuse_color * cos_theta;
        float cos_alpha = dot(R,V);
        if (cos_alpha > 0) {
            specular = light_color * specular_color * pow(cos_alpha,shininess);
            }
        }
    }

    color = diffuse + specular;

    // append the required alpha value
    f_light_contribution = vec4(color, 1.0);

}
