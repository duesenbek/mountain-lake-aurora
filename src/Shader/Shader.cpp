#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

Shader::Shader()
    : m_programId(0) {}

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
    : m_programId(0) {
    load(vertexPath, fragmentPath);
}

Shader::~Shader() {
    if (m_programId != 0) {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

std::string Shader::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "[Shader Error] Failed to open file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Shader::checkCompileErrors(GLuint shader, const std::string& type) {
    GLint success;
    GLchar infoLog[1024];

    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "[Shader Error] Compilation error of type " << type << ":\n"
                      << infoLog << "\n--------------------------------------------------" << std::endl;
            return false;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "[Shader Error] Program linking error:\n"
                      << infoLog << "\n--------------------------------------------------" << std::endl;
            return false;
        }
    }
    return true;
}

bool Shader::load(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vSourceStr = readFile(vertexPath);
    std::string fSourceStr = readFile(fragmentPath);

    if (vSourceStr.empty() || fSourceStr.empty()) {
        std::cerr << "[Shader Error] Source file(s) missing or empty." << std::endl;
        return false;
    }

    const char* vShaderCode = vSourceStr.c_str();
    const char* fShaderCode = fSourceStr.c_str();

    // 1. Vertex Shader
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    if (!checkCompileErrors(vertexShader, "VERTEX")) {
        glDeleteShader(vertexShader);
        return false;
    }

    // 2. Fragment Shader
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    if (!checkCompileErrors(fragmentShader, "FRAGMENT")) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    // 3. Shader Program
    if (m_programId != 0) {
        glDeleteProgram(m_programId);
        m_programId = 0;
        m_uniformLocationCache.clear();
    }

    m_programId = glCreateProgram();
    glAttachShader(m_programId, vertexShader);
    glAttachShader(m_programId, fragmentShader);
    glLinkProgram(m_programId);

    bool linked = checkCompileErrors(m_programId, "PROGRAM");

    // Free shaders as they're linked into program now
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!linked) {
        glDeleteProgram(m_programId);
        m_programId = 0;
        return false;
    }

    std::cout << "[Shader] Successfully loaded and linked: " << vertexPath << " & " << fragmentPath << std::endl;
    return true;
}

void Shader::use() const {
    if (m_programId != 0) {
        glUseProgram(m_programId);
    }
}

void Shader::unbind() const {
    glUseProgram(0);
}

GLint Shader::getUniformLocation(const std::string& name) const {
    if (m_programId == 0) return -1;

    auto it = m_uniformLocationCache.find(name);
    if (it != m_uniformLocationCache.end()) {
        return it->second;
    }

    GLint location = glGetUniformLocation(m_programId, name.c_str());
    if (location == -1) {
        // Output once if uniform not found
        // std::cerr << "[Shader Warning] Uniform '" << name << "' doesn't exist or is not used." << std::endl;
    }
    m_uniformLocationCache[name] = location;
    return location;
}

void Shader::setBool(const std::string& name, bool value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, static_cast<int>(value));
    }
}

void Shader::setInt(const std::string& name, int value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1i(location, value);
    }
}

void Shader::setFloat(const std::string& name, float value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform1f(location, value);
    }
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setVec2(const std::string& name, float x, float y) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform2f(location, x, y);
    }
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setVec3(const std::string& name, float x, float y, float z) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform3f(location, x, y, z);
    }
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniform4f(location, x, y, z, w);
    }
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    GLint location = getUniformLocation(name);
    if (location != -1) {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
    }
}
