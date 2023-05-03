#version 460

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNorm;

layout(location = 0) out vec3 fragPosWorld;

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
    fragPosWorld = (model * vec4(vPos, 1.f)).xyz;

    // just transform vertices into light space
    gl_Position = shadowViewProjection * model * vec4(vPos, 1.f);
}
