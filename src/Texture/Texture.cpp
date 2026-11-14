#include "Texture.h"
#include "../Vendor/stb_image.h"
#include <iostream>

Texture::Texture()
    : m_rendererID(0), m_width(0), m_height(0), m_nrChannels(0) {
}

Texture::Texture(const std::string& filepath)
    : m_rendererID(0), m_width(0), m_height(0), m_nrChannels(0) {
    load(filepath);
}

Texture::~Texture() {
    if (m_rendererID != 0) {
        glDeleteTextures(1, &m_rendererID);
        m_rendererID = 0;
    }
}

Texture::Texture(Texture&& other) noexcept
    : m_rendererID(other.m_rendererID),
      m_filepath(std::move(other.m_filepath)),
      m_width(other.m_width),
      m_height(other.m_height),
      m_nrChannels(other.m_nrChannels) {
    other.m_rendererID = 0;
    other.m_width = 0;
    other.m_height = 0;
    other.m_nrChannels = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        if (m_rendererID != 0) {
            glDeleteTextures(1, &m_rendererID);
        }
        m_rendererID = other.m_rendererID;
        m_filepath = std::move(other.m_filepath);
        m_width = other.m_width;
        m_height = other.m_height;
        m_nrChannels = other.m_nrChannels;

        other.m_rendererID = 0;
        other.m_width = 0;
        other.m_height = 0;
        other.m_nrChannels = 0;
    }
    return *this;
}

bool Texture::load(const std::string& filepath) {
    m_filepath = filepath;

    if (m_rendererID != 0) {
        glDeleteTextures(1, &m_rendererID);
        m_rendererID = 0;
    }

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(filepath.c_str(), &m_width, &m_height, &m_nrChannels, 0);
    if (!data) {
        std::cerr << "[Texture Error] Failed to load image: " << filepath << std::endl;
        return false;
    }

    GLenum internalFormat = GL_RGB;
    GLenum dataFormat = GL_RGB;

    if (m_nrChannels == 1) {
        internalFormat = GL_RED;
        dataFormat = GL_RED;
    } else if (m_nrChannels == 3) {
        internalFormat = GL_RGB8;
        dataFormat = GL_RGB;
    } else if (m_nrChannels == 4) {
        internalFormat = GL_RGBA8;
        dataFormat = GL_RGBA;
    }

    glGenTextures(1, &m_rendererID);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);

    // Texture wrapping: repeat seamlessly across terrain coordinates
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Mipmapping and trilinear filtering for high visual quality at distances
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_width, m_height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);

    std::cout << "[Texture] Successfully loaded " << filepath 
              << " (" << m_width << "x" << m_height << ", " << m_nrChannels << " ch, ID=" << m_rendererID << ")" << std::endl;
    return true;
}

void Texture::bind(unsigned int slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_rendererID);
}

void Texture::unbind() const {
    glBindTexture(GL_TEXTURE_2D, 0);
}
