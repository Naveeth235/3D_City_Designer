#ifndef TEXTRENDERER_H
#define TEXTRENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include "shader.h"

class TextRenderer {
public:
    TextRenderer();
    ~TextRenderer();
    
    void init(int screenWidth, int screenHeight);
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color);
    void setProjection(int width, int height);
    
private:
    Shader shader;
    unsigned int VAO, VBO;
    int width, height;
    
    void renderChar(char c, float& x, float y, float scale, const glm::vec3& color);
};

#endif
