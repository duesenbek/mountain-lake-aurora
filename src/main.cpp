#include "Window/Window.h"
#include "Shader/Shader.h"
#include "Mesh/Mesh.h"
#include "Camera/Camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

// Camera instance (initial position overlooking the scene)
Camera camera(glm::vec3(0.0f, 3.5f, 9.0f));

// Mouse cursor tracking
float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

void processInput(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Camera movement controls
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.processKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.processKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.processKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.processKeyboard(RIGHT, dt);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.processKeyboard(UP, dt);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.processKeyboard(DOWN, dt);
}

void mouseCallback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

int main() {
    const int SCR_WIDTH = 1280;
    const int SCR_HEIGHT = 720;
    const std::string TITLE = "Mountain Lake Aurora - Interactive Camera (TASK-004)";

    Window window(SCR_WIDTH, SCR_HEIGHT, TITLE);
    if (!window.init()) {
        std::cerr << "Initialization failed. Exiting..." << std::endl;
        return -1;
    }

    // Capture cursor for smooth FPS camera control
    glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window.getHandle(), mouseCallback);
    glfwSetScrollCallback(window.getHandle(), scrollCallback);

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
    std::cout << "\nControls:" << std::endl;
    std::cout << "  [W/A/S/D] - Move Camera" << std::endl;
    std::cout << "  [Space/Shift] - Up/Down" << std::endl;
    std::cout << "  [Mouse] - Look around (pitch/yaw)" << std::endl;
    std::cout << "  [Scroll Wheel] - Zoom (FOV)" << std::endl;
    std::cout << "  [ESC] - Exit application\n" << std::endl;

    // TASK-002: Load basic shader
    Shader basicShader("shaders/basic.vert", "shaders/basic.frag");
    if (!basicShader.isValid()) {
        std::cerr << "Failed to initialize basic shader!" << std::endl;
        return -1;
    }

    // TASK-003: Create meshes (Cube and Ground Plane)
    Mesh cubeMesh = Mesh::createCube(1.5f);
    Mesh planeMesh = Mesh::createPlane(20.0f, 20.0f, 20, 20);

    // Deep night blue clear color (base for mountain lake night scene)
    const float clearR = 0.02f;
    const float clearG = 0.04f;
    const float clearB = 0.09f;

    // Main render loop
    while (!window.shouldClose()) {
        // Calculate per-frame deltaTime
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Process inputs
        processInput(window.getHandle(), deltaTime);

        // Rendering commands
        window.clear(clearR, clearG, clearB, 1.0f);

        basicShader.use();

        // TASK-004: Dynamic view & projection from Camera
        float aspectRatio = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
        glm::mat4 projection = camera.getProjectionMatrix(aspectRatio);
        glm::mat4 view = camera.getViewMatrix();

        basicShader.setMat4("projection", projection);
        basicShader.setMat4("view", view);

        // 1. Draw Ground Plane
        glm::mat4 planeModel = glm::mat4(1.0f);
        planeModel = glm::translate(planeModel, glm::vec3(0.0f, -1.0f, 0.0f));
        basicShader.setMat4("model", planeModel);
        basicShader.setVec4("uColor", glm::vec4(0.12f, 0.18f, 0.22f, 1.0f));
        planeMesh.draw();

        // 2. Draw Rotating 3D Test Cube
        glm::mat4 cubeModel = glm::mat4(1.0f);
        cubeModel = glm::translate(cubeModel, glm::vec3(0.0f, 0.5f, 0.0f));
        cubeModel = glm::rotate(cubeModel, currentFrame * 0.8f, glm::vec3(0.5f, 1.0f, 0.0f));
        basicShader.setMat4("model", cubeModel);
        basicShader.setVec4("uColor", glm::vec4(0.2f, 0.65f, 0.85f, 1.0f));
        cubeMesh.draw();

        // Swap buffers and poll I/O events
        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
