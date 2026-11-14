#include "Terrain.h"
#include "../Vendor/stb_image.h"
#include <iostream>
#include <cmath>
#include <algorithm>

Terrain::Terrain()
    : m_resolutionX(0), m_resolutionZ(0), m_width(64.0f), m_depth(64.0f), m_maxHeight(14.0f) {
}

Terrain::~Terrain() {
}

bool Terrain::loadFromHeightmap(const std::string& heightmapPath, float width, float depth, float maxHeight) {
    int w, h, channels;
    unsigned char* data = stbi_load(heightmapPath.c_str(), &w, &h, &channels, 1);
    if (!data) {
        std::cerr << "[Terrain Warning] Failed to load heightmap: " << heightmapPath 
                  << ". Falling back to procedural generation." << std::endl;
        return generateProcedural(128, 128, width, depth, maxHeight);
    }

    m_width = width;
    m_depth = depth;
    m_maxHeight = maxHeight;
    m_resolutionX = w;
    m_resolutionZ = h;

    m_heightGrid.resize(w * h);
    for (int i = 0; i < w * h; ++i) {
        m_heightGrid[i] = static_cast<float>(data[i]) / 255.0f;
    }

    stbi_image_free(data);

    buildMeshFromHeightData(m_heightGrid, m_resolutionX, m_resolutionZ);
    std::cout << "[Terrain] Successfully loaded heightmap " << heightmapPath 
              << " (" << w << "x" << h << ")." << std::endl;
    return true;
}

bool Terrain::generateProcedural(int resolutionX, int resolutionZ, float width, float depth, float maxHeight) {
    m_width = width;
    m_depth = depth;
    m_maxHeight = maxHeight;
    m_resolutionX = resolutionX;
    m_resolutionZ = resolutionZ;

    m_heightGrid.resize(resolutionX * resolutionZ);

    float centerX = static_cast<float>(resolutionX) * 0.5f;
    float centerZ = static_cast<float>(resolutionZ) * 0.5f;
    float maxRadius = std::sqrt(centerX * centerX + centerZ * centerZ);

    for (int z = 0; z < resolutionZ; ++z) {
        for (int x = 0; x < resolutionX; ++x) {
            float nx = static_cast<float>(x) / static_cast<float>(resolutionX - 1); // [0, 1]
            float nz = static_cast<float>(z) / static_cast<float>(resolutionZ - 1); // [0, 1]

            // Distance from center (0 at lake center, 1 near map borders)
            float dx = static_cast<float>(x) - centerX;
            float dz = static_cast<float>(z) - centerZ;
            float distFromCenter = std::sqrt(dx * dx + dz * dz) / maxRadius;

            // Mountain valley ridge formula:
            // 1. Bowl shape (mountains rise as distance from center increases)
            float bowl = std::pow(distFromCenter, 1.8f);

            // 2. Harmonic mountain peaks (octaves of sinusoids)
            float peaks = 0.5f * std::sin(nx * 12.0f) * std::cos(nz * 12.0f)
                        + 0.25f * std::sin(nx * 24.0f + 1.2f) * std::cos(nz * 24.0f + 0.8f)
                        + 0.125f * std::sin(nx * 48.0f) * std::sin(nz * 48.0f);

            float h = bowl * 0.75f + peaks * 0.25f * bowl;
            h = std::clamp(h, 0.0f, 1.0f);

            // Flatten the center valley floor for the alpine lake
            if (distFromCenter < 0.22f) {
                float t = distFromCenter / 0.22f;
                h *= (t * t);
            }

            m_heightGrid[z * resolutionX + x] = h;
        }
    }

    buildMeshFromHeightData(m_heightGrid, m_resolutionX, m_resolutionZ);
    std::cout << "[Terrain] Procedural mountain valley generated (" 
              << resolutionX << "x" << resolutionZ << ")." << std::endl;
    return true;
}

