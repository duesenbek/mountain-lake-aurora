#pragma once

#include "../Mesh/Mesh.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>

class Terrain {
public:
    Terrain();
    ~Terrain();

    // Loads a grayscale heightmap from disk and constructs the 3D mountain terrain mesh
    bool loadFromHeightmap(const std::string& heightmapPath,
                          float width = 64.0f,
                          float depth = 64.0f,
                          float maxHeight = 14.0f);

    // Generates procedural mountain valley heightmap if no external file is supplied
    bool generateProcedural(int resolutionX = 128,
                            int resolutionZ = 128,
                            float width = 64.0f,
                            float depth = 64.0f,
                            float maxHeight = 14.0f);

    // Renders the terrain mesh
    void draw() const;

    // Returns the terrain height at arbitrary world coordinates (X, Z) via bilinear interpolation
    float getHeightAt(float worldX, float worldZ) const;

    float getWidth() const { return m_width; }
    float getDepth() const { return m_depth; }
    float getMaxHeight() const { return m_maxHeight; }
    int getResolutionX() const { return m_resolutionX; }
    int getResolutionZ() const { return m_resolutionZ; }

private:
    void buildMeshFromHeightData(const std::vector<float>& heightData, int resX, int resZ);
    void calculateNormals(std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices, int resX, int resZ);

    Mesh m_mesh;
    std::vector<float> m_heightGrid; // Normalized heights [0, 1] for fast collision/query
    int m_resolutionX;
    int m_resolutionZ;
    float m_width;
    float m_depth;
    float m_maxHeight;
};
