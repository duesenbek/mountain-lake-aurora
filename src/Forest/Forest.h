#pragma once

#include <vector>
#include <memory>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader/Shader.h"
#include "../Terrain/Terrain.h"
#include "../Campfire/Campfire.h"

/**
 * @struct TreeVertex
 * @brief Vertex format for procedural pine trees (position, normal, color).
 */
struct TreeVertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec3 color;
};

/**
 * @class Forest
 * @brief Manages procedural pine tree geometry, foothill distribution, and single-pass
 *        GPU instanced rendering (TASK-018: Instanced Pine Trees).
 */
class Forest {
public:
    Forest();
    ~Forest();

    // Prevent accidental copying of OpenGL handles
    Forest(const Forest&) = delete;
    Forest& operator=(const Forest&) = delete;

    // Initializes pine tree mesh, generates instances around the mountain foothills,
    // and compiles the wind-animated tree shader
    bool init(const Terrain& terrain,
              int treeCount = 280,
              float minRadius = 32.0f,
              float maxRadius = 78.0f,
              const glm::vec3& campfirePos = glm::vec3(-32.0f, 5.1f, -19.0f));

    // Renders all pine trees in a single GPU draw call using instanced rendering
    void render(const glm::mat4& projection,
                const glm::mat4& view,
                const glm::vec3& lightDir,
                const glm::vec3& lightColor,
                const glm::vec3& ambientColor,
                bool moonActive,
                const PointLight& pLight,
                float time);

    size_t getInstanceCount() const { return m_instanceMatrices.size(); }

private:
    void buildPineTreeGeometry(std::vector<TreeVertex>& vertices, std::vector<unsigned int>& indices);
    void setupBuffers(const std::vector<TreeVertex>& vertices, const std::vector<unsigned int>& indices);
    void cleanup();

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    GLuint m_instanceVBO;
    size_t m_indexCount;

    std::vector<glm::mat4> m_instanceMatrices;
    std::unique_ptr<Shader> m_shader;
};
