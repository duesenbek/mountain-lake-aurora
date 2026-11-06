#include "Mesh.h"
#include <iostream>

Mesh::Mesh()
    : m_vao(0), m_vbo(0), m_ebo(0), m_vertexCount(0), m_indexCount(0) {
}

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : m_vao(0), m_vbo(0), m_ebo(0), m_vertexCount(0), m_indexCount(0) {
    setupMesh(vertices, indices);
}

Mesh::~Mesh() {
    cleanup();
}

Mesh::Mesh(Mesh&& other) noexcept
    : m_vao(other.m_vao), m_vbo(other.m_vbo), m_ebo(other.m_ebo),
      m_vertexCount(other.m_vertexCount), m_indexCount(other.m_indexCount) {
    other.m_vao = 0;
    other.m_vbo = 0;
    other.m_ebo = 0;
    other.m_vertexCount = 0;
    other.m_indexCount = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        m_vao = other.m_vao;
        m_vbo = other.m_vbo;
        m_ebo = other.m_ebo;
        m_vertexCount = other.m_vertexCount;
        m_indexCount = other.m_indexCount;

        other.m_vao = 0;
        other.m_vbo = 0;
        other.m_ebo = 0;
        other.m_vertexCount = 0;
        other.m_indexCount = 0;
    }
    return *this;
}

void Mesh::cleanup() {
    if (m_ebo != 0) {
        glDeleteBuffers(1, &m_ebo);
        m_ebo = 0;
    }
    if (m_vbo != 0) {
        glDeleteBuffers(1, &m_vbo);
        m_vbo = 0;
    }
    if (m_vao != 0) {
        glDeleteVertexArrays(1, &m_vao);
        m_vao = 0;
    }
    m_vertexCount = 0;
    m_indexCount = 0;
}

void Mesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    cleanup();

    m_vertexCount = vertices.size();
    m_indexCount = indices.size();

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glGenBuffers(1, &m_ebo);

    glBindVertexArray(m_vao);

    // VBO - Vertex data
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    // EBO - Index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Attribute 0: Position (location = 0)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // Attribute 1: Normal (location = 1)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Attribute 2: Texture Coordinates (location = 2)
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

    glBindVertexArray(0);
}

void Mesh::draw() const {
    if (m_vao == 0 || m_indexCount == 0) return;

    glBindVertexArray(m_vao);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indexCount), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Mesh Mesh::createCube(float size) {
    float h = size * 0.5f;

    std::vector<Vertex> vertices = {
        // Front face (Normal: 0, 0, 1)
        Vertex(glm::vec3(-h, -h,  h), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( h, -h,  h), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( h,  h,  h), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-h,  h,  h), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f)),

        // Back face (Normal: 0, 0, -1)
        Vertex(glm::vec3( h, -h, -h), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(-h, -h, -h), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(-h,  h, -h), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3( h,  h, -h), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)),

        // Left face (Normal: -1, 0, 0)
        Vertex(glm::vec3(-h, -h, -h), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3(-h, -h,  h), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3(-h,  h,  h), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-h,  h, -h), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),

        // Right face (Normal: 1, 0, 0)
        Vertex(glm::vec3( h, -h,  h), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( h, -h, -h), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( h,  h, -h), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3( h,  h,  h), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)),

        // Top face (Normal: 0, 1, 0)
        Vertex(glm::vec3(-h,  h,  h), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( h,  h,  h), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( h,  h, -h), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-h,  h, -h), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f)),

        // Bottom face (Normal: 0, -1, 0)
        Vertex(glm::vec3(-h, -h, -h), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)),
        Vertex(glm::vec3( h, -h, -h), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)),
        Vertex(glm::vec3( h, -h,  h), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
        Vertex(glm::vec3(-h, -h,  h), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f))
    };

    std::vector<unsigned int> indices = {
        0, 1, 2,  2, 3, 0,       // Front
        4, 5, 6,  6, 7, 4,       // Back
        8, 9, 10, 10, 11, 8,     // Left
        12, 13, 14, 14, 15, 12,  // Right
        16, 17, 18, 18, 19, 16,  // Top
        20, 21, 22, 22, 23, 20   // Bottom
    };

    return Mesh(vertices, indices);
}

Mesh Mesh::createPlane(float width, float depth, int gridX, int gridZ) {
    if (gridX < 1) gridX = 1;
    if (gridZ < 1) gridZ = 1;

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float halfW = width * 0.5f;
    float halfD = depth * 0.5f;

    float dx = width / static_cast<float>(gridX);
    float dz = depth / static_cast<float>(gridZ);

    for (int z = 0; z <= gridZ; ++z) {
        float zPos = -halfD + static_cast<float>(z) * dz;
        float v = static_cast<float>(z) / static_cast<float>(gridZ);

        for (int x = 0; x <= gridX; ++x) {
            float xPos = -halfW + static_cast<float>(x) * dx;
            float u = static_cast<float>(x) / static_cast<float>(gridX);

            vertices.emplace_back(
                glm::vec3(xPos, 0.0f, zPos),
                glm::vec3(0.0f, 1.0f, 0.0f),
                glm::vec2(u, v)
            );
        }
    }

    int stride = gridX + 1;
    for (int z = 0; z < gridZ; ++z) {
        for (int x = 0; x < gridX; ++x) {
            unsigned int topLeft = z * stride + x;
            unsigned int topRight = topLeft + 1;
            unsigned int bottomLeft = (z + 1) * stride + x;
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
