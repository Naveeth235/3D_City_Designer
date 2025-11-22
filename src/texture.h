#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

class Texture {
public:
    unsigned int ID;
    int width;
    int height;
    int nrChannels;
    
    Texture();
    Texture(const char* imagePath, bool flip = true);
    ~Texture();
    
    void load(const char* imagePath, bool flip = true);
    void bind(unsigned int unit = 0);
    void unbind();
    
private:
    void generateTexture(unsigned char* data);
};

#endif
