#include "Window/Window.h"
#include "Shader/Shader.h"
#include "Mesh/Mesh.h"
#include "Camera/Camera.h"
#include "Terrain/Terrain.h"
#include "Vendor/stb_image_write.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// Camera instance elevated overlooking the mountain valley
Camera camera(glm::vec3(0.0f, 16.0f, 26.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -22.0f);

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

    // Speed boost with left control
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.setMovementSpeed(14.0f);
    } else {
        camera.setMovementSpeed(7.0f);
    }

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
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.processMouseMovement(xoffset, yoffset);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.processMouseScroll(static_cast<float>(yoffset));
}

// Generates a grayscale heightmap PNG asset if it doesn't already exist
void ensureHeightmapAsset(const std::string& path) {
    const int W = 256;
    const int H = 256;
    std::vector<unsigned char> data(W * H);

    float cx = W * 0.5f;
    float cy = H * 0.5f;
    float maxR = std::sqrt(cx * cx + cy * cy);

    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            float dx = x - cx;
            float dy = y - cy;
            float dist = std::sqrt(dx * dx + dy * dy) / maxR;

            float bowl = std::pow(dist, 1.8f);
            float nx = static_cast<float>(x) / static_cast<float>(W);
            float ny = static_cast<float>(y) / static_cast<float>(H);

            float peaks = 0.5f * std::sin(nx * 14.0f) * std::cos(ny * 14.0f)
                        + 0.25f * std::sin(nx * 28.0f + 1.2f) * std::cos(ny * 28.0f + 0.8f)
                        + 0.125f * std::sin(nx * 56.0f) * std::sin(ny * 56.0f);

            float val = bowl * 0.75f + peaks * 0.25f * bowl;
            if (dist < 0.22f) {
                float t = dist / 0.22f;
                val *= (t * t);
            }

            val = std::clamp(val, 0.0f, 1.0f);
            data[y * W + x] = static_cast<unsigned char>(val * 255.0f);
        }
    }

    stbi_write_png(path.c_str(), W, H, 1, data.data(), W);
    std::cout << "[Asset] Heightmap generated at " << path << std::endl;
}

int main() {
    const int SCR_WIDTH = 1280;
    const int SCR_HEIGHT = 720;
    const std::string TITLE = "Mountain Lake Aurora - Terrain (TASK-005)";

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
    std::cout << "  [Left Ctrl] - Sprint / Fast Fly" << std::endl;
    std::cout << "  [Mouse] - Look around (pitch/yaw)" << std::endl;
    std::cout << "  [Scroll Wheel] - Zoom (FOV)" << std::endl;
    std::cout << "  [ESC] - Exit application\n" << std::endl;

    // TASK-005: Initialize heightmap asset and Terrain
    const std::string heightmapFile = "assets/heightmaps/mountain_valley.png";
    ensureHeightmapAsset(heightmapFile);

    Terrain terrain;
    terrain.loadFromHeightmap(heightmapFile, 70.0f, 70.0f, 15.0f);

    // Load Terrain Shaders
    Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");
    if (!terrainShader.isValid()) {
        std::cerr << "Failed to initialize terrain shader!" << std::endl;
        return -1;
    }

    // Also keep a reference water/lake placeholder plane in the valley center
    Mesh lakePlaceholder = Mesh::createPlane(18.0f, 18.0f, 1, 1);
    Shader basicShader("shaders/basic.vert", "shaders/basic.frag");

    // Deep night blue clear color
    const float clearR = 0.015f;
    const float clearG = 0.03f;
    const float clearB = 0.07f;

    // Main render loop
    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.getHandle(), deltaTime);

        window.clear(clearR, clearG, clearB, 1.0f);

        float aspectRatio = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
        glm::mat4 projection = camera.getProjectionMatrix(aspectRatio, 0.1f, 250.0f);
        glm::mat4 view = camera.getViewMatrix();

        // 1. Render Mountain Terrain
        terrainShader.use();
        terrainShader.setMat4("projection", projection);
        terrainShader.setMat4("view", view);

        glm::mat4 terrainModel = glm::mat4(1.0f);
        terrainShader.setMat4("model", terrainModel);

        // Directional Moonlight & Ambient
        terrainShader.setVec3("uLightDir", glm::vec3(0.4f, 0.8f, 0.3f));
        terrainShader.setVec3("uLightColor", glm::vec3(0.55f, 0.65f, 0.85f));   // Cold moonlight
        terrainShader.setVec3("uAmbientColor", glm::vec3(0.10f, 0.14f, 0.20f)); // Night ambient
        terrainShader.setFloat("uMaxHeight", terrain.getMaxHeight());

        terrain.draw();

        // 2. Render Temporary Lake Placeholder in valley center
        if (basicShader.isValid()) {
            basicShader.use();
            basicShader.setMat4("projection", projection);
            basicShader.setMat4("view", view);

            glm::mat4 lakeModel = glm::mat4(1.0f);
            lakeModel = glm::translate(lakeModel, glm::vec3(0.0f, 0.4f, 0.0f));
            basicShader.setMat4("model", lakeModel);
            basicShader.setVec4("uColor", glm::vec4(0.08f, 0.22f, 0.35f, 0.85f)); // Semi-transparent alpine water placeholder
            lakePlaceholder.draw();
        }

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
