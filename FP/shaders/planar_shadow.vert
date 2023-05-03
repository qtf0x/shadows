/**
 * @file teapotahedron.vert
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#version 460 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;

layout(shared, binding = 0) uniform Scene {
    mat4 model; // model matrix
    mat4 viewProjection;
    mat4 modelViewProjection; // model-view-projection matrix
    mat4 viewportMatrix;      // viewport matrix

    mat4 shadowViewProjection; // shadow transforms

    float tessLevel; // inner/outer tessellation level

    vec3 eyePos; // eye position in world space

    int wireframe;     // use wireframe rendering
    int controlPoints; // show control points
};

layout(shared, binding = 1) uniform Light {
    vec4 lightPos; // light position in world space

    vec3 lightAmb;  // ambient light intensity
    vec3 lightDiff; // diffuse light intensity
    vec3 lightSpec; // specular light intensity

    float attenConst; // constant attenuation term
    float attenLin;   // linear attenuation term
    float attenQuad;  // quadratic attenuation term

    float shadowBias;
    int doMultisampling;
    float shadowMapSamples;
};

void main() {
    // planar projected shadow
    vec4 n = vec4(0.f, 1.f, 0.f, 0.f);
    vec4 l = lightPos;
    float d = 0.f;

    mat4 shadowProjection =
        mat4(dot(n, l) + d - n.x * l.x, -n.x * l.y, -n.x * l.z, -n.x,
             -n.y * l.x, dot(n, l) + d - n.y * l.y, -n.y * l.z, -n.y,
             -n.z * l.x, -n.z * l.y, dot(n, l) + d - n.z * l.z, -n.z, -d * l.x,
             -d * l.y, -d * l.z, dot(n, l));

    // transform & output the vertex in clip space
    gl_Position = viewProjection * shadowProjection * model * vec4(vPos, 1.f);
}
