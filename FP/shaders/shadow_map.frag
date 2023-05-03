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

uniform samplerCube shadowMap;

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

float ShadowCalculation() {
    /* https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows */

    // get vector between fragment position and light position
    vec3 fragToLight = fragPosWorld - lightPos.xyz;
    // use the light to fragment vector to sample from the depth map
    float closestDepth = texture(shadowMap, fragToLight).r;
    // it is currently in linear range between [0,1]. Re-transform back to
    // original value
    closestDepth *= 1000.f;
    // now get current linear depth as the length between the fragment and light
    // position
    float currentDepth = length(fragToLight);

    if (doMultisampling == 0)
        return currentDepth - shadowBias > closestDepth ? 1.f : 0.f;

    // sample multiple times along each axis, average results
    else if (doMultisampling == 1) {
        float shadow = 0.0;
        float offset = 0.1;
        for (float x = -offset; x < offset;
             x += offset / (shadowMapSamples * 0.5)) {
            for (float y = -offset; y < offset;
                 y += offset / (shadowMapSamples * 0.5)) {
                for (float z = -offset; z < offset;
                     z += offset / (shadowMapSamples * 0.5)) {
                    float closestDepth =
                        texture(shadowMap, fragToLight + vec3(x, y, z)).r;
                    closestDepth *= 1000.f; // undo mapping [0;1]
                    if (currentDepth - shadowBias > closestDepth)
                        shadow += 1.0;
                }
            }
        }
        shadow /= (shadowMapSamples * shadowMapSamples * shadowMapSamples);

        return shadow;
    }
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

    return (ambient + (1.f - ShadowCalculation()) * (diffuse + specular)) /
           attenuation;
}

void main() {
    // if we are looking at the front face of the fragment
    if (gl_FrontFacing)
        fragColor =
            vec4(phongModel(fragPosWorld, normalize(fragNormWorld)), 1.f);

    // otherwise we are looking at the back face of the fragment
    // apply color w/ flipped normal
    else
        fragColor =
            vec4(phongModel(fragPosWorld, normalize(-fragNormWorld)), 1.f);

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
