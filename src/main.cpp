#include "Window/Window.h"
#include "Shader/Shader.h"
#include "Mesh/Mesh.h"
#include "Camera/Camera.h"
#include "Terrain/Terrain.h"
#include "Texture/Texture.h"
#include "Texture/TextureGenerator.h"
#include "Terrain/MountainNoise.h"
#include "Lake/Lake.h"
#include "Skybox/Skybox.h"
#include "Aurora/Aurora.h"
#include "Vendor/stb_image_write.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>

// Camera instance elevated overlooking the mountain valley
Camera camera(glm::vec3(0.0f, 18.0f, 32.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -18.0f);

// Mouse cursor tracking
float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;
bool firstMouse = true;

// Timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Global Aurora interactive controls
float g_auroraIntensity = 1.2f;
float g_auroraSpeed = 0.65f;

void processInput(GLFWwindow* window, float dt) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Speed boost with left control
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.setMovementSpeed(20.0f);
    } else {
        camera.setMovementSpeed(8.5f);
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

    // Interactive Aurora adjustments (TASK-015 preview)
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        g_auroraIntensity = std::min(g_auroraIntensity + 0.6f * dt, 2.5f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        g_auroraIntensity = std::max(g_auroraIntensity - 0.6f * dt, 0.1f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        g_auroraSpeed = std::min(g_auroraSpeed + 0.5f * dt, 2.0f);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        g_auroraSpeed = std::max(g_auroraSpeed - 0.5f * dt, 0.1f);
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
    const std::string TITLE = "Mountain Lake Aurora - Full Night Scene (TASK-008 & TASK-009/010)";

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
    std::cout << "  [Up/Down Arrows] - Adjust Aurora Intensity" << std::endl;
    std::cout << "  [Left/Right Arrows] - Adjust Aurora Animation Speed" << std::endl;
    std::cout << "  [ESC] - Exit application\n" << std::endl;

    // TASK-008: Starry Skybox
    Skybox skybox;
    skybox.generateProceduralStars(512, 3500);

    // TASK-009/010: Animated Aurora Borealis (curved multi-ribbon glowing curtains)
    Aurora aurora;
    aurora.init(3);

    // TASK-005/006: Majestic alpine mountain valley (180x180 area, 48.0m height)
    const std::string heightmapFile = "assets/heightmaps/mountain_valley.png";
    ensureHeightmapAsset(heightmapFile);

    Terrain terrain;
    terrain.loadFromHeightmap(heightmapFile, 180.0f, 180.0f, 48.0f);

    Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");
    if (!terrainShader.isValid()) {
        std::cerr << "Failed to initialize terrain shader!" << std::endl;
        return -1;
    }

    // Mountain textures (grass, rock, snow)
    TextureGenerator::ensureTerrainTextures("assets/textures/");
    Texture texValley("assets/textures/grass_valley.png");
    Texture texRock("assets/textures/rock_cliff.png");
    Texture texSnow("assets/textures/snow_peak.png");

    terrainShader.use();
    terrainShader.setInt("uTexValley", 0);
    terrainShader.setInt("uTexRock", 1);
    terrainShader.setInt("uTexSnow", 2);
    terrainShader.setFloat("uTextureScale", 32.0f);

    // TASK-007: Alpine Lake
    Lake lake(52.0f, 48.0f, 0.55f, 80, 80);
    if (!lake.init("shaders/water.vert", "shaders/water.frag")) {
        std::cerr << "Failed to initialize lake water surface!" << std::endl;
        return -1;
    }

    // Directional Moonlight & Cold Night Sky Ambient
    const glm::vec3 lightDir(0.4f, 0.8f, 0.3f);
    const glm::vec3 lightColor(0.55f, 0.65f, 0.85f);
    const glm::vec3 ambientColor(0.10f, 0.14f, 0.20f);

    // Deep night clear color
    const float clearR = 0.015f;
    const float clearG = 0.025f;
    const float clearB = 0.055f;

    // Main render loop
    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window.getHandle(), deltaTime);

        window.clear(clearR, clearG, clearB, 1.0f);

        float aspectRatio = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
        glm::mat4 projection = camera.getProjectionMatrix(aspectRatio, 0.1f, 500.0f);
        glm::mat4 view = camera.getViewMatrix();

        // 1. Render Mountain Terrain (Opaque)
        terrainShader.use();
        terrainShader.setMat4("projection", projection);
        terrainShader.setMat4("view", view);

        glm::mat4 terrainModel = glm::mat4(1.0f);
        terrainShader.setMat4("model", terrainModel);

        terrainShader.setVec3("uLightDir", lightDir);
        terrainShader.setVec3("uLightColor", lightColor);
        terrainShader.setVec3("uAmbientColor", ambientColor);
        terrainShader.setVec3("uViewPos", camera.getPosition());
        terrainShader.setFloat("uMaxHeight", terrain.getMaxHeight());

        texValley.bind(0);
        texRock.bind(1);
        texSnow.bind(2);

        terrain.draw();

        // 2. Render Starry Skybox (rendered with GL_LEQUAL depth)
        skybox.draw(view, projection);

        // 3. Render Animated Aurora Borealis (curved luminous ribbons with additive blend)
        aurora.setIntensity(g_auroraIntensity);
        aurora.setSpeed(g_auroraSpeed);
        aurora.draw(view, projection, currentFrame);

        // 4. Render Alpine Lake Surface (Semi-transparent with animated waves & specular)
        lake.render(projection, view, camera.getPosition(), lightDir, lightColor, ambientColor, currentFrame);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
