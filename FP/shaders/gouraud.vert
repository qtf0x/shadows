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

layout(location = 0) out vec3 fragPosWorld;
layout(location = 1) out vec3 fragNormWorld;

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

void main() {
    // transform vertex position and normal into world space
    // (will be interpolated for each fragment)
    fragPosWorld = (model * vec4(vPos, 1.f)).xyz;
    fragNormWorld = (model * vec4(normalize(vNorm), 0.f)).xyz;

    // transform & output the vertex in clip space
    gl_Position = modelViewProjection * vec4(vPos, 1.f);
}
