#version 460

layout(location = 0) out vec4 fragColor; // color to apply to this fragment

void main() {
    // just draw the occluders as white (everything else should be black)
    fragColor = vec4(1.f);
}
