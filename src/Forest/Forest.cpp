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

    // 1. Tapered Trunk with Flared Root Base and Natural Pine Bark
    const glm::vec3 barkDark(0.12f, 0.08f, 0.05f);
    const glm::vec3 barkMid(0.18f, 0.12f, 0.07f);
    const glm::vec3 barkLight(0.24f, 0.16f, 0.10f);

    struct TrunkRing {
        float y;
        float radius;
        glm::vec3 color;
    };

    std::vector<TrunkRing> trunkRings = {
        { -0.40f, 0.35f, barkDark },   // Root anchor into soil
        {  0.00f, 0.28f, barkDark },   // Ground surface
        {  1.20f, 0.22f, barkMid  },   // Lower exposed trunk
        {  2.50f, 0.17f, barkMid  },   // Mid canopy trunk
        {  4.20f, 0.12f, barkLight },  // Upper canopy trunk
        {  5.80f, 0.06f, barkLight }   // Near crown
    };

    const int trunkSegments = 8;
    for (size_t r = 0; r < trunkRings.size(); ++r) {
        float y = trunkRings[r].y;
        float rad = trunkRings[r].radius;
        glm::vec3 col = trunkRings[r].color;

        for (int i = 0; i < trunkSegments; ++i) {
            float angle = static_cast<float>(i) * 2.0f * 3.14159265f / static_cast<float>(trunkSegments);
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);
            glm::vec3 norm(cosA, 0.0f, sinA);
            glm::vec3 segCol = col * (0.88f + 0.24f * ((i % 2 == 0) ? 1.0f : 0.0f));
            vertices.push_back({ glm::vec3(cosA * rad, y, sinA * rad), norm, segCol });
        }
    }

    for (size_t r = 0; r < trunkRings.size() - 1; ++r) {
        unsigned int ringStart = static_cast<unsigned int>(r * trunkSegments);
        unsigned int nextRingStart = static_cast<unsigned int>((r + 1) * trunkSegments);

        for (int i = 0; i < trunkSegments; ++i) {
            int next = (i + 1) % trunkSegments;
            unsigned int b0 = ringStart + i;
            unsigned int b1 = ringStart + next;
            unsigned int t0 = nextRingStart + i;
            unsigned int t1 = nextRingStart + next;

            indices.push_back(b0); indices.push_back(b1); indices.push_back(t0);
            indices.push_back(t0); indices.push_back(b1); indices.push_back(t1);
        }
    }

    // 2. Realistic Jagged Multi-Star Conifer Foliage (7 Staggered Tiers)
    struct FoliageTier {
        float yAttach;     // Trunk attachment height (top of tier cone)
        float yBoughBase;   // Inner recess height
        float yDroopTip;    // Outer branch tip height (drooping under gravity)
        float outerRadius;  // Outer branch tip radius
        float innerRadius;  // Inner recess radius between branch boughs
        float rotOffset;    // Rotation offset in radians to break vertical alignment
        glm::vec3 colTip;   // Vibrant outer needle tip color
        glm::vec3 colRecess;// Shaded inner branch needle color
        glm::vec3 colBase;  // Deep underside shadow needle color
    };

    std::vector<FoliageTier> tiers = {
        // Tier 0 (Lowest, widest boughs, deep dark green, heavy droop)
        { 2.6f, 1.2f, 0.85f, 2.50f, 1.35f, 0.00f,
          glm::vec3(0.045f, 0.16f, 0.075f), glm::vec3(0.025f, 0.09f, 0.040f), glm::vec3(0.018f, 0.06f, 0.025f) },

        // Tier 1 (Lower-middle boughs)
        { 3.5f, 2.0f, 1.65f, 2.15f, 1.15f, 0.52f,
          glm::vec3(0.055f, 0.19f, 0.085f), glm::vec3(0.030f, 0.11f, 0.048f), glm::vec3(0.022f, 0.07f, 0.030f) },

        // Tier 2 (Middle boughs)
        { 4.3f, 2.8f, 2.45f, 1.80f, 0.95f, 1.04f,
          glm::vec3(0.065f, 0.22f, 0.095f), glm::vec3(0.038f, 0.13f, 0.055f), glm::vec3(0.025f, 0.08f, 0.035f) },

        // Tier 3 (Upper-middle boughs)
        { 5.1f, 3.6f, 3.25f, 1.45f, 0.78f, 1.57f,
          glm::vec3(0.075f, 0.25f, 0.110f), glm::vec3(0.045f, 0.15f, 0.065f), glm::vec3(0.028f, 0.09f, 0.040f) },

        // Tier 4 (Upper boughs)
        { 5.8f, 4.4f, 4.05f, 1.10f, 0.60f, 2.09f,
          glm::vec3(0.085f, 0.28f, 0.125f), glm::vec3(0.052f, 0.17f, 0.075f), glm::vec3(0.032f, 0.10f, 0.045f) },

        // Tier 5 (Near-crown boughs)
        { 6.4f, 5.1f, 4.80f, 0.75f, 0.42f, 2.61f,
          glm::vec3(0.095f, 0.31f, 0.135f), glm::vec3(0.060f, 0.19f, 0.085f), glm::vec3(0.038f, 0.12f, 0.050f) },

        // Tier 6 (Apical crown and leader tip, sunlit fresh green needles)
        { 7.2f, 5.8f, 5.50f, 0.45f, 0.25f, 3.14f,
          glm::vec3(0.115f, 0.35f, 0.150f), glm::vec3(0.075f, 0.23f, 0.098f), glm::vec3(0.045f, 0.14f, 0.060f) }
    };

    const int boughPoints = 12;

    for (const auto& tier : tiers) {
        unsigned int attachVertexIdx = static_cast<unsigned int>(vertices.size());
        vertices.push_back({ glm::vec3(0.0f, tier.yAttach, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), tier.colTip });

        unsigned int underVertexIdx = static_cast<unsigned int>(vertices.size());
        vertices.push_back({ glm::vec3(0.0f, tier.yBoughBase - 0.15f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), tier.colBase });

        unsigned int rimStartIdx = static_cast<unsigned int>(vertices.size());
        for (int i = 0; i < boughPoints; ++i) {
            float angle = static_cast<float>(i) * 2.0f * 3.14159265f / static_cast<float>(boughPoints) + tier.rotOffset;
            float cosA = std::cos(angle);
            float sinA = std::sin(angle);

            bool isTip = (i % 2 == 0);
            float rad = isTip ? tier.outerRadius : tier.innerRadius;
            float yPos = isTip ? tier.yDroopTip : tier.yBoughBase;
            glm::vec3 col = isTip ? tier.colTip : tier.colRecess;

            glm::vec3 normal = glm::normalize(glm::vec3(cosA * 0.75f, 0.55f, sinA * 0.75f));

            vertices.push_back({ glm::vec3(cosA * rad, yPos, sinA * rad), normal, col });
        }

        // Upper faces
        for (int i = 0; i < boughPoints; ++i) {
            int next = (i + 1) % boughPoints;
            unsigned int vCurr = rimStartIdx + i;
            unsigned int vNext = rimStartIdx + next;

            indices.push_back(attachVertexIdx);
            indices.push_back(vCurr);
            indices.push_back(vNext);
        }

        // Underside faces (deep shadow ambient occlusion under boughs)
        for (int i = 0; i < boughPoints; ++i) {
            int next = (i + 1) % boughPoints;
            unsigned int vCurr = rimStartIdx + i;
            unsigned int vNext = rimStartIdx + next;

            indices.push_back(underVertexIdx);
            indices.push_back(vNext);
            indices.push_back(vCurr);
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
    std::uniform_real_distribution<float> distScale(0.75f, 1.45f);
    std::uniform_real_distribution<float> distRot(0.0f, 360.0f);
    std::uniform_real_distribution<float> distTilt(-2.5f, 2.5f);

    int tries = 0;
    while (static_cast<int>(m_instanceMatrices.size()) < treeCount && tries < 4500) {
        tries++;
        float angle = distAngle(rng);
        float r = distRadius(rng);
        float x = r * std::cos(angle);
        float z = r * std::sin(angle);

        // Check distance to campfire (keep open scenic meadow around camp)
        float dToFire = std::hypot(x - campfirePos.x, z - campfirePos.z);
        if (dToFire < 7.5f) continue;

        // Sample terrain height
        float y = terrain.getHeightAt(x, z);

        // Foothill constraints:
        // Must be on dry ground well above lake level (y > 1.8m)
        // Must be below high barren rocky peaks (y < 24.0m)
        if (y < 1.8f || y > 24.0f) continue;

        float scale = distScale(rng);
        float rotY = distRot(rng);
        float tiltX = distTilt(rng);
        float tiltZ = distTilt(rng);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y - 0.15f, z)); // Sink root flare into soil
        model = glm::rotate(model, glm::radians(rotY), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(tiltX), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(tiltZ), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(scale, scale * 1.05f, scale));

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
