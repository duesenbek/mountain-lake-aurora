#include "Forest.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <random>
#include <cmath>

Forest::Forest()
    : m_vao(0), m_vbo(0), m_ebo(0), m_instanceVBO(0), m_indexCount(0) {
}

Forest::~Forest() {
    cleanup();
}

void Forest::cleanup() {
    if (m_instanceVBO) { glDeleteBuffers(1, &m_instanceVBO); m_instanceVBO = 0; }
    if (m_ebo) { glDeleteBuffers(1, &m_ebo); m_ebo = 0; }
    if (m_vbo) { glDeleteBuffers(1, &m_vbo); m_vbo = 0; }
    if (m_vao) { glDeleteVertexArrays(1, &m_vao); m_vao = 0; }
}

void Forest::buildPineTreeGeometry(std::vector<TreeVertex>& vertices, std::vector<unsigned int>& indices) {
    vertices.clear();
    indices.clear();

    // 1. Tree Trunk (Dark wooden bark)
    const glm::vec3 barkColor(0.18f, 0.11f, 0.06f);
    const int trunkSegments = 6;
    const float trunkRadiusBottom = 0.22f;
    const float trunkRadiusTop = 0.12f;
    const float trunkHeight = 2.8f;

    unsigned int trunkBaseIdx = vertices.size();
    for (int i = 0; i < trunkSegments; ++i) {
        float angle = static_cast<float>(i) * 2.0f * 3.14159265f / static_cast<float>(trunkSegments);
        float cosA = std::cos(angle);
        float sinA = std::sin(angle);
        glm::vec3 normal(cosA, 0.0f, sinA);

        vertices.push_back({ glm::vec3(cosA * trunkRadiusBottom, 0.0f, sinA * trunkRadiusBottom), normal, barkColor });
        vertices.push_back({ glm::vec3(cosA * trunkRadiusTop, trunkHeight, sinA * trunkRadiusTop), normal, barkColor });
    }

    for (int i = 0; i < trunkSegments; ++i) {
        int next = (i + 1) % trunkSegments;
        unsigned int b0 = trunkBaseIdx + i * 2;
        unsigned int t0 = b0 + 1;
        unsigned int b1 = trunkBaseIdx + next * 2;
        unsigned int t1 = b1 + 1;

        indices.push_back(b0); indices.push_back(b1); indices.push_back(t0);
        indices.push_back(t0); indices.push_back(b1); indices.push_back(t1);
    }

    // 2. Foliage (4 tiered conical layers of pine needles)
    struct Tier {
        float yBottom;
        float yTop;
        float radius;
        glm::vec3 color;
    };

    std::vector<Tier> tiers = {
        { 1.0f, 3.4f, 1.85f, glm::vec3(0.045f, 0.16f, 0.075f) }, // Lower deep needles
        { 2.2f, 4.6f, 1.45f, glm::vec3(0.055f, 0.20f, 0.090f) }, // Middle needles
        { 3.4f, 5.8f, 1.05f, glm::vec3(0.070f, 0.24f, 0.105f) }, // Upper needles
        { 4.6f, 6.8f, 0.65f, glm::vec3(0.090f, 0.28f, 0.125f) }  // Crown tip
    };

    const int coneSegments = 8;
    for (const auto& tier : tiers) {
        unsigned int tierBase = vertices.size();
        // Tip vertex
        vertices.push_back({ glm::vec3(0.0f, tier.yTop, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), tier.color });

        // Base rim vertices
        for (int i = 0; i < coneSegments; ++i) {
            float angle = static_cast<float>(i) * 2.0f * 3.14159265f / static_cast<float>(coneSegments);
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            glm::vec3 normal = glm::normalize(glm::vec3(cosA * 0.8f, 0.4f, sinA * 0.8f));

            vertices.push_back({ glm::vec3(cosA * tier.radius, tier.yBottom, sinA * tier.radius), normal, tier.color });
        }

        // Rim triangles
        for (int i = 0; i < coneSegments; ++i) {
            int next = (i + 1) % coneSegments;
            indices.push_back(tierBase);
            indices.push_back(tierBase + 1 + i);
            indices.push_back(tierBase + 1 + next);
        }
    }

    m_indexCount = indices.size();
}

