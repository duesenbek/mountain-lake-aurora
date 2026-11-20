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
#include "Campfire/Campfire.h"
#include "Forest/Forest.h"
#include "Vendor/stb_image_write.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <iomanip>
#include <sstream>

// Camera instance overlooking the hillside campfire terrace, lake panorama, and aurora
Camera camera(glm::vec3(-27.0f, 7.2f, -13.0f), glm::vec3(0.0f, 1.0f, 0.0f), -135.0f, -12.0f);

// Mouse cursor tracking
float lastX = 1280.0f / 2.0f;
float lastY = 720.0f / 2.0f;
bool firstMouse = true;

// Timing & FPS Profiling
float deltaTime = 0.0f;
float lastFrame = 0.0f;
int frameCount = 0;
float fpsTimer = 0.0f;

// Scene Interaction states (TASK-014 / 015)
float g_auroraIntensity = 1.2f;
float g_auroraSpeed = 0.65f;
bool g_moonlightActive = true;
bool g_cKeyPressed = false;
bool g_lKeyPressed = false;
bool g_oneKeyPressed = false;
bool g_twoKeyPressed = false;

// Global Campfire instance (positioned precisely on left hillside terrace from Screenshot 1)
Campfire campfire(glm::vec3(-32.0f, 5.1f, -19.0f));

