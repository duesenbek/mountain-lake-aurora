#include "Skybox.h"
#include "../Vendor/stb_image.h"
#include <iostream>
#include <random>
#include <cmath>

Skybox::Skybox()
    : m_vao(0), m_vbo(0), m_textureID(0) {
    setupCube();
    m_shader.load("shaders/skybox.vert", "shaders/skybox.frag");
}

Skybox::~Skybox() {
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
}

void Skybox::setupCube() {
    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
}

bool Skybox::load(const std::vector<std::string>& faces) {
    if (faces.size() < 6) return false;

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        } else {
            std::cerr << "[Skybox] Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
            return false;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

bool Skybox::generateProceduralStars(int size, int starCount) {
    if (m_textureID != 0) {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }

    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);

    std::mt19937 rng(1337);
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
    std::uniform_int_distribution<int> coordDist(0, size - 1);

    // Deep starry midnight gradient with bright sparkling stars
    for (unsigned int face = 0; face < 6; ++face) {
        std::vector<unsigned char> faceData(size * size * 3, 0);

        for (int y = 0; y < size; ++y) {
            for (int x = 0; x < size; ++x) {
                int idx = (y * size + x) * 3;

                // Subtle deep cosmic blue background gradient
                float grad = (face == 2) ? 0.05f : ((face == 3) ? 0.005f : (1.0f - static_cast<float>(y) / size) * 0.035f);
                faceData[idx + 0] = static_cast<unsigned char>(3 + grad * 8.0f);   // Red
                faceData[idx + 1] = static_cast<unsigned char>(6 + grad * 16.0f);  // Green
                faceData[idx + 2] = static_cast<unsigned char>(14 + grad * 32.0f); // Blue
            }
        }

        // Scatter stars across the sky faces (omit bottom face = 3)
        if (face != 3) {
            int faceStars = (face == 2) ? starCount * 2 : starCount; // Top face has dense zenith stars
            for (int s = 0; s < faceStars; ++s) {
                int sx = coordDist(rng);
                int sy = coordDist(rng);
                int sidx = (sy * size + sx) * 3;

                float brightness = dist01(rng);
                unsigned char starCol = (brightness > 0.85f) ? 255 : (160 + static_cast<unsigned char>(brightness * 90.0f));

                // Star center
                faceData[sidx + 0] = std::max(faceData[sidx + 0], (brightness > 0.92f) ? static_cast<unsigned char>(210) : starCol);
                faceData[sidx + 1] = std::max(faceData[sidx + 1], starCol);
                faceData[sidx + 2] = 255; // White-blue star

                // 4-pixel twinkle bloom for bright stars
                if (brightness > 0.90f) {
                    int neighbors[4][2] = { {sx+1, sy}, {sx-1, sy}, {sx, sy+1}, {sx, sy-1} };
                    for (auto& n : neighbors) {
                        if (n[0] >= 0 && n[0] < size && n[1] >= 0 && n[1] < size) {
                            int nidx = (n[1] * size + n[0]) * 3;
                            faceData[nidx + 0] = std::max(faceData[nidx + 0], (unsigned char)90);
                            faceData[nidx + 1] = std::max(faceData[nidx + 1], (unsigned char)120);
                            faceData[nidx + 2] = std::max(faceData[nidx + 2], (unsigned char)170);
                        }
                    }
                }
            }
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + face,
                     0, GL_RGB, size, size, 0, GL_RGB, GL_UNSIGNED_BYTE, faceData.data());
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    std::cout << "[Skybox] Procedural starry night cubemap generated successfully (" 
              << size << "x" << size << "x6 faces)." << std::endl;
    return true;
}

void Skybox::draw(const glm::mat4& view, const glm::mat4& projection) {
    if (m_vao == 0 || m_textureID == 0 || !m_shader.isValid()) return;

    // Use GL_LEQUAL depth test so skybox renders precisely at furthest depth (1.0)
    glDepthFunc(GL_LEQUAL);

    m_shader.use();
    m_shader.setMat4("view", view);
    m_shader.setMat4("projection", projection);
    m_shader.setVec3("uNightTint", glm::vec3(0.95f, 1.0f, 1.15f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureID);
    m_shader.setInt("skybox", 0);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    // Reset default depth test
    glDepthFunc(GL_LESS);
}
