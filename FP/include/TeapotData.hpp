#include <glad/glad.h>

#include <glm/vec3.hpp>

using glm::vec3;

#define us (GLushort)

constexpr size_t TEAPOT_NUM_VERTICES{(size_t)269u},
    TEAPOT_NUM_PATCHES{(size_t)28u}, PATCH_DIMENSION{(size_t)4u};

constexpr vec3 teapot_cp_vertices[TEAPOT_NUM_VERTICES]{
    // 1
    {1.4f, 0.0f, 2.4f},
    {1.4f, -0.784f, 2.4f},
    {0.784f, -1.4f, 2.4f},
    {0.0f, -1.4f, 2.4f},
    {1.3375f, 0.0f, 2.53125f},
    {1.3375f, -0.749f, 2.53125f},
    {0.749f, -1.3375f, 2.53125f},
    {0.0f, -1.3375f, 2.53125f},
    {1.4375f, 0.0f, 2.53125f},
    {1.4375f, -0.805f, 2.53125f},
    // 11
    {0.805f, -1.4375f, 2.53125f},
    {0.0f, -1.4375f, 2.53125f},
    {1.5f, 0.0f, 2.4f},
    {1.5f, -0.84f, 2.4f},
    {0.84f, -1.5f, 2.4f},
    {0.0f, -1.5f, 2.4f},
    {-0.784f, -1.4f, 2.4f},
    {-1.4f, -0.784f, 2.4f},
    {-1.4f, 0.0f, 2.4f},
    {-0.749f, -1.3375f, 2.53125f},
    // 21
    {-1.3375f, -0.749f, 2.53125f},
    {-1.3375f, 0.0f, 2.53125f},
    {-0.805f, -1.4375f, 2.53125f},
    {-1.4375f, -0.805f, 2.53125f},
    {-1.4375f, 0.0f, 2.53125f},
    {-0.84f, -1.5f, 2.4f},
    {-1.5f, -0.84f, 2.4f},
    {-1.5f, 0.0f, 2.4f},
    {-1.4f, 0.784f, 2.4f},
    {-0.784f, 1.4f, 2.4f},
    // 31
    {0.0f, 1.4f, 2.4f},
    {-1.3375f, 0.749f, 2.53125f},
    {-0.749f, 1.3375f, 2.53125f},
    {0.0f, 1.3375f, 2.53125f},
    {-1.4375f, 0.805f, 2.53125f},
    {-0.805f, 1.4375f, 2.53125f},
    {0.0f, 1.4375f, 2.53125f},
    {-1.5f, 0.84f, 2.4f},
    {-0.84f, 1.5f, 2.4f},
    {0.0f, 1.5f, 2.4f},
    // 41
    {0.784f, 1.4f, 2.4f},
    {1.4f, 0.784f, 2.4f},
    {0.749f, 1.3375f, 2.53125f},
    {1.3375f, 0.749f, 2.53125f},
    {0.805f, 1.4375f, 2.53125f},
    {1.4375f, 0.805f, 2.53125f},
    {0.84f, 1.5f, 2.4f},
    {1.5f, 0.84f, 2.4f},
    {1.75f, 0.0f, 1.875f},
    {1.75f, -0.98f, 1.875f},
    // 51
    {0.98f, -1.75f, 1.875f},
    {0.0f, -1.75f, 1.875f},
    {2.0f, 0.0f, 1.35f},
    {2.0f, -1.12f, 1.35f},
    {1.12f, -2.0f, 1.35f},
    {0.0f, -2.0f, 1.35f},
    {2.0f, 0.0f, 0.9f},
    {2.0f, -1.12f, 0.9f},
    {1.12f, -2.0f, 0.9f},
    {0.0f, -2.0f, 0.9f},
    // 61
    {-0.98f, -1.75f, 1.875f},
    {-1.75f, -0.98f, 1.875f},
    {-1.75f, 0.0f, 1.875f},
    {-1.12f, -2.0f, 1.35f},
    {-2.0f, -1.12f, 1.35f},
    {-2.0f, 0.0f, 1.35f},
    {-1.12f, -2.0f, 0.9f},
    {-2.0f, -1.12f, 0.9f},
    {-2.0f, 0.0f, 0.9f},
    {-1.75f, 0.98f, 1.875f},
    // 71
    {-0.98f, 1.75f, 1.875f},
    {0.0f, 1.75f, 1.875f},
    {-2.0f, 1.12f, 1.35f},
    {-1.12f, 2.0f, 1.35f},
    {0.0f, 2.0f, 1.35f},
    {-2.0f, 1.12f, 0.9f},
    {-1.12f, 2.0f, 0.9f},
    {0.0f, 2.0f, 0.9f},
    {0.98f, 1.75f, 1.875f},
    {1.75f, 0.98f, 1.875f},
    // 81
    {1.12f, 2.0f, 1.35f},
    {2.0f, 1.12f, 1.35f},
    {1.12f, 2.0f, 0.9f},
    {2.0f, 1.12f, 0.9f},
    {2.0f, 0.0f, 0.45f},
    {2.0f, -1.12f, 0.45f},
    {1.12f, -2.0f, 0.45f},
    {0.0f, -2.0f, 0.45f},
    {1.5f, 0.0f, 0.225f},
    {1.5f, -0.84f, 0.225f},
    // 91
    {0.84f, -1.5f, 0.225f},
    {0.0f, -1.5f, 0.225f},
    {1.5f, 0.0f, 0.15f},
    {1.5f, -0.84f, 0.15f},
    {0.84f, -1.5f, 0.15f},
    {0.0f, -1.5f, 0.15f},
    {-1.12f, -2.0f, 0.45f},
    {-2.0f, -1.12f, 0.45f},
    {-2.0f, 0.0f, 0.45f},
    {-0.84f, -1.5f, 0.225f},
    // 101
    {-1.5f, -0.84f, 0.225f},
    {-1.5f, 0.0f, 0.225f},
    {-0.84f, -1.5f, 0.15f},
    {-1.5f, -0.84f, 0.15f},
    {-1.5f, 0.0f, 0.15f},
    {-2.0f, 1.12f, 0.45f},
    {-1.12f, 2.0f, 0.45f},
    {0.0f, 2.0f, 0.45f},
    {-1.5f, 0.84f, 0.225f},
    {-0.84f, 1.5f, 0.225f},
    // 111
    {0.0f, 1.5f, 0.225f},
    {-1.5f, 0.84f, 0.15f},
    {-0.84f, 1.5f, 0.15f},
    {0.0f, 1.5f, 0.15f},
    {1.12f, 2.0f, 0.45f},
    {2.0f, 1.12f, 0.45f},
    {0.84f, 1.5f, 0.225f},
    {1.5f, 0.84f, 0.225f},
    {0.84f, 1.5f, 0.15f},
    {1.5f, 0.84f, 0.15f},
    // 121
    {-1.6f, 0.0f, 2.025f},
    {-1.6f, -0.3f, 2.025f},
    {-1.5f, -0.3f, 2.25f},
    {-1.5f, 0.0f, 2.25f},
    {-2.3f, 0.0f, 2.025f},
    {-2.3f, -0.3f, 2.025f},
    {-2.5f, -0.3f, 2.25f},
    {-2.5f, 0.0f, 2.25f},
    {-2.7f, 0.0f, 2.025f},
    {-2.7f, -0.3f, 2.025f},
    // 131
    {-3.0f, -0.3f, 2.25f},
    {-3.0f, 0.0f, 2.25f},
    {-2.7f, 0.0f, 1.8f},
    {-2.7f, -0.3f, 1.8f},
    {-3.0f, -0.3f, 1.8f},
    {-3.0f, 0.0f, 1.8f},
    {-1.5f, 0.3f, 2.25f},
    {-1.6f, 0.3f, 2.025f},
    {-2.5f, 0.3f, 2.25f},
    {-2.3f, 0.3f, 2.025f},
    // 141
    {-3.0f, 0.3f, 2.25f},
    {-2.7f, 0.3f, 2.025f},
    {-3.0f, 0.3f, 1.8f},
    {-2.7f, 0.3f, 1.8f},
    {-2.7f, 0.0f, 1.575f},
    {-2.7f, -0.3f, 1.575f},
    {-3.0f, -0.3f, 1.35f},
    {-3.0f, 0.0f, 1.35f},
    {-2.5f, 0.0f, 1.125f},
    {-2.5f, -0.3f, 1.125f},
    // 151
    {-2.65f, -0.3f, 0.9375f},
    {-2.65f, 0.0f, 0.9375f},
    {-2.0f, -0.3f, 0.9f},
    {-1.9f, -0.3f, 0.6f},
    {-1.9f, 0.0f, 0.6f},
    {-3.0f, 0.3f, 1.35f},
    {-2.7f, 0.3f, 1.575f},
    {-2.65f, 0.3f, 0.9375f},
    {-2.5f, 0.3f, 1.1255f},
    {-1.9f, 0.3f, 0.6f},
    // 161
    {-2.0f, 0.3f, 0.9f},
    {1.7f, 0.0f, 1.425f},
    {1.7f, -0.66f, 1.425f},
    {1.7f, -0.66f, 0.6f},
    {1.7f, 0.0f, 0.6f},
    {2.6f, 0.0f, 1.425f},
    {2.6f, -0.66f, 1.425f},
    {3.1f, -0.66f, 0.825f},
    {3.1f, 0.0f, 0.825f},
    {2.3f, 0.0f, 2.1f},
    // 171
    {2.3f, -0.25f, 2.1f},
    {2.4f, -0.25f, 2.025f},
    {2.4f, 0.0f, 2.025f},
    {2.7f, 0.0f, 2.4f},
    {2.7f, -0.25f, 2.4f},
    {3.3f, -0.25f, 2.4f},
    {3.3f, 0.0f, 2.4f},
    {1.7f, 0.66f, 0.6f},
    {1.7f, 0.66f, 1.425f},
    {3.1f, 0.66f, 0.825f},
    // 181
    {2.6f, 0.66f, 1.425f},
    {2.4f, 0.25f, 2.025f},
    {2.3f, 0.25f, 2.1f},
    {3.3f, 0.25f, 2.4f},
    {2.7f, 0.25f, 2.4f},
    {2.8f, 0.0f, 2.475f},
    {2.8f, -0.25f, 2.475f},
    {3.525f, -0.25f, 2.49375f},
    {3.525f, 0.0f, 2.49375f},
    {2.9f, 0.0f, 2.475f},
    // 191
    {2.9f, -0.15f, 2.475f},
    {3.45f, -0.15f, 2.5125f},
    {3.45f, 0.0f, 2.5125f},
    {2.8f, 0.0f, 2.4f},
    {2.8f, -0.15f, 2.4f},
    {3.2f, -0.15f, 2.4f},
    {3.2f, 0.0f, 2.4f},
    {3.525f, 0.25f, 2.49375f},
    {2.8f, 0.25f, 2.475f},
    {3.45f, 0.15f, 2.5125f},
    // 201
    {2.9f, 0.15f, 2.475f},
    {3.2f, 0.15f, 2.4f},
    {2.8f, 0.15f, 2.4f},
    {0.0f, 0.0f, 3.15f},
    {0.0f, -0.002f, 3.15f},
    {0.002f, 0.0f, 3.15f},
    {0.8f, 0.0f, 3.15f},
    {0.8f, -0.45f, 3.15f},
    {0.45f, -0.8f, 3.15f},
    {0.0f, -0.8f, 3.15f},
    // 211
    {0.0f, 0.0f, 2.85f},
    {0.2f, 0.0f, 2.7f},
    {0.2f, -0.112f, 2.7f},
    {0.112f, -0.2f, 2.7f},
    {0.0f, -0.2f, 2.7f},
    {-0.002f, 0.0f, 3.15f},
    {-0.45f, -0.8f, 3.15f},
    {-0.8f, -0.45f, 3.15f},
    {-0.8f, 0.0f, 3.15f},
    {-0.112f, -0.2f, 2.7f},
    // 221
    {-0.2f, -0.112f, 2.7f},
    {-0.2f, 0.0f, 2.7f},
    {0.0f, 0.002f, 3.15f},
    {-0.8f, 0.45f, 3.15f},
    {-0.45f, 0.8f, 3.15f},
    {0.0f, 0.8f, 3.15f},
    {-0.2f, 0.112f, 2.7f},
    {-0.112f, 0.2f, 2.7f},
    {0.0f, 0.2f, 2.7f},
    {0.45f, 0.8f, 3.15f},
    // 231
    {0.8f, 0.45f, 3.15f},
    {0.112f, 0.2f, 2.7f},
    {0.2f, 0.112f, 2.7f},
    {0.4f, 0.0f, 2.55f},
    {0.4f, -0.224f, 2.55f},
    {0.224f, -0.4f, 2.55f},
    {0.0f, -0.4f, 2.55f},
    {1.3f, 0.0f, 2.55f},
    {1.3f, -0.728f, 2.55f},
    {0.728f, -1.3f, 2.55f},
    // 241
    {0.0f, -1.3f, 2.55f},
    {1.3f, 0.0f, 2.4f},
    {1.3f, -0.728f, 2.4f},
    {0.728f, -1.3f, 2.4f},
    {0.0f, -1.3f, 2.4f},
    {-0.224f, -0.4f, 2.55f},
    {-0.4f, -0.224f, 2.55f},
    {-0.4f, 0.0f, 2.55f},
    {-0.728f, -1.3f, 2.55f},
    {-1.3f, -0.728f, 2.55f},
    // 251
    {-1.3f, 0.0f, 2.55f},
    {-0.728f, -1.3f, 2.4f},
    {-1.3f, -0.728f, 2.4f},
    {-1.3f, 0.0f, 2.4f},
    {-0.4f, 0.224f, 2.55f},
    {-0.224f, 0.4f, 2.55f},
    {0.0f, 0.4f, 2.55f},
    {-1.3f, 0.728f, 2.55f},
    {-0.728f, 1.3f, 2.55f},
    {0.0f, 1.3f, 2.55f},
    // 261
    {-1.3f, 0.728f, 2.4f},
    {-0.728f, 1.3f, 2.4f},
    {0.0f, 1.3f, 2.4f},
    {0.224f, 0.4f, 2.55f},
    {0.4f, 0.224f, 2.55f},
    {0.728f, 1.3f, 2.55f},
    {1.3f, 0.728f, 2.55f},
    {0.728f, 1.3f, 2.4f},
    {1.3f, 0.728f, 2.4f},
};

