#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "SceneNode.h"
#include "../Shader/Shader.h"

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float intensity;
    float constant;
    float linear;
    float quadratic;
};

class Campfire {
public:
    Campfire(const glm::vec3& worldPos = glm::vec3(0.0f));
    ~Campfire();

    bool init();

    // Renders logs, stones, and flame geometry hierarchically
    void render(const glm::mat4& projection, const glm::mat4& view, float time);

    // Light toggling & parameters
    void toggle() { m_isActive = !m_isActive; }
    void setActive(bool active) { m_isActive = active; }
    bool isActive() const { return m_isActive; }

    PointLight getLightData(float time) const;
    glm::vec3 getPosition() const { return m_rootNode ? m_rootNode->getPosition() : m_worldPos; }
    void setPosition(const glm::vec3& pos);

private:
    void buildCampfireHierarchy();

    glm::vec3 m_worldPos;
    bool m_isActive;
    float m_flickerSpeed;

    std::shared_ptr<SceneNode> m_rootNode;
    std::shared_ptr<SceneNode> m_flameNode;

    // Shared mesh primitives
    std::shared_ptr<Mesh> m_logMesh;
    std::shared_ptr<Mesh> m_stoneMesh;
    std::shared_ptr<Mesh> m_flameMesh;
    std::shared_ptr<Mesh> m_benchMesh;

    Shader m_objectShader;
    Shader m_flameShader;
};
