#pragma once

#include <string>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

class Window {
public:
    Window(int width, int height, const std::string& title);
    ~Window();

    bool init();
    bool shouldClose() const;
    void pollEvents() const;
    void swapBuffers() const;
    void clear(float r, float g, float b, float a = 1.0f) const;

    GLFWwindow* getHandle() const { return m_window; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

    int m_width;
    int m_height;
    std::string m_title;
    GLFWwindow* m_window;
};
