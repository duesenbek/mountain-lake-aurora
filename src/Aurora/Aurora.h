#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader/Shader.h"
#include "../Mesh/Mesh.h"

struct AuroraCurtain {
    Mesh mesh;
    glm::vec3 basePosition;
    float length;
    float height;
    float curveAmplitude;
    float timeOffset;
    float speedMultiplier;
};

class Aurora {
public:
    Aurora();
    ~Aurora();

    // Initializes multi-curtain curved aurora ribbon meshes
    void init(int ribbonCount = 3);

    // Render the glowing Aurora Borealis curtains
    void draw(const glm::mat4& view, const glm::mat4& projection, float time);

    // Interactive parameters
    void setIntensity(float intensity) { m_intensity = intensity; }
    void setSpeed(float speed) { m_speed = speed; }
    float getIntensity() const { return m_intensity; }
    float getSpeed() const { return m_speed; }

private:
    Mesh generateCurvedRibbon(int segments, float length, float height, float curveFreq, float curveAmp);

    std::vector<AuroraCurtain> m_curtains;
    Shader m_shader;
    float m_intensity;
    float m_speed;
};
