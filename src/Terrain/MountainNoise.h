#pragma once

#include <cmath>
#include <algorithm>

/**
 * @file MountainNoise.h
 * @brief Realistic alpine mountain terrain generator using Ken Perlin Gradient Noise,
 *        Kent Musgrave's Ridged Multifractal, and Organic Glacial Valley Shaping.
 *
 * Produces a spacious green valley basin around an alpine lake, surrounded by
 * rolling foothills, granite cliffs, and snow-capped mountain peaks.
 */
namespace AlpineMountain {

// Ken Perlin's reference permutation table
static const int PERM[256] = {
    151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
    8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
    35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
    134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
    55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,
    18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,
    250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
    189,28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,
    172,9,129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,
    228,251,34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,
    107,49,192,214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,
    138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

inline int p(int i) {
    return PERM[i & 255];
}

// Quintic Hermite curve: 6t^5 - 15t^4 + 10t^3
inline float fade(float t) {
    return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

inline float lerp(float t, float a, float b) {
    return a + t * (b - a);
}

inline float grad2D(int hash, float x, float y) {
    int h = hash & 7;
    float u = (h < 4) ? x : y;
    float v = (h < 4) ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

// 2D Perlin Gradient Noise
inline float perlin2D(float x, float y) {
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;

    float xf = x - std::floor(x);
    float yf = y - std::floor(y);

    float u = fade(xf);
    float v = fade(yf);

    int A = p(X) + Y;
    int B = p(X + 1) + Y;

    float g00 = grad2D(p(A), xf, yf);
    float g10 = grad2D(p(B), xf - 1.0f, yf);
    float g01 = grad2D(p(A + 1), xf, yf - 1.0f);
    float g11 = grad2D(p(B + 1), xf - 1.0f, yf - 1.0f);

    return lerp(v, lerp(u, g00, g10), lerp(u, g01, g11)) * 0.35f;
}

// Fractal Brownian Motion
inline float fbm(float x, float y, int octaves, float lacunarity = 2.0f, float gain = 0.5f) {
    float sum = 0.0f;
    float amp = 1.0f;
    float freq = 1.0f;
    float maxAmp = 0.0f;
    for (int i = 0; i < octaves; ++i) {
        sum += perlin2D(x * freq, y * freq) * amp;
        maxAmp += amp;
        freq *= lacunarity;
        amp *= gain;
    }
    return sum / maxAmp;
}

// Kent Musgrave's Ridged Multifractal Noise
inline float ridgedMF(float x, float y, int octaves, float lacunarity = 2.05f, float gain = 0.5f, float offset = 1.0f) {
    float sum = 0.0f;
    float amp = 0.5f;
    float freq = 1.0f;
    float weight = 1.0f;
    for (int i = 0; i < octaves; ++i) {
        float n = perlin2D(x * freq, y * freq);
        float signal = offset - std::abs(n);
        signal *= signal;
        signal *= weight;
        weight = std::clamp(signal * 2.0f, 0.0f, 1.0f);
        sum += signal * amp;
        freq *= lacunarity;
        amp *= gain;
    }
    return sum;
}

/**
 * @brief Samples normalized alpine terrain height at UV coordinates [0, 1].
 *        Returns height in [0.0, 1.0].
 */
inline float sampleHeight(float u, float v) {
    float nx = (u - 0.5f) * 2.0f; // [-1, 1]
    float ny = (v - 0.5f) * 2.0f; // [-1, 1]

    // Domain warp: perturb coordinates with multi-scale noise for natural mountain curves
    float w1x = fbm(u * 2.2f, v * 2.2f, 3) * 0.30f;
    float w1y = fbm(u * 2.2f + 4.3f, v * 2.2f + 2.1f, 3) * 0.30f;
    float wu = u + w1x;
    float wv = v + w1y;

    // Mountain ridges and geological features
    float r1 = ridgedMF(wu * 2.6f + 0.7f, wv * 2.6f + 1.2f, 5, 2.05f, 0.50f);
    float r2 = ridgedMF(wu * 4.8f + 3.1f, wv * 4.8f + 2.7f, 4, 2.1f, 0.45f);
    float f1 = fbm(wu * 6.5f, wv * 6.5f, 4, 2.0f, 0.45f);

    float rawMountain = r1 * 0.60f + r2 * 0.25f + (f1 * 0.5f + 0.5f) * 0.15f;

    // Organic lake footprint
    float lakeWarpX = fbm(nx * 1.2f, ny * 1.2f, 2) * 0.12f;
    float lakeWarpY = fbm(nx * 1.2f + 2.5f, ny * 1.2f + 1.5f, 2) * 0.12f;
    float lx = nx + lakeWarpX;
    float ly = ny + lakeWarpY;
    float dist = std::sqrt(lx * lx * 1.15f + ly * ly * 0.90f);

    // Spacious Alpine Valley & Foothills Profile:
    // dist < 0.22: calm lake basin (water bed)
    // 0.22 .. 0.46: wide green valley meadows & gentle foothills (lots of greenery!)
    // 0.46 .. 1.0: majestic alpine mountain peaks rising gracefully on the horizon
    float valleyMask = 0.0f;
    if (dist < 0.22f) {
        valleyMask = 0.01f * (dist / 0.22f);
    } else if (dist < 0.46f) {
        float t = (dist - 0.22f) / 0.24f;
        float ease = t * t * (3.0f - 2.0f * t);
        valleyMask = 0.01f + ease * 0.28f;
    } else {
        float t = (dist - 0.46f) / 0.54f;
        t = std::clamp(t, 0.0f, 1.0f);
        valleyMask = 0.29f + std::pow(t, 1.30f) * 0.71f;
    }

    // Northern mountains: taller peaks as scenic backdrop for Aurora
    float northBoost = (ny < 0.0f) ? (1.0f + (-ny) * 0.30f) : 1.0f;

    float height = rawMountain * valleyMask * northBoost;

    // Flatten strictly inside the central lake basin to allow clean water plane placement
    if (dist < 0.20f) {
        float t = dist / 0.20f;
        height = std::min(height, 0.005f + 0.015f * (t * t));
    }

    return std::clamp(height, 0.0f, 1.0f);
}

} // namespace AlpineMountain
