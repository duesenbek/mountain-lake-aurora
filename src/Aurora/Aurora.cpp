#include "Aurora.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

Aurora::Aurora()
    : m_intensity(1.15f), m_speed(0.6f) {
}

Aurora::~Aurora() {
}

void Aurora::init(int ribbonCount) {
    m_shader.load("shaders/aurora.vert", "shaders/aurora.frag");
    m_curtains.clear();

    // Configure distinct multi-layered curtains spanning across the sky
    float configs[3][5] = {
        // { yOffset, zOffset, length, height, curveAmp }
        { 38.0f, -12.0f, 130.0f, 32.0f, 14.0f },
        { 45.0f,   5.0f, 140.0f, 36.0f, 18.0f },
        { 52.0f,  24.0f, 150.0f, 40.0f, 22.0f }
    };

    for (int i = 0; i < ribbonCount && i < 3; ++i) {
        AuroraCurtain curtain;
        curtain.basePosition = glm::vec3(0.0f, configs[i][0], configs[i][1]);
        curtain.length = configs[i][2];
        curtain.height = configs[i][3];
        curtain.curveAmplitude = configs[i][4];
        curtain.timeOffset = i * 2.14f;
        curtain.speedMultiplier = 0.85f + i * 0.25f;

        curtain.mesh = generateCurvedRibbon(80, curtain.length, curtain.height, 0.045f, curtain.curveAmplitude);
        m_curtains.push_back(std::move(curtain));
    }

    std::cout << "[Aurora] Initialized " << m_curtains.size() << " Aurora Borealis curtains." << std::endl;
}

Mesh Aurora::generateCurvedRibbon(int segments, float length, float height, float curveFreq, float curveAmp) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int vertCols = segments + 1;
    int vertRows = 16; // Vertical tessellation for smooth streamer waves

    float startX = -length * 0.5f;
    float dx = length / static_cast<float>(segments);
    float dy = height / static_cast<float>(vertRows - 1);

    for (int r = 0; r < vertRows; ++r) {
        float v = static_cast<float>(r) / static_cast<float>(vertRows - 1);
        float y = v * height;

        for (int c = 0; c < vertCols; ++c) {
            float u = static_cast<float>(c) / static_cast<float>(segments);
            float x = startX + static_cast<float>(c) * dx;

            // S-curve sinuous ribbon in the XZ plane
            float z = std::sin(x * curveFreq) * curveAmp 
                    + std::cos(x * curveFreq * 0.5f) * (curveAmp * 0.4f);

            glm::vec3 pos(x, y, z);
            glm::vec3 norm(0.0f, 0.0f, 1.0f);
            glm::vec2 uv(u, v);

            vertices.emplace_back(pos, norm, uv);
        }
    }

    for (int r = 0; r < vertRows - 1; ++r) {
        for (int c = 0; c < segments; ++c) {
            unsigned int topLeft = r * vertCols + c;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (r + 1) * vertCols + c;
            unsigned int bottomRight = bottomLeft + 1;

            indices.push_back(topLeft);
            indices.push_back(bottomLeft);
            indices.push_back(topRight);

            indices.push_back(topRight);
            indices.push_back(bottomLeft);
            indices.push_back(bottomRight);
        }
    }

    return Mesh(vertices, indices);
}

void Aurora::draw(const glm::mat4& view, const glm::mat4& projection, float time) {
    if (!m_shader.isValid() || m_curtains.empty()) return;

    // Enable blending for glowing transparent curtains without depth write
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE); // Additive blending for vivid aurora luminescence
    glDepthMask(GL_FALSE);             // Don't occlude other curtains or background stars
    glDisable(GL_CULL_FACE);           // Render both sides of the curtain ribbons

    m_shader.use();
    m_shader.setMat4("view", view);
    m_shader.setMat4("projection", projection);
    m_shader.setFloat("uIntensity", m_intensity);

    for (size_t i = 0; i < m_curtains.size(); ++i) {
        auto& c = m_curtains[i];

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, c.basePosition);
        m_shader.setMat4("model", model);

        float curTime = (time + c.timeOffset) * c.speedMultiplier;
        m_shader.setFloat("uTime", curTime);
        m_shader.setFloat("uSpeed", m_speed);

        c.mesh.draw();
    }

    // Restore standard rendering state
    glEnable(GL_CULL_FACE);
    glDepthMask(GL_TRUE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
