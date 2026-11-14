#include "Window/Window.h"
#include "Shader/Shader.h"
#include "Mesh/Mesh.h"
#include "Camera/Camera.h"
#include "Terrain/Terrain.h"
#include "Terrain/MountainNoise.h"
#include "Texture/Texture.h"
#include "Texture/TextureGenerator.h"
#include "Vendor/stb_image_write.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// Camera instance positioned at the lake shore looking up at the alpine peaks
Camera camera(glm::vec3(0.0f, 3.8f, 24.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, 7.0f);

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

    // Speed boost with left control (fast fly across wide mountain valley)
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.setMovementSpeed(26.0f);
    } else {
        camera.setMovementSpeed(13.0f);
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

    for (int y = 0; y < H; ++y) {
        float ny = static_cast<float>(y) / static_cast<float>(H - 1);
        for (int x = 0; x < W; ++x) {
            float nx = static_cast<float>(x) / static_cast<float>(W - 1);
            float val = AlpineMountain::sampleHeight(nx, ny);
            data[y * W + x] = static_cast<unsigned char>(std::clamp(val * 255.0f, 0.0f, 255.0f));
        }
    }

    stbi_write_png(path.c_str(), W, H, 1, data.data(), W);
    std::cout << "[Asset] Alpine mountain heightmap generated at " << path << std::endl;
}

int main() {
    const int SCR_WIDTH = 1280;
    const int SCR_HEIGHT = 720;
    const std::string TITLE = "Mountain Lake Aurora - Environment (TASK-006)";

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

    // TASK-005/006: Initialize majestic alpine mountain valley (180x180 area, 65.0m height)
    const std::string heightmapFile = "assets/heightmaps/mountain_valley.png";
    ensureHeightmapAsset(heightmapFile);

    Terrain terrain;
    terrain.loadFromHeightmap(heightmapFile, 180.0f, 180.0f, 65.0f);

    // Load Terrain Shaders
    Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");
    if (!terrainShader.isValid()) {
        std::cerr << "Failed to initialize terrain shader!" << std::endl;
        return -1;
    }

    // TASK-006: Ensure and load mountain environment textures (grass, rock, snow)
    TextureGenerator::ensureTerrainTextures("assets/textures/");
    Texture texValley("assets/textures/grass_valley.png");
    Texture texRock("assets/textures/rock_cliff.png");
    Texture texSnow("assets/textures/snow_peak.png");

    terrainShader.use();
    terrainShader.setInt("uTexValley", 0);
    terrainShader.setInt("uTexRock", 1);
    terrainShader.setInt("uTexSnow", 2);
    terrainShader.setFloat("uTextureScale", 32.0f);

    // Lake placeholder plane in the valley center
    Mesh lakePlaceholder = Mesh::createPlane(38.0f, 34.0f, 1, 1);
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
        glm::mat4 projection = camera.getProjectionMatrix(aspectRatio, 0.1f, 400.0f);
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
        terrainShader.setVec3("uViewPos", camera.getPosition());
        terrainShader.setFloat("uMaxHeight", terrain.getMaxHeight());

        // Bind multi-layer terrain textures (TASK-006)
        texValley.bind(0);
        texRock.bind(1);
        texSnow.bind(2);

        terrain.draw();

        // 2. Render Temporary Lake Placeholder in valley center
        if (basicShader.isValid()) {
            basicShader.use();
            basicShader.setMat4("projection", projection);
            basicShader.setMat4("view", view);

            glm::mat4 lakeModel = glm::mat4(1.0f);
            lakeModel = glm::translate(lakeModel, glm::vec3(0.0f, 0.55f, 0.0f));
            basicShader.setMat4("model", lakeModel);
            basicShader.setVec4("uColor", glm::vec4(0.06f, 0.20f, 0.32f, 0.90f)); // Semi-transparent alpine water placeholder
            lakePlaceholder.draw();
        }

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
