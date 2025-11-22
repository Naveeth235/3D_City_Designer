#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>

Texture::Texture() : ID(0), width(0), height(0), nrChannels(0) {}

Texture::Texture(const char* imagePath, bool flip) {
    load(imagePath, flip);
}

Texture::~Texture() {
    if (ID != 0) {
        glDeleteTextures(1, &ID);
    }
}

void Texture::load(const char* imagePath, bool flip) {
    stbi_set_flip_vertically_on_load(flip);
    
    unsigned char* data = stbi_load(imagePath, &width, &height, &nrChannels, 0);
    
    if (data) {
        generateTexture(data);
        stbi_image_free(data);
    } else {
        std::cout << "WARNING::TEXTURE::FAILED_TO_LOAD: " << imagePath << " - Using fallback texture" << std::endl;
        // Create a simple fallback texture (white)
        width = 2;
        height = 2;
        nrChannels = 3;
        unsigned char fallbackData[] = {
            255, 255, 255,  200, 200, 200,
            200, 200, 200,  255, 255, 255
        };
        generateTexture(fallbackData);
    }
}

void Texture::bind(unsigned int unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::generateTexture(unsigned char* data) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    
    // Set texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    GLenum format;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    else
        format = GL_RGB;
    
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}
