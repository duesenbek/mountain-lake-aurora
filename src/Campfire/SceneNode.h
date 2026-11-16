#pragma once

#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Mesh/Mesh.h"
#include "../Shader/Shader.h"

// SceneNode represents a hierarchical transformation node in the scene graph
class SceneNode {
public:
    SceneNode(const std::string& name = "Node")
        : m_name(name),
          m_position(0.0f),
          m_rotation(0.0f),
          m_scale(1.0f),
          m_color(1.0f),
          m_parent(nullptr) {}

    void setPosition(const glm::vec3& pos) { m_position = pos; }
    void setRotation(const glm::vec3& rot) { m_rotation = rot; }
    void setScale(const glm::vec3& scale)  { m_scale = scale; }
    void setColor(const glm::vec4& color)  { m_color = color; }
    void setMesh(const std::shared_ptr<Mesh>& mesh) { m_mesh = mesh; }

    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getScale() const    { return m_scale; }
    glm::vec4 getColor() const    { return m_color; }

    void addChild(const std::shared_ptr<SceneNode>& child) {
        child->m_parent = this;
        m_children.push_back(child);
    }

    // Calculates local model matrix (Translate * Rotate * Scale)
    glm::mat4 getLocalTransform() const {
        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, m_position);
        if (m_rotation.x != 0.0f)
            transform = glm::rotate(transform, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        if (m_rotation.y != 0.0f)
            transform = glm::rotate(transform, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        if (m_rotation.z != 0.0f)
            transform = glm::rotate(transform, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, m_scale);
        return transform;
    }

    // Computes world transform by traversing the parent hierarchy
    glm::mat4 getWorldTransform() const {
        if (m_parent) {
            return m_parent->getWorldTransform() * getLocalTransform();
        }
        return getLocalTransform();
    }

    // Recursive hierarchical rendering
    void draw(const Shader& shader, const glm::mat4& parentTransform = glm::mat4(1.0f)) const {
        glm::mat4 currentWorld = parentTransform * getLocalTransform();

        if (m_mesh) {
            shader.setMat4("model", currentWorld);
            shader.setVec4("uColor", m_color);
            m_mesh->draw();
        }

        for (const auto& child : m_children) {
            child->draw(shader, currentWorld);
        }
    }

private:
    std::string m_name;
    glm::vec3 m_position;
    glm::vec3 m_rotation; // Euler angles in degrees
    glm::vec3 m_scale;
    glm::vec4 m_color;

    std::shared_ptr<Mesh> m_mesh;
    SceneNode* m_parent;
    std::vector<std::shared_ptr<SceneNode>> m_children;
};
