#pragma once

#include "../Mesh/Mesh.h"
#include "../Shader/Shader.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>

/**
 * @class Lake
 * @brief Manages the alpine lake geometry, water shader, animated waves, and rendering.
 *
 * Implements TASK-007 (Lake Geometry & Surface) with support for:
 * - Subdivided horizontal water plane mesh with vertex wave displacement
 * - Blinn-Phong specular moonlight glints
 * - Schlick Fresnel reflectance at grazing angles
 * - Semi-transparent glacial water tint
 */
class Lake {
public:
    Lake(float width = 48.0f, float depth = 44.0f, float waterLevel = 0.55f, int gridX = 64, int gridZ = 64);
    ~Lake();

    // Prevent accidental copying of OpenGL handles
    Lake(const Lake&) = delete;
    Lake& operator=(const Lake&) = delete;

    // Move semantics
    Lake(Lake&&) noexcept = default;
    Lake& operator=(Lake&&) noexcept = default;

    // Initializes lake geometry and compiles the water shader
    bool init(const std::string& vertShaderPath = "shaders/water.vert",
              const std::string& fragShaderPath = "shaders/water.frag");

    // Renders the semi-transparent reflective lake surface
    void render(const glm::mat4& projection,
                const glm::mat4& view,
                const glm::vec3& viewPos,
                const glm::vec3& lightDir,
                const glm::vec3& lightColor,
                const glm::vec3& ambientColor,
                float time,
                GLuint heightmapTextureID = 0);

    // Getters and Setters
    float getWaterLevel() const { return m_waterLevel; }
    void setWaterLevel(float level) { m_waterLevel = level; }

    float getWidth() const { return m_width; }
    float getDepth() const { return m_depth; }

    void setColors(const glm::vec3& deepColor, const glm::vec3& shallowColor) {
        m_deepColor = deepColor;
        m_shallowColor = shallowColor;
    }

private:
    float m_width;
    float m_depth;
    float m_waterLevel;
    int m_gridX;
    int m_gridZ;

    glm::vec3 m_deepColor;
    glm::vec3 m_shallowColor;

    Mesh m_mesh;
    std::unique_ptr<Shader> m_shader;
};
