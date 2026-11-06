#include "Window/Window.h"
#include "Shader/Shader.h"
#include "Mesh/Mesh.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    const int SCR_WIDTH = 1280;
    const int SCR_HEIGHT = 720;
    const std::string TITLE = "Mountain Lake Aurora - Pre-Alpha";

    Window window(SCR_WIDTH, SCR_HEIGHT, TITLE);
    if (!window.init()) {
        std::cerr << "Initialization failed. Exiting..." << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // TASK-002: Load basic shader
    Shader basicShader("shaders/basic.vert", "shaders/basic.frag");
    if (!basicShader.isValid()) {
        std::cerr << "Failed to initialize basic shader!" << std::endl;
        return -1;
    }

    // TASK-003: Create test meshes (Cube and Plane)
    Mesh cubeMesh = Mesh::createCube(1.5f);
    Mesh planeMesh = Mesh::createPlane(12.0f, 12.0f, 10, 10);

    // Deep night blue clear color (base for mountain lake night scene)
    const float clearR = 0.02f;
    const float clearG = 0.04f;
    const float clearB = 0.09f;

    // Static Camera Matrices (TASK-004 will introduce interactive camera)
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 4.0f, 8.0f),  // Camera position
        glm::vec3(0.0f, 0.0f, 0.0f),  // Look at origin
        glm::vec3(0.0f, 1.0f, 0.0f)   // Up vector
    );

    // Main render loop
    while (!window.shouldClose()) {
        processInput(window.getHandle());

        // Rendering commands
        window.clear(clearR, clearG, clearB, 1.0f);

        float timeValue = static_cast<float>(glfwGetTime());

        basicShader.use();
        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);

        // 1. Draw Ground Plane (representing terrain base / valley floor)
        glm::mat4 planeModel = glm::mat4(1.0f);
        planeModel = glm::translate(planeModel, glm::vec3(0.0f, -1.0f, 0.0f));
        basicShader.setMat4("model", planeModel);
        basicShader.setVec4("uColor", glm::vec4(0.12f, 0.18f, 0.22f, 1.0f)); // Dark slate ground
        planeMesh.draw();

        // 2. Draw Rotating 3D Test Cube
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.5f, 0.0f));
        cubeModel = glm::rotate(cubeModel, timeValue * 0.8f, glm::vec3(0.5f, 1.0f, 0.0f));
        basicShader.setMat4("model", cubeModel);
        basicShader.setVec4("uColor", glm::vec4(0.2f, 0.65f, 0.85f, 1.0f)); // Glacial cyan/blue
        cubeMesh.draw();

        // Swap buffers and poll I/O events
        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
