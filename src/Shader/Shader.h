#pragma once

#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader {
public:
    Shader();
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    // Load, compile and link shader stages from source files
    bool load(const std::string& vertexPath, const std::string& fragmentPath);

    // Activate the shader program
    void use() const;

    // Deactivate the shader program
    void unbind() const;

    // Program ID getter
    GLuint getID() const { return m_programId; }
    bool isValid() const { return m_programId != 0; }

    // Utility uniform setters
    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

private:
    GLuint m_programId;
    mutable std::unordered_map<std::string, GLint> m_uniformLocationCache;

    GLint getUniformLocation(const std::string& name) const;
    static std::string readFile(const std::string& filepath);
    static bool checkCompileErrors(GLuint shader, const std::string& type);
};
