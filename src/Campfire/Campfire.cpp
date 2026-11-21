#include "Campfire.h"
#include <cmath>
#include <iostream>

Campfire::Campfire(const glm::vec3& worldPos)
    : m_worldPos(worldPos),
      m_isActive(true),
      m_flickerSpeed(1.0f) {
}

Campfire::~Campfire() {
}

bool Campfire::init() {
    m_objectShader.load("shaders/basic.vert", "shaders/basic.frag");
    m_flameShader.load("shaders/flame.vert", "shaders/flame.frag");

    // Shared primitive meshes
    m_logMesh = std::make_shared<Mesh>(Mesh::createCube(1.0f));
    m_stoneMesh = std::make_shared<Mesh>(Mesh::createCube(1.0f));
    m_flameMesh = std::make_shared<Mesh>(Mesh::createCube(1.0f));
    m_benchMesh = std::make_shared<Mesh>(Mesh::createCube(1.0f));

    buildCampfireHierarchy();
    std::cout << "[Campfire] Hierarchical campfire model initialized." << std::endl;
    return true;
}

void Campfire::setPosition(const glm::vec3& pos) {
    m_worldPos = pos;
    if (m_rootNode) {
        m_rootNode->setPosition(pos);
    }
}

void Campfire::buildCampfireHierarchy() {
    // 1. Root Parent Node (The Campfire Assembly)
    m_rootNode = std::make_shared<SceneNode>("CampfireRoot");
    m_rootNode->setPosition(m_worldPos);

    // 2. Child Nodes: Circular Stone Ring around the fire pit
    const int numStones = 8;
    const float ringRadius = 1.05f;
    for (int i = 0; i < numStones; ++i) {
        float angle = glm::radians(static_cast<float>(i) * (360.0f / numStones));
        auto stone = std::make_shared<SceneNode>("Stone_" + std::to_string(i));
        stone->setMesh(m_stoneMesh);
        stone->setPosition(glm::vec3(std::cos(angle) * ringRadius, 0.12f, std::sin(angle) * ringRadius));
        stone->setScale(glm::vec3(0.35f, 0.25f, 0.35f));
        stone->setRotation(glm::vec3(5.0f, static_cast<float>(i * 45), -5.0f));
        stone->setColor(glm::vec4(0.25f, 0.27f, 0.30f, 1.0f)); // Granite charcoal stone
        m_rootNode->addChild(stone);
    }

    // 3. Child Nodes: Cross-stacked Wooden Fire Logs
    float logRotations[4] = { 25.0f, 115.0f, 65.0f, 155.0f };
    float logHeights[4]   = { 0.12f, 0.18f, 0.24f, 0.28f };
    for (int i = 0; i < 4; ++i) {
        auto log = std::make_shared<SceneNode>("Log_" + std::to_string(i));
        log->setMesh(m_logMesh);
        log->setPosition(glm::vec3(0.0f, logHeights[i], 0.0f));
        log->setRotation(glm::vec3(12.0f, logRotations[i], 0.0f));
        log->setScale(glm::vec3(1.5f, 0.20f, 0.22f));
        log->setColor(glm::vec4(0.35f, 0.20f, 0.10f, 1.0f)); // Burnt pine bark
        m_rootNode->addChild(log);
    }

    // 4. Child Node: Animated Glowing Flame Mesh
    m_flameNode = std::make_shared<SceneNode>("FlameCore");
    m_flameNode->setMesh(m_flameMesh);
    m_flameNode->setPosition(glm::vec3(0.0f, 0.55f, 0.0f));
    m_flameNode->setScale(glm::vec3(0.55f, 1.1f, 0.55f));
    m_rootNode->addChild(m_flameNode);

    // 5. Child Nodes: Rustic Log Benches / Camp Stools around the fire
    // Log stool 1 (Lake-facing / south)
    auto bench1 = std::make_shared<SceneNode>("Bench_South");
    bench1->setMesh(m_benchMesh);
    bench1->setPosition(glm::vec3(0.0f, 0.22f, 2.1f));
    bench1->setRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    bench1->setScale(glm::vec3(1.6f, 0.32f, 0.45f));
    bench1->setColor(glm::vec4(0.28f, 0.16f, 0.08f, 1.0f)); // Carved timber wood
    m_rootNode->addChild(bench1);

    // Log stool 2 (West side)
    auto bench2 = std::make_shared<SceneNode>("Bench_West");
    bench2->setMesh(m_benchMesh);
    bench2->setPosition(glm::vec3(-2.1f, 0.22f, 0.0f));
    bench2->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    bench2->setScale(glm::vec3(1.6f, 0.32f, 0.45f));
    bench2->setColor(glm::vec4(0.30f, 0.17f, 0.09f, 1.0f));
    m_rootNode->addChild(bench2);

    // Log stool 3 (East side)
    auto bench3 = std::make_shared<SceneNode>("Bench_East");
    bench3->setMesh(m_benchMesh);
    bench3->setPosition(glm::vec3(2.1f, 0.22f, 0.0f));
    bench3->setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
    bench3->setScale(glm::vec3(1.6f, 0.32f, 0.45f));
    bench3->setColor(glm::vec4(0.29f, 0.16f, 0.08f, 1.0f));
    m_rootNode->addChild(bench3);
}

PointLight Campfire::getLightData(float time) const {
    PointLight light;
    glm::vec3 currentRootPos = m_rootNode ? m_rootNode->getPosition() : m_worldPos;
    light.position = currentRootPos + glm::vec3(0.0f, 0.75f, 0.0f);

    if (!m_isActive) {
        light.intensity = 0.0f;
        light.color = glm::vec3(0.0f);
        return light;
    }

    // Dynamic flame flickering using multi-frequency sinusoids
    float flicker = std::sin(time * 16.0f) * 0.18f 
                  + std::cos(time * 26.0f) * 0.12f 
                  + std::sin(time * 9.0f)  * 0.14f;
    light.intensity = std::max(0.65f + flicker, 0.2f);
    light.color = glm::vec3(1.0f, 0.52f, 0.15f); // Warm incandescent firelight

    // Attenuation coefficients (range ~ 32m)
    light.constant = 1.0f;
    light.linear = 0.09f;
    light.quadratic = 0.032f;

    return light;
}

void Campfire::render(const glm::mat4& projection, const glm::mat4& view, float time) {
    if (!m_rootNode) return;

    // 1. Render Physical Structure (Stones & Logs) using Hierarchy
    m_objectShader.use();
    m_objectShader.setMat4("projection", projection);
    m_objectShader.setMat4("view", view);

    // Draw logs and stones with default model transform
    m_rootNode->draw(m_objectShader);

    // 2. Render Glowing Flame
    if (m_isActive && m_flameShader.isValid()) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive luminous flame
        glDepthMask(GL_FALSE);

        m_flameShader.use();
        m_flameShader.setMat4("projection", projection);
        m_flameShader.setMat4("view", view);

        // Get world transform of flame through parent hierarchy
        glm::mat4 flameWorld = m_flameNode->getWorldTransform();

        // Pulsate flame scale with flickering
        float flicker = std::sin(time * 15.0f) * 0.06f;
        flameWorld = glm::scale(flameWorld, glm::vec3(1.0f + flicker, 1.0f + flicker * 2.0f, 1.0f + flicker));

        m_flameShader.setMat4("model", flameWorld);
        m_flameShader.setFloat("uTime", time);
        m_flameShader.setFloat("uActive", m_isActive ? 1.0f : 0.0f);

        m_flameMesh->draw();

        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}
