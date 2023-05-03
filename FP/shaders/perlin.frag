/**
 * @file teapotahedron.frag
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#version 460 core

layout(location = 0) in vec3 fragPosWorld;  // interpolated world-space position
layout(location = 1) in vec3 fragNormWorld; // interpolated world-space normal
layout(location = 2) in vec3 edgeDistances; // interpolated triangle edge dists

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

layout(shared, binding = 2) uniform Material {
    vec3 materialAmb;  // ambient reflectivity
    vec3 materialDiff; // diffuse reflectivity
    vec3 materialSpec; // specular reflectivity

    float shininess; // specular shininess factor
};

vec3 blinnPhongSpecular(vec3 fragPosWorld, vec3 fragNormWorld, vec3 lightVec,
                        float lightDotNorm) {
    vec3 viewVec = normalize(eyePos - fragPosWorld);
    vec3 halfwayVec = normalize(viewVec + lightVec);

    return lightSpec * materialSpec *
           pow(max(dot(halfwayVec, fragNormWorld), 0.f), 4.f * shininess) *
           lightDotNorm;
}

vec3 phongModel(vec3 fragPosWorld, vec3 fragNormWorld) {
    // compute ambient component
    vec3 ambient = lightAmb * materialAmb;

    // used for diffuse and specular
    vec3 lightVec = normalize(lightPos.xyz - fragPosWorld);
    float lightDotNorm = max(dot(lightVec, fragNormWorld), 0.f);

    // compute diffuse component
    vec3 diffuse = lightDiff * materialDiff * lightDotNorm;

    // compute specular component
    vec3 specular =
        blinnPhongSpecular(fragPosWorld, fragNormWorld, lightVec, lightDotNorm);

    // compute attenuation
    float lightDist = distance(lightPos.xyz, fragPosWorld);
    float attenuation =
        attenConst + attenLin * lightDist + attenQuad * pow(lightDist, 2);

    return (ambient + diffuse + specular) / attenuation;
}

/* Perlin Noise Stuff */

vec3 gradients[16];
int table[16];

void init() {
    int i;
    gradients[0] = vec3(0, -1, -1);
    gradients[1] = vec3(1, 0, -1);
    gradients[2] = vec3(0, -1, 1);
    gradients[3] = vec3(0, 1, -1);
    gradients[4] = vec3(1, -1, 0);
    gradients[5] = vec3(1, 1, 0);
    gradients[6] = vec3(-1, 1, 0);
    gradients[7] = vec3(0, 1, 1);
    gradients[8] = vec3(-1, 0, -1);
    gradients[9] = vec3(1, 1, 0);
    gradients[10] = vec3(-1, 1, 0);
    gradients[11] = vec3(-1, -1, 0);
    gradients[12] = vec3(1, 0, 1);
    gradients[13] = vec3(-1, 0, 1);
    gradients[14] = vec3(0, -1, 1);
    gradients[15] = vec3(0, -1, -1);
    for (i = 0; i < 16; i++)
        table[i] = i;
}

float smoothingFunc(float t) {
    t = (t > 0.) ? t : -t;

    float t3 = t * t * t;
    float t4 = t3 * t;

    return -6 * t4 * t + 15 * t4 - 10 * t3 + 1.;
}

float randomNumber(float u, float v, float w, int i, int j, int k) {
    int idx;
    idx = table[abs(k) % 16];
    idx = table[abs(j + idx) % 16];
    idx = table[abs(i + idx) % 16];

    vec3 gijk = gradients[idx];
    vec3 uvw = vec3(u, v, w);

    return smoothingFunc(u) * smoothingFunc(v) * smoothingFunc(w) *
           dot(gijk, uvw);
}

float perlin(vec3 pos, float scalingFactor) {
    float x = scalingFactor * pos.x;
    float y = scalingFactor * pos.y;
    float z = scalingFactor * pos.z;

    int xmin = int(floor(x));
    int ymin = int(floor(y));
    int zmin = int(floor(z));

    float n = 0.f;
    for (int i = xmin; i <= xmin + 1; ++i) {
        for (int j = ymin; j <= ymin + 1; ++j) {
            for (int k = zmin; k <= zmin + 1; ++k)
                n += randomNumber(x - i, y - j, z - k, i, j, k);
        }
    }

    // return (n + 1.f) / 2.f; // the non-abs version
    return abs(n);
}

void main() {
    /* Perlin Noise Generation */

    float a = 0.f;

    init();

    float xPeriod = 5.f, yPeriod = 10.f; // defines repetition of marble lines
    float turbPower = 20.f;              // makes twists
    float turbSize = 2.f;

    // float xyValue = fragPosWorld.x * xPeriod / 100.f +
    //                 fragPosWorld.y * yPeriod / 100.f +
    //                 turbPower * perlin(fragPosWorld, turbSize);

    // a = abs(sin(xyValue * 3.14159f));

    // add five octaves of noise
    for (int i = 0; i < 3; ++i) {
        float xyValue = fragPosWorld.x * xPeriod / 100.f +
                        fragPosWorld.y * yPeriod / 100.f +
                        turbPower * perlin(fragPosWorld, i * turbSize) *
                            pow(2, -i * turbSize);

        a += abs(sin(xyValue * 3.14159f)) / 8.f;
    }

    // a = pow(a, 2.5); // increase the power to decrease cloud size

    // if (a < 0.02) // increase the threshold for sharper cloud boundaries
    //     discard;

    a = (log(a) - log(1e-1)) / (log(1) - log(1e-1)); // smoothing function

    /* ====================================================================== */

    // if we are looking at the front face of the fragment
    if (gl_FrontFacing)
        fragColor =
            vec4(phongModel(fragPosWorld, normalize(fragNormWorld)), 1.f);

    // otherwise we are looking at the back face of the fragment
    // apply color w/ flipped normal
    else
        fragColor =
            vec4(phongModel(fragPosWorld, normalize(-fragNormWorld)), 1.f);

    // fragColor = mix(fragColor, vec4(vec3(1.f, 1.f, 1.f) * a, 1.f), 0.5f);

    if (wireframe == 1) {
        // find smallest edge distance
        float minEdgeDist =
            min(edgeDistances.x, min(edgeDistances.y, edgeDistances.z));

        // hard-code edge properties
        float edgeWidth = 1.f;
        vec4 edgeColor = vec4(1.f, 1.f, 0.f, 1.f);

        // determine mix factor with the edge color
        float mixVal =
            smoothstep(edgeWidth - 1.f, edgeWidth + 1.f, minEdgeDist);

        // mix surface color with the edge color
        fragColor = mix(edgeColor, fragColor, mixVal);
    }
}
