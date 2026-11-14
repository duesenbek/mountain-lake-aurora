#pragma once

#include <GL/glew.h>
#include <string>

class Texture {
public:
    Texture();
    explicit Texture(const std::string& filepath);
    ~Texture();

    // Move semantics (no copy to avoid double-free of GPU handles)
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    bool load(const std::string& filepath);
    void bind(unsigned int slot = 0) const;
    void unbind() const;

    GLuint getID() const { return m_rendererID; }
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    bool isValid() const { return m_rendererID != 0; }

private:
    GLuint m_rendererID;
    std::string m_filepath;
    int m_width;
    int m_height;
    int m_nrChannels;
};
