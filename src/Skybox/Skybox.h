#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "../Shader/Shader.h"

class Skybox {
public:
    Skybox();
    ~Skybox();

    // Loads 6 face images or generates procedural starry cubemap faces if files are absent
    bool load(const std::vector<std::string>& faces);
    bool generateProceduralStars(int size = 512, int starCount = 3500);

    // Render the starry skybox
    void draw(const glm::mat4& view, const glm::mat4& projection);

    GLuint getTextureID() const { return m_textureID; }

private:
    void setupCube();

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_textureID;
    Shader m_shader;
};
