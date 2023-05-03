#version 460

layout(triangles) in;
layout(triangle_strip, max_vertices = 18) out;

layout(shared, binding = 0) uniform Scene {
    mat4 model; // model matrix
    mat4 viewProjection;
    mat4 modelViewProjection; // model-view-projection matrix
    mat4 viewportMatrix;      // viewport matrix

    mat4 shadowViewProjections[6]; // shadow transforms

    float tessLevel; // inner/outer tessellation level

    vec3 eyePos; // eye position in world space

    int wireframe;     // use wireframe rendering
    int controlPoints; // show control points
};

void emitFace(mat4 m) {
    for (int i = 0; i < 3; ++i) {
        gl_Position = m * gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
}

void main() {
    gl_Layer = 0; // built-in variable that specifies cubemap face
    emitFace(shadowViewProjections[0]);

    gl_Layer = 1;
    emitFace(shadowViewProjections[1]);

    gl_Layer = 2;
    emitFace(shadowViewProjections[2]);

    gl_Layer = 3;
    emitFace(shadowViewProjections[3]);

    gl_Layer = 4;
    emitFace(shadowViewProjections[4]);

    gl_Layer = 5;
    emitFace(shadowViewProjections[5]);
}