void Terrain::buildMeshFromHeightData(const std::vector<float>& heightData, int resX, int resZ) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    vertices.reserve(resX * resZ);
    indices.reserve((resX - 1) * (resZ - 1) * 6);

    float halfW = m_width * 0.5f;
    float halfD = m_depth * 0.5f;
    float dx = m_width / static_cast<float>(resX - 1);
    float dz = m_depth / static_cast<float>(resZ - 1);

    for (int z = 0; z < resZ; ++z) {
        float zPos = -halfD + static_cast<float>(z) * dz;
        float v = static_cast<float>(z) / static_cast<float>(resZ - 1);

        for (int x = 0; x < resX; ++x) {
            float xPos = -halfW + static_cast<float>(x) * dx;
            float u = static_cast<float>(x) / static_cast<float>(resX - 1);

            float yPos = heightData[z * resX + x] * m_maxHeight;

            // Normal will be computed in calculateNormals()
            vertices.emplace_back(
                glm::vec3(xPos, yPos, zPos),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec2(u, v)
            );
        }
    }

    // Build index grid
    for (int z = 0; z < resZ - 1; ++z) {
        for (int x = 0; x < resX - 1; ++x) {
            unsigned int topLeft = z * resX + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * resX + x;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    calculateNormals(vertices, indices, resX, resZ);
    m_mesh.setupMesh(vertices, indices);
}

void Terrain::calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, int resX, int resZ) {
    // Reset all vertex normals
    for (auto& v : vertices) {
        v.normal = glm::vec3(0.0f);
    }

    // Accumulate triangle face normals
    for (size_t i = 0; i < indices.size(); i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        glm::vec3 v0 = vertices[i0].position;
        glm::vec3 v1 = vertices[i1].position;
        glm::vec3 v2 = vertices[i2].position;

        glm::vec3 edge1 = v1 - v0;
        glm::vec3 edge2 = v2 - v0;
        glm::vec3 faceNormal = glm::cross(edge1, edge2);

        vertices[i0].normal += faceNormal;
        vertices[i1].normal += faceNormal;
        vertices[i2].normal += faceNormal;
    }

    // Normalize each accumulated normal vector
    for (auto& v : vertices) {
        if (glm::length(v.normal) > 0.0001f) {
            v.normal = glm::normalize(v.normal);
        } else {
            v.normal = glm::vec3(0.0f, 1.0f, 0.0f);
        }
    }
}

float Terrain::getHeightAt(float worldX, float worldZ) const {
    if (m_heightGrid.empty() || m_resolutionX < 2 || m_resolutionZ < 2)
        return 0.0f;

    float halfW = m_width * 0.5f;
    float halfD = m_depth * 0.5f;

    // Convert world coordinate to local terrain grid coordinates [0, res - 1]
    float gridX = ((worldX + halfW) / m_width) * static_cast<float>(m_resolutionX - 1);
    float gridZ = ((worldZ + halfD) / m_depth) * static_cast<float>(m_resolutionZ - 1);

    if (gridX < 0.0f || gridX >= static_cast<float>(m_resolutionX - 1) ||
        gridZ < 0.0f || gridZ >= static_cast<float>(m_resolutionZ - 1)) {
        return 0.0f;
    }

    int x0 = static_cast<int>(std::floor(gridX));
    int z0 = static_cast<int>(std::floor(gridZ));
    int x1 = x0 + 1;
    int z1 = z0 + 1;

    float fracX = gridX - static_cast<float>(x0);
    float fracZ = gridZ - static_cast<float>(z0);

    float h00 = m_heightGrid[z0 * m_resolutionX + x0];
    float h10 = m_heightGrid[z0 * m_resolutionX + x1];
    float h01 = m_heightGrid[z1 * m_resolutionX + x0];
    float h11 = m_heightGrid[z1 * m_resolutionX + x1];

    // Bilinear interpolation
    float hTop = h00 * (1.0f - fracX) + h10 * fracX;
    float hBottom = h01 * (1.0f - fracX) + h11 * fracX;
    float normalizedH = hTop * (1.0f - fracZ) + hBottom * fracZ;

    return normalizedH * m_maxHeight;
}

void Terrain::draw() const {
    m_mesh.draw();
}
