/**
 * @file flat.frag
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#version 460 core

layout(location = 0) out vec4 fragColor; // color to apply to this fragment

layout(shared, binding = 2) uniform Material {
    vec3 materialAmb;  // ambient reflectivity
    vec3 materialDiff; // diffuse reflectivity
    vec3 materialSpec; // specular reflectivity

    float shininess; // specular shininess factor
};

void main() {
    fragColor = vec4(0.f, 0.f, 0.f, shininess); // black
}
