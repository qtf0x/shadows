#version 460 core

// specify how many vertices make up a patch
layout(vertices = 16) out;

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
    // pass through vertex position unchanged
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    // specify outer and inner tessellation levels
    for (int i = 0; i < 4; ++i) {
        if (i < 2)
            gl_TessLevelInner[i] = tessLevel;

        gl_TessLevelOuter[i] = tessLevel;
    }
}
