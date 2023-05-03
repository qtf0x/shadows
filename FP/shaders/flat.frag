/**
 * @file flat.frag
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#version 460 core

layout(location = 0) in vec3 edgeDistances;

layout(location = 0) out vec4 fragColor; // color to apply to this fragment

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

layout(shared, binding = 2) uniform Material {
    vec3 materialAmb;  // ambient reflectivity
    vec3 materialDiff; // diffuse reflectivity
    vec3 materialSpec; // specular reflectivity

    float shininess; // specular shininess factor
};

void main() {
    if (controlPoints == 0)
        discard;

    fragColor = vec4(materialDiff, 1.f);

    if (wireframe == 1) {
        // find smallest edge distance
        float minEdgeDist =
            min(edgeDistances.x, min(edgeDistances.y, edgeDistances.z));

        // hard-code edge properties
        float edgeWidth = 1.f;
        vec4 edgeColor = vec4(1.f, 1.f, 0.f, 1.f);

        if (minEdgeDist > edgeWidth && controlPoints == 0)
            discard; // don't render fragments not part of the line
        else if (controlPoints == 0)
            fragColor = edgeColor;
        else {
            // determine mix factor with the edge color
            float mixVal =
                smoothstep(edgeWidth - 1.f, edgeWidth + 1.f, minEdgeDist);

            // mix surface color with the edge color
            fragColor = mix(edgeColor, fragColor, mixVal);
        }
    }
}
