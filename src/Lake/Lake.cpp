#include "Lake.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

Lake::Lake(float width, float depth, float waterLevel, int gridX, int gridZ)
    : m_width(width),
      m_depth(depth),
      m_waterLevel(waterLevel),
      m_gridX(gridX),
      m_gridZ(gridZ),
      m_deepColor(glm::vec3(0.02f, 0.08f, 0.18f)),
      m_shallowColor(glm::vec3(0.06f, 0.24f, 0.36f)) {
}

Lake::~Lake() {
}

bool Lake::init(const std::string& vertShaderPath, const std::string& fragShaderPath) {
    // Construct subdivided water plane mesh for smooth wave animation and specular reflections
    m_mesh = Mesh::createPlane(m_width, m_depth, m_gridX, m_gridZ);

    m_shader = std::make_unique<Shader>(vertShaderPath, fragShaderPath);
    if (!m_shader->isValid()) {
        std::cerr << "[Lake Error] Failed to compile water shaders!" << std::endl;
        return false;
    }

    std::cout << "[Lake] Initialized lake surface (" << m_width << "x" << m_depth 
              << "m, " << m_gridX << "x" << m_gridZ << " quads) at elevation y=" 
              << m_waterLevel << "m." << std::endl;
    return true;
}

void Lake::render(const glm::mat4& projection,
                  const glm::mat4& view,
                  const glm::vec3& viewPos,
                  const glm::vec3& lightDir,
                  const glm::vec3& lightColor,
                  const glm::vec3& ambientColor,
                  float time) {
    if (!m_shader || !m_shader->isValid()) return;

    // Enable alpha blending for semi-transparent water surface
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_shader->use();
    m_shader->setMat4("projection", projection);
    m_shader->setMat4("view", view);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, m_waterLevel, 0.0f));
    m_shader->setMat4("model", model);

    m_shader->setFloat("uTime", time);
    m_shader->setVec3("uViewPos", viewPos);
    m_shader->setVec3("uLightDir", lightDir);
    m_shader->setVec3("uLightColor", lightColor);
    m_shader->setVec3("uAmbientColor", ambientColor);
    m_shader->setVec3("uDeepColor", m_deepColor);
    m_shader->setVec3("uShallowColor", m_shallowColor);

    m_mesh.draw();

    glDisable(GL_BLEND);
}
