#version 460

layout(location = 0) in vec3 fragPosWorld;

layout(shared, binding = 1) uniform Light {
    vec4 lightPos; // light position in world space

    vec3 lightAmb;  // ambient light intensity
    vec3 lightDiff; // diffuse light intensity
    vec3 lightSpec; // specular light intensity

    float attenConst; // constant attenuation term
    float attenLin;   // linear attenuation term
    float attenQuad;  // quadratic attenuation term

    float shadowBias;
    int doMultisampling;
    float shadowMapSamples;
};

void main() {
    // get distance between fragment and light source
    float lightDistance = length(fragPosWorld - lightPos.xyz);

    // map to [0;1] range by dividing by far plane
    lightDistance = lightDistance / 1000.f;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}
