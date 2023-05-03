/**
 * @file teapotahedron.geom
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#version 460 core

// primitive I/O
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

// attribute inputs
layout(location = 0) in vec3 fragPosWorldArr[];  // world-space positions
layout(location = 1) in vec3 fragNormWorldArr[]; // world-space normals

// pass-through attribute outputs
layout(location = 0) out vec3 fragPosWorld;
layout(location = 1) out vec3 fragNormWorld;
layout(location = 2) noperspective out vec3 edgeDistances;

// scene uniforms (really only need viewport)
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
    float ha = 0.f, hb = 0.f, hc = 0.f;

    if (wireframe == 1) {
        // perform perspective division and viewport transform for each vertex
        vec3 p0 = vec3(viewportMatrix *
                       (gl_in[0].gl_Position / gl_in[0].gl_Position.w));
        vec3 p1 = vec3(viewportMatrix *
                       (gl_in[1].gl_Position / gl_in[1].gl_Position.w));
        vec3 p2 = vec3(viewportMatrix *
                       (gl_in[2].gl_Position / gl_in[2].gl_Position.w));

        // calculate side lengths of triangle
        float a = length(p0 - p1);
        float b = length(p1 - p2);
        float c = length(p2 - p0);

        // calculate interior angles of triangle using law of cosines
        float alpha = acos((b * b + c * c - a * a) / (2.f * b * c));
        float beta = acos((a * a + c * c - b * b) / (2.f * a * c));

        // calculate altitudes from each vertex
        ha = c * sin(beta);
        hb = c * sin(alpha);
        hc = b * sin(alpha);
    }

    // set attributes and emit vertices
    edgeDistances = vec3(0.f, hb, 0.f);
    gl_Position = gl_in[0].gl_Position;
    fragPosWorld = fragPosWorldArr[0];
    fragNormWorld = fragNormWorldArr[0];
    EmitVertex();

    edgeDistances = vec3(0.f, 0.f, hc);
    gl_Position = gl_in[1].gl_Position;
    fragPosWorld = fragPosWorldArr[1];
    fragNormWorld = fragNormWorldArr[1];
    EmitVertex();

    edgeDistances = vec3(ha, 0.f, 0.f);
    gl_Position = gl_in[2].gl_Position;
    fragPosWorld = fragPosWorldArr[2];
    fragNormWorld = fragNormWorldArr[2];
    EmitVertex();

    EndPrimitive();
}
