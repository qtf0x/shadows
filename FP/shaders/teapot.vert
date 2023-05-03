#version 460 core

layout(location = 0) in vec3 vPos;

void main() {
    // pass through vertex positions unchanged
    gl_Position = vec4(vPos, 1.f);
}