void processInput(GLFWwindow* window, float dt, const Terrain& terrain, float waterLevel) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Sprint mode
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        camera.setMovementSpeed(22.0f);
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

    // Interactive Aurora adjustments
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        g_auroraIntensity = std::min(g_auroraIntensity + 0.7f * dt, 2.5f);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        g_auroraIntensity = std::max(g_auroraIntensity - 0.7f * dt, 0.1f);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        g_auroraSpeed = std::min(g_auroraSpeed + 0.6f * dt, 2.5f);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        g_auroraSpeed = std::max(g_auroraSpeed - 0.6f * dt, 0.1f);

    // Toggle Campfire [C]
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!g_cKeyPressed) {
            campfire.toggle();
            std::cout << "[Interaction] Campfire " << (campfire.isActive() ? "IGNITED" : "EXTINGUISHED") << std::endl;
            g_cKeyPressed = true;
        }
    } else {
        g_cKeyPressed = false;
    }

    // Toggle Moonlight [L]
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!g_lKeyPressed) {
            g_moonlightActive = !g_moonlightActive;
            std::cout << "[Interaction] Moonlight " << (g_moonlightActive ? "ON" : "OFF") << std::endl;
            g_lKeyPressed = true;
        }
    } else {
        g_lKeyPressed = false;
    }

    // Preset 1: Campfire & Meadow Terrace View (Hillside terrace overlooking lake)
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!g_oneKeyPressed) {
            float fireY = terrain.getHeightAt(-32.0f, -19.0f);
            camera.setPosition(glm::vec3(-27.0f, fireY + 1.8f, -13.0f));
            std::cout << "[Camera Preset] Hillside Campfire & Lake terrace view." << std::endl;
            g_oneKeyPressed = true;
        }
    } else {
        g_oneKeyPressed = false;
    }

    // Preset 2: Panoramic Mountain, Forest & Aurora Aerial View
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!g_twoKeyPressed) {
            camera.setPosition(glm::vec3(0.0f, 26.0f, 48.0f));
            std::cout << "[Camera Preset] Aerial panoramic view." << std::endl;
            g_twoKeyPressed = true;
        }
    } else {
        g_twoKeyPressed = false;
    }

    // Ground collision & boundary clamping: prevents camera from plunging underground or beneath the lake/map
    glm::vec3 camPos = camera.getPosition();
    camPos.x = std::clamp(camPos.x, -82.0f, 82.0f);
    camPos.z = std::clamp(camPos.z, -82.0f, 82.0f);

    float groundY = terrain.getHeightAt(camPos.x, camPos.z);
    float minAllowedY = groundY + 1.35f; // Maintain minimum human eye height above terrain
    if (minAllowedY < waterLevel + 0.65f) {
        minAllowedY = waterLevel + 0.65f; // Do not dive underneath the lake water plane
    }

    if (camPos.y < minAllowedY) {
        camPos.y = minAllowedY;
        camera.setPosition(camPos);
    }
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
    const std::string TITLE = "Mountain Lake Aurora";

    Window window(SCR_WIDTH, SCR_HEIGHT, TITLE);
    if (!window.init()) {
        std::cerr << "Initialization failed. Exiting..." << std::endl;
        return -1;
    }

    glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window.getHandle(), mouseCallback);
    glfwSetScrollCallback(window.getHandle(), scrollCallback);

    std::cout << "========================================================\n"
              << "  Mountain Lake Aurora (OpenGL 3.3 Core)\n"
              << "  Authors: Bekzat Duisenbek (SE-2417) & Asylbek Agabekuly (SE-2422)\n"
              << "========================================================\n"
              << "Controls:\n"
              << "  [W/A/S/D] - Fly Camera\n"
              << "  [Space/Shift] - Elevation Up/Down\n"
              << "  [Left Ctrl] - Sprint Flight\n"
              << "  [Mouse] - Look Around\n"
              << "  [Scroll] - Zoom FOV\n"
              << "  [C] - Ignite / Extinguish Campfire\n"
              << "  [L] - Toggle Moonlight\n"
              << "  [Up/Down Arrows] - Aurora Intensity\n"
              << "  [Left/Right Arrows] - Aurora Animation Speed\n"
              << "  [1 / 2] - Switch Camera View Presets\n"
              << "  [ESC] - Exit Application\n"
              << "========================================================\n" << std::endl;

    // 1. Starry Skybox
    Skybox skybox;
    skybox.generateProceduralStars(512, 3500);

    // 2. Animated Aurora Borealis
    Aurora aurora;
    aurora.init(3);

    // 3. Terrain & Heightmap
    const std::string heightmapFile = "assets/heightmaps/mountain_valley.png";
    ensureHeightmapAsset(heightmapFile);

    Terrain terrain;
    terrain.loadFromHeightmap(heightmapFile, 180.0f, 180.0f, 48.0f);

    Shader terrainShader("shaders/terrain.vert", "shaders/terrain.frag");
    if (!terrainShader.isValid()) {
        std::cerr << "Failed to initialize terrain shader!" << std::endl;
        return -1;
    }

    TextureGenerator::ensureTerrainTextures("assets/textures/");
    Texture texValley("assets/textures/grass_valley.png");
    Texture texRock("assets/textures/rock_cliff.png");
    Texture texSnow("assets/textures/snow_peak.png");

    terrainShader.use();
    terrainShader.setInt("uTexValley", 0);
    terrainShader.setInt("uTexRock", 1);
    terrainShader.setInt("uTexSnow", 2);
    terrainShader.setFloat("uTextureScale", 32.0f);

    // 4. Expanded Alpine Lake Surface (TASK-007)
    Lake lake(68.0f, 62.0f, 0.55f, 96, 96);
    if (!lake.init("shaders/water.vert", "shaders/water.frag")) {
        std::cerr << "Failed to initialize lake water surface!" << std::endl;
        return -1;
    }

    // 5. Hierarchical Campfire (Positioned on hillside meadow terrace matching Screenshot 1)
    float campfireGroundY = terrain.getHeightAt(-32.0f, -19.0f);
    campfire.setPosition(glm::vec3(-32.0f, campfireGroundY + 0.05f, -19.0f));
    campfire.init();

    // 6. Instanced Boreal Pine Forest along Mountain Foothills (280 realistic conifer trees)
    Forest forest;
    if (!forest.init(terrain, 280, 32.0f, 78.0f, campfire.getPosition())) {
        std::cerr << "Failed to initialize foothill pine forest!" << std::endl;
        return -1;
    }

    // Directional Moonlight
    const glm::vec3 lightDir(0.4f, 0.8f, 0.3f);
    const glm::vec3 lightColor(0.55f, 0.65f, 0.85f);
    const glm::vec3 ambientColor(0.08f, 0.12f, 0.18f);

    // Render loop
    while (!window.shouldClose()) {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // FPS Counter in window title
        frameCount++;
        fpsTimer += deltaTime;
        if (fpsTimer >= 0.5f) {
            float fps = static_cast<float>(frameCount) / fpsTimer;
            std::stringstream ss;
            ss << "Mountain Lake Aurora | FPS: " << std::fixed << std::setprecision(1) << fps
               << " | Campfire: " << (campfire.isActive() ? "ON" : "OFF")
               << " | Moon: " << (g_moonlightActive ? "ON" : "OFF")
               << " | Aurora: " << std::setprecision(2) << g_auroraIntensity;
            glfwSetWindowTitle(window.getHandle(), ss.str().c_str());
            frameCount = 0;
            fpsTimer = 0.0f;
        }

        processInput(window.getHandle(), deltaTime, terrain, lake.getWaterLevel());

        window.clear(0.012f, 0.022f, 0.045f, 1.0f);

        float aspectRatio = static_cast<float>(window.getWidth()) / static_cast<float>(window.getHeight());
        glm::mat4 projection = camera.getProjectionMatrix(aspectRatio, 0.1f, 500.0f);
        glm::mat4 view = camera.getViewMatrix();

        PointLight pLight = campfire.getLightData(currentFrame);

        // 1. Render Terrain with Moon + Campfire Point Light
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
        terrainShader.setFloat("uMoonActive", g_moonlightActive ? 1.0f : 0.0f);

        // Campfire Point Light Uniforms
        terrainShader.setVec3("uPointLightPos", pLight.position);
        terrainShader.setVec3("uPointLightColor", pLight.color);
        terrainShader.setFloat("uPointLightIntensity", pLight.intensity);
        terrainShader.setFloat("uPointLightConst", pLight.constant);
        terrainShader.setFloat("uPointLightLinear", pLight.linear);
        terrainShader.setFloat("uPointLightQuad", pLight.quadratic);

        texValley.bind(0);
        texRock.bind(1);
        texSnow.bind(2);

        terrain.draw();

        // 2. Render Instanced Boreal Pine Forest (TASK-018, Foothills around valley)
        forest.render(projection, view, lightDir, lightColor, ambientColor, g_moonlightActive, pLight, currentFrame);

        // 3. Render Campfire (Logs, Ring of Stones, Flickering Flame via Hierarchy)
        campfire.render(projection, view, currentFrame);

        // 4. Render Starry Skybox
        skybox.draw(view, projection);

        // 5. Render Animated Aurora Borealis
        aurora.setIntensity(g_auroraIntensity);
        aurora.setSpeed(g_auroraSpeed);
        aurora.draw(view, projection, currentFrame);

        // 6. Render Alpine Lake Surface (Waves, Fresnel, Specular)
        glm::vec3 activeMoonCol = g_moonlightActive ? lightColor : glm::vec3(0.0f);
        lake.render(projection, view, camera.getPosition(), lightDir, activeMoonCol, ambientColor, currentFrame);

        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
