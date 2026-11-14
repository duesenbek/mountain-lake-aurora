#pragma once

#include "../Vendor/stb_image_write.h"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <fstream>

namespace TextureGenerator {

// Fast integer hash for seamless lattice noise
inline float latticeHash(int x, int y, int period, int seed) {
    x = (x % period + period) % period;
    y = (y % period + period) % period;
    int n = x + y * 57 + seed * 131;
    n = (n << 13) ^ n;
    int nn = (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff;
    return 1.0f - static_cast<float>(nn) / 1073741824.0f;
}

// Seamless 2D value noise with cubic Hermite interpolation
inline float seamlessNoise(float u, float v, int period, int seed) {
    float x = u * static_cast<float>(period);
    float y = v * static_cast<float>(period);

    int iX = static_cast<int>(std::floor(x));
    int iY = static_cast<int>(std::floor(y));
    float fX = x - static_cast<float>(iX);
    float fY = y - static_cast<float>(iY);

    float sX = fX * fX * (3.0f - 2.0f * fX);
    float sY = fY * fY * (3.0f - 2.0f * fY);

    float n00 = latticeHash(iX,     iY,     period, seed);
    float n10 = latticeHash(iX + 1, iY,     period, seed);
    float n01 = latticeHash(iX,     iY + 1, period, seed);
    float n11 = latticeHash(iX + 1, iY + 1, period, seed);

    float nx0 = n00 + sX * (n10 - n00);
    float nx1 = n01 + sX * (n11 - n01);
    return nx0 + sY * (nx1 - nx0);
}

// Multi-octave fractal brownian motion
inline float seamlessFbm(float u, float v, int basePeriod, int octaves, int seed) {
    float sum = 0.0f;
    float amp = 0.5f;
    int period = basePeriod;
    for (int i = 0; i < octaves; ++i) {
        sum += amp * seamlessNoise(u, v, period, seed + i * 37);
        amp *= 0.5f;
        period *= 2;
    }
    return sum;
}

inline bool fileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

inline void generateGrassValley(const std::string& path, int size = 512) {
    if (fileExists(path)) return;
    std::vector<unsigned char> img(size * size * 3);

    for (int y = 0; y < size; ++y) {
        float v = static_cast<float>(y) / static_cast<float>(size);
        for (int x = 0; x < size; ++x) {
            float u = static_cast<float>(x) / static_cast<float>(size);

            float n1 = seamlessFbm(u, v, 8, 5, 101);
            float n2 = seamlessFbm(u, v, 16, 4, 202);
            float detail = seamlessNoise(u, v, 64, 303) * 0.15f;

            // Palette: dark pine moss, soil patches, lush alpine greenery
            float blend = std::clamp(n1 * 0.7f + n2 * 0.3f + detail, 0.0f, 1.0f);

            float r = 0.08f + 0.07f * blend;
            float g = 0.16f + 0.14f * blend;
            float b = 0.09f + 0.06f * blend;

            int idx = (y * size + x) * 3;
            img[idx + 0] = static_cast<unsigned char>(std::clamp(r * 255.0f, 0.0f, 255.0f));
            img[idx + 1] = static_cast<unsigned char>(std::clamp(g * 255.0f, 0.0f, 255.0f));
            img[idx + 2] = static_cast<unsigned char>(std::clamp(b * 255.0f, 0.0f, 255.0f));
        }
    }

    stbi_write_png(path.c_str(), size, size, 3, img.data(), size * 3);
    std::cout << "[TextureGenerator] Generated valley grass texture: " << path << std::endl;
}

inline void generateRockCliff(const std::string& path, int size = 512) {
    if (fileExists(path)) return;
    std::vector<unsigned char> img(size * size * 3);

    for (int y = 0; y < size; ++y) {
        float v = static_cast<float>(y) / static_cast<float>(size);
        for (int x = 0; x < size; ++x) {
            float u = static_cast<float>(x) / static_cast<float>(size);

            // High turbulence with sharp stratified rock layers
            float layers = std::sin((v * 16.0f + seamlessFbm(u, v, 8, 4, 404) * 2.5f) * 3.14159f);
            float rockFbm = seamlessFbm(u, v, 12, 6, 505);
            float val = std::clamp(0.5f + 0.4f * rockFbm + 0.15f * layers, 0.0f, 1.0f);

            // Cold mountain granite palette (slate grey with subtle warm mineral veins)
            float r = 0.18f + 0.22f * val;
            float g = 0.19f + 0.21f * val;
            float b = 0.22f + 0.24f * val;

            int idx = (y * size + x) * 3;
            img[idx + 0] = static_cast<unsigned char>(std::clamp(r * 255.0f, 0.0f, 255.0f));
            img[idx + 1] = static_cast<unsigned char>(std::clamp(g * 255.0f, 0.0f, 255.0f));
            img[idx + 2] = static_cast<unsigned char>(std::clamp(b * 255.0f, 0.0f, 255.0f));
        }
    }

    stbi_write_png(path.c_str(), size, size, 3, img.data(), size * 3);
    std::cout << "[TextureGenerator] Generated mountain rock texture: " << path << std::endl;
}

inline void generateSnowPeak(const std::string& path, int size = 512) {
    if (fileExists(path)) return;
    std::vector<unsigned char> img(size * size * 3);

    for (int y = 0; y < size; ++y) {
        float v = static_cast<float>(y) / static_cast<float>(size);
        for (int x = 0; x < size; ++x) {
            float u = static_cast<float>(x) / static_cast<float>(size);

            float micro = seamlessNoise(u, v, 128, 606) * 0.08f;
            float gentle = seamlessFbm(u, v, 6, 3, 707) * 0.12f;
            float val = std::clamp(0.85f + gentle + micro, 0.0f, 1.0f);

            // Pale glacial ice & alpine snow with subtle blue ambient cast
            float r = 0.78f * val;
            float g = 0.86f * val;
            float b = 0.95f * val;

            int idx = (y * size + x) * 3;
            img[idx + 0] = static_cast<unsigned char>(std::clamp(r * 255.0f, 0.0f, 255.0f));
            img[idx + 1] = static_cast<unsigned char>(std::clamp(g * 255.0f, 0.0f, 255.0f));
            img[idx + 2] = static_cast<unsigned char>(std::clamp(b * 255.0f, 0.0f, 255.0f));
        }
    }

    stbi_write_png(path.c_str(), size, size, 3, img.data(), size * 3);
    std::cout << "[TextureGenerator] Generated alpine snow texture: " << path << std::endl;
}

inline void ensureTerrainTextures(const std::string& baseDir = "assets/textures/") {
    generateGrassValley(baseDir + "grass_valley.png");
    generateRockCliff(baseDir + "rock_cliff.png");
    generateSnowPeak(baseDir + "snow_peak.png");
}

} // namespace TextureGenerator