void Forest::setupBuffers(const std::vector<TreeVertex>& vertices, const std::vector<unsigned int>& indices) {
    cleanup();

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);
    glGenBuffers(1, &m_instanceVBO);

    glBindVertexArray(m_vao);

    // 1. Vertex Buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(TreeVertex), vertices.data(), GL_STATIC_DRAW);

    // 2. Element Buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position (Location 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(TreeVertex), (void*)offsetof(TreeVertex, pos));

    // Normal (Location 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(TreeVertex), (void*)offsetof(TreeVertex, normal));

    // Color (Location 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(TreeVertex), (void*)offsetof(TreeVertex, color));

    // 3. Instance Matrix Buffer (Locations 3, 4, 5, 6)
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferData(GL_ARRAY_BUFFER, m_instanceMatrices.size() * sizeof(glm::mat4), m_instanceMatrices.data(), GL_STATIC_DRAW);

    std::size_t vec4Size = sizeof(glm::vec4);
    for (unsigned int i = 0; i < 4; ++i) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * vec4Size));
        glVertexAttribDivisor(3 + i, 1); // 1 per instance
    }

    glBindVertexArray(0);
}

bool Forest::init(const Terrain& terrain,
                  int treeCount,
                  float minRadius,
                  float maxRadius,
                  const glm::vec3& campfirePos) {
    std::vector<TreeVertex> vertices;
    std::vector<unsigned int> indices;
    buildPineTreeGeometry(vertices, indices);

    // Compile tree shader
    m_shader = std::make_unique<Shader>("shaders/tree.vert", "shaders/tree.frag");
    if (!m_shader->isValid()) {
        std::cerr << "[Forest Error] Failed to compile tree shaders!" << std::endl;
        return false;
    }

    // Generate instance transformation matrices along mountain foothills
    m_instanceMatrices.clear();
    m_instanceMatrices.reserve(treeCount);

    std::mt19937 rng(54321); // Deterministic seed
    std::uniform_real_distribution<float> distAngle(0.0f, 6.2831853f);
    std::uniform_real_distribution<float> distRadius(minRadius, maxRadius);
    std::uniform_real_distribution<float> distScale(0.85f, 1.40f);
    std::uniform_real_distribution<float> distRot(0.0f, 360.0f);

    int tries = 0;
    while (static_cast<int>(m_instanceMatrices.size()) < treeCount && tries < 2000) {
        tries++;
        float angle = distAngle(rng);
        float r = distRadius(rng);
        float x = r * std::cos(angle);
        float z = r * std::sin(angle);

        // Check distance to campfire (keep clear opening around camp)
        float dToFire = std::hypot(x - campfirePos.x, z - campfirePos.z);
        if (dToFire < 6.5f) continue;

        // Sample terrain height
        float y = terrain.getHeightAt(x, z);

        // Foothill constraints:
        // Must be on dry ground well above lake level (y > 1.6m)
        // Must be below high barren rocky peaks (y < 22.0m)
        if (y < 1.6f || y > 22.0f) continue;

        float scale = distScale(rng);
        float rotY = distRot(rng);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y - 0.1f, z)); // Slightly sink trunk base into soil
        model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(scale, scale * 1.1f, scale));

        m_instanceMatrices.push_back(model);
    }

    setupBuffers(vertices, indices);

    std::cout << "[Forest] Successfully generated boreal pine forest (" 
              << m_instanceMatrices.size() << " instanced trees) along mountain foothills." << std::endl;
    return true;
}

void Forest::render(const glm::mat4& projection,
                    const glm::mat4& view,
                    const glm::vec3& lightDir,
                    const glm::vec3& lightColor,
                    const glm::vec3& ambientColor,
                    bool moonActive,
                    const PointLight& pLight,
                    float time) {
    if (!m_shader || !m_shader->isValid() || m_instanceMatrices.empty()) return;

    m_shader->use();
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);
    m_shader->setFloat("uTime", time);

    // Moonlight
    m_shader->setVec3("uLightDir", lightDir);
    m_shader->setVec3("uLightColor", lightColor);
    m_shader->setVec3("uAmbientColor", ambientColor);
    m_shader->setFloat("uMoonActive", moonActive ? 1.0f : 0.0f);

    // Campfire Point Light
    m_shader->setVec3("uPointLightPos", pLight.position);
    m_shader->setVec3("uPointLightColor", pLight.color);
    m_shader->setFloat("uPointLightIntensity", pLight.intensity);
    m_shader->setFloat("uPointLightConst", pLight.constant);
    m_shader->setFloat("uPointLightLinear", pLight.linear);
    m_shader->setFloat("uPointLightQuad", pLight.quadratic);

    glBindVertexArray(m_vao);
    glDrawElementsInstanced(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0, static_cast<GLsizei>(m_instanceMatrices.size()));
    glBindVertexArray(0);
}
