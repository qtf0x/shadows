#version 460 core

layout(quads, equal_spacing, ccw) in;

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

// solve the bezier curve equation for 4 points and a parameter value
vec4 evalBezierCurve(vec4 P0, vec4 P1, vec4 P2, vec4 P3, float t) {
    return (-P0 + 3.f * P1 - 3.f * P2 + P3) * pow(t, 3.f) +
           (3.f * P0 - 6.f * P1 + 3.f * P2) * pow(t, 2.f) +
           (-3.f * P0 + 3.f * P1) * t + P0;
}

void main() {
    // get tessellation parameters
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    // get our control points - rename for ease of access
    vec4 p00 = gl_in[0].gl_Position;
    vec4 p01 = gl_in[1].gl_Position;
    vec4 p02 = gl_in[2].gl_Position;
    vec4 p03 = gl_in[3].gl_Position;
    vec4 p04 = gl_in[4].gl_Position;
    vec4 p05 = gl_in[5].gl_Position;
    vec4 p06 = gl_in[6].gl_Position;
    vec4 p07 = gl_in[7].gl_Position;
    vec4 p08 = gl_in[8].gl_Position;
    vec4 p09 = gl_in[9].gl_Position;
    vec4 p10 = gl_in[10].gl_Position;
    vec4 p11 = gl_in[11].gl_Position;
    vec4 p12 = gl_in[12].gl_Position;
    vec4 p13 = gl_in[13].gl_Position;
    vec4 p14 = gl_in[14].gl_Position;
    vec4 p15 = gl_in[15].gl_Position;

    // evaluate our bezier surface at point (u, v)
    vec4 bezierPoint =
        evalBezierCurve(evalBezierCurve(p00, p01, p02, p03, u),
                        evalBezierCurve(p04, p05, p06, p07, u),
                        evalBezierCurve(p08, p09, p10, p11, u),
                        evalBezierCurve(p12, p13, p14, p15, u), v);

    // output bezier point
    gl_Position = shadowViewProjection * model * vec4(bezierPoint.xyz, 1.f);
}
