#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;

    Vertex() = default;
    Vertex(const glm::vec3& pos, const glm::vec3& norm, const glm::vec2& uv)
        : position(pos), normal(norm), texCoords(uv) {}
};

class Mesh {
public:
    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    // Prevent accidental shallow copies of OpenGL handles
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Allow move semantics
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    // Initialize or re-upload mesh data to GPU buffers
    void setupMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    // Render mesh using indexed drawing
    void draw() const;

    // Helper procedural geometry factory methods
    static Mesh createCube(float size = 1.0f);
    static Mesh createPlane(float width = 10.0f, float depth = 10.0f, int gridX = 1, int gridZ = 1);

    // Getters
    size_t getVertexCount() const { return m_vertexCount; }
    size_t getIndexCount() const { return m_indexCount; }
    GLuint getVAO() const { return m_vao; }

private:
    void cleanup();

    GLuint m_vao;
    GLuint m_vbo;
    GLuint m_ebo;
    size_t m_vertexCount;
    size_t m_indexCount;
};
