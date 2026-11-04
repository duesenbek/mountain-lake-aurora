#include "Window/Window.h"
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

    // Deep night blue clear color (base for mountain lake night scene)
    const float clearR = 0.02f;
    const float clearG = 0.04f;
    const float clearB = 0.09f;

    // Main render loop
    while (!window.shouldClose()) {
        processInput(window.getHandle());

        // Rendering commands
        window.clear(clearR, clearG, clearB, 1.0f);

        // Swap buffers and poll I/O events
        window.swapBuffers();
        window.pollEvents();
    }

    return 0;
}