// no no no - *bad* formatter, behave yourself
constexpr GLushort
    teapot_patches[TEAPOT_NUM_PATCHES * PATCH_DIMENSION * PATCH_DIMENSION]{
        // rim
        us 1u, us 2u, us 3u, us 4u, us 5u, us 6u, us 7u, us 8u, us 9u, us 10u,
        us 11u, us 12u, us 13u, us 14u, us 15u, us 16u,

        us 4u, us 17u, us 18u, us 19u, us 8u, us 20u, us 21u, us 22u, us 12u,
        us 23u, us 24u, us 25u, us 16u, us 26u, us 27u, us 28u,

        us 19u, us 29u, us 30u, us 31u, us 22u, us 32u, us 33u, us 34u, us 25u,
        us 35u, us 36u, us 37u, us 28u, us 38u, us 39u, us 40u,

        us 31u, us 41u, us 42u, us 1u, us 34u, us 43u, us 44u, us 5u, us 37u,
        us 45u, us 46u, us 9u, us 40u, us 47u, us 48u, us 13u,

        // body
        us 13u, us 14u, us 15u, us 16u, us 49u, us 50u, us 51u, us 52u, us 53u,
        us 54u, us 55u, us 56u, us 57u, us 58u, us 59u, us 60u,

        us 16u, us 26u, us 27u, us 28u, us 52u, us 61u, us 62u, us 63u, us 56u,
        us 64u, us 65u, us 66u, us 60u, us 67u, us 68u, us 69u,

        us 28u, us 38u, us 39u, us 40u, us 63u, us 70u, us 71u, us 72u, us 66u,
        us 73u, us 74u, us 75u, us 69u, us 76u, us 77u, us 78u,

        us 40u, us 47u, us 48u, us 13u, us 72u, us 79u, us 80u, us 49u, us 75u,
        us 81u, us 82u, us 53u, us 78u, us 83u, us 84u, us 57u,

        us 57u, us 58u, us 59u, us 60u, us 85u, us 86u, us 87u, us 88u, us 89u,
        us 90u, us 91u, us 92u, us 93u, us 94u, us 95u, us 96u,

        us 60u, us 67u, us 68u, us 69u, us 88u, us 97u, us 98u, us 99u, us 92u,
        us 100u, us 101u, us 102u, us 96u, us 103u, us 104u, us 105u,

        us 69u, us 76u, us 77u, us 78u, us 99u, us 106u, us 107u, us 108u,
        us 102u, us 109u, us 110u, us 111u, us 105u, us 112u, us 113u, us 114u,

        us 78u, us 83u, us 84u, us 57u, us 108u, us 115u, us 116u, us 85u,
        us 111u, us 117u, us 118u, us 89u, us 114u, us 119u, us 120u, us 93u,

        // handle
        us 121u, us 122u, us 123u, us 124u, us 125u, us 126u, us 127u, us 128u,
        us 129u, us 130u, us 131u, us 132u, us 133u, us 134u, us 135u, us 136u,

        us 124u, us 137u, us 138u, us 121u, us 128u, us 139u, us 140u, us 125u,
        us 132u, us 141u, us 142u, us 129u, us 136u, us 143u, us 144u, us 133u,

        us 133u, us 134u, us 135u, us 136u, us 145u, us 146u, us 147u, us 148u,
        us 149u, us 150u, us 151u, us 152u, us 69u, us 153u, us 154u, us 155u,

        us 136u, us 143u, us 144u, us 133u, us 148u, us 156u, us 157u, us 145u,
        us 152u, us 158u, us 159u, us 149u, us 155u, us 160u, us 161u, us 69u,

        // spout
        us 162u, us 163u, us 164u, us 165u, us 166u, us 167u, us 168u, us 169u,
        us 170u, us 171u, us 172u, us 173u, us 174u, us 175u, us 176u, us 177u,

        us 165u, us 178u, us 179u, us 162u, us 169u, us 180u, us 181u, us 166u,
        us 173u, us 182u, us 183u, us 170u, us 177u, us 184u, us 185u, us 174u,

        us 174u, us 175u, us 176u, us 177u, us 186u, us 187u, us 188u, us 189u,
        us 190u, us 191u, us 192u, us 193u, us 194u, us 195u, us 196u, us 197u,

        us 177u, us 184u, us 185u, us 174u, us 189u, us 198u, us 199u, us 186u,
        us 193u, us 200u, us 201u, us 190u, us 197u, us 202u, us 203u, us 194u,

        // lid
        us 204u, us 204u, us 204u, us 204u, us 207u, us 208u, us 209u, us 210u,
        us 211u, us 211u, us 211u, us 211u, us 212u, us 213u, us 214u, us 215u,

        us 204u, us 204u, us 204u, us 204u, us 210u, us 217u, us 218u, us 219u,
        us 211u, us 211u, us 211u, us 211u, us 215u, us 220u, us 221u, us 222u,

        us 204u, us 204u, us 204u, us 204u, us 219u, us 224u, us 225u, us 226u,
        us 211u, us 211u, us 211u, us 211u, us 222u, us 227u, us 228u, us 229u,

        us 204u, us 204u, us 204u, us 204u, us 226u, us 230u, us 231u, us 207u,
        us 211u, us 211u, us 211u, us 211u, us 229u, us 232u, us 233u, us 212u,

        us 212u, us 213u, us 214u, us 215u, us 234u, us 235u, us 236u, us 237u,
        us 238u, us 239u, us 240u, us 241u, us 242u, us 243u, us 244u, us 245u,

        us 215u, us 220u, us 221u, us 222u, us 237u, us 246u, us 247u, us 248u,
        us 241u, us 249u, us 250u, us 251u, us 245u, us 252u, us 253u, us 254u,

        us 222u, us 227u, us 228u, us 229u, us 248u, us 255u, us 256u, us 257u,
        us 251u, us 258u, us 259u, us 260u, us 254u, us 261u, us 262u, us 263u,

        us 229u, us 232u, us 233u, us 212u, us 257u, us 264u, us 265u, us 234u,
        us 260u, us 266u, us 267u, us 238u, us 263u, us 268u, us 269u, us 242u

        // no bottom!
    };