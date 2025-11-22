#include "renderer2d.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <algorithm>
#include <iostream>

Renderer2D::Renderer2D() : VAO(0), VBO(0), width(800), height(600) {}

Renderer2D::~Renderer2D() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void Renderer2D::init(int screenWidth, int screenHeight) {
    width = screenWidth;
    height = screenHeight;
    
    // Load shader
    shader.load("shaders/basic_vert.glsl", "shaders/basic_frag.glsl");
    
    if (shader.ID == 0) {
        std::cerr << "ERROR::RENDERER2D::SHADER_FAILED_TO_LOAD" << std::endl;
        return;
    }
    
    // Create VAO and VBO
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    // Position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    // Set orthographic projection
    glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);
    shader.use();
    shader.setMat4("projection", projection);
}

void Renderer2D::clear() {
    pixels.clear();
}

void Renderer2D::render() {
    if (shader.ID == 0) return; // Don't render if shader failed to load
    
    shader.use();
    glBindVertexArray(VAO);
    
    // Draw grid
    drawGrid(width, 50);
    
    // Draw all stored lines
    for (const auto& line : lines) {
        drawBresenhamLine(line.start.x, line.start.y, line.end.x, line.end.y, line.color);
    }
    
    // Draw all stored circles
    for (const auto& circle : circles) {
        drawMidpointCircle(circle.center.x, circle.center.y, circle.radius, circle.color);
    }
    
    // Upload and draw pixels
    uploadPixels();
    
    glBindVertexArray(0);
    clear();
}

void Renderer2D::drawPixel(int x, int y, glm::vec3 color) {
    if (x >= 0 && x < width && y >= 0 && y < height) {
        pixels.push_back(glm::vec2(x, y));
        shader.setVec3("color", color);
        uploadPixels();
        pixels.clear();
    }
}

void Renderer2D::drawBresenhamLine(int x1, int y1, int x2, int y2, glm::vec3 color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    int x = x1;
    int y = y1;
    
    shader.setVec3("color", color);
    
    while (true) {
        pixels.push_back(glm::vec2(x, y));
        
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}

void Renderer2D::drawMidpointCircle(int cx, int cy, int radius, glm::vec3 color) {
    int x = 0;
    int y = radius;
    int d = 1 - radius;
    
    shader.setVec3("color", color);
    
    plot8Points(cx, cy, x, y, color);
    
    while (x < y) {
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
        plot8Points(cx, cy, x, y, color);
    }
}

void Renderer2D::plot8Points(int cx, int cy, int x, int y, glm::vec3 color) {
    pixels.push_back(glm::vec2(cx + x, cy + y));
    pixels.push_back(glm::vec2(cx - x, cy + y));
    pixels.push_back(glm::vec2(cx + x, cy - y));
    pixels.push_back(glm::vec2(cx - x, cy - y));
    pixels.push_back(glm::vec2(cx + y, cy + x));
    pixels.push_back(glm::vec2(cx - y, cy + x));
    pixels.push_back(glm::vec2(cx + y, cy - x));
    pixels.push_back(glm::vec2(cx - y, cy - x));
}

void Renderer2D::drawGrid(int gridSize, int cellSize) {
    glm::vec3 gridColor(0.2f, 0.2f, 0.2f);
    shader.setVec3("color", gridColor);
    
    // Vertical lines
    for (int x = 0; x < gridSize; x += cellSize) {
        drawBresenhamLine(x, 0, x, height, gridColor);
    }
    
    // Horizontal lines
    for (int y = 0; y < gridSize; y += cellSize) {
        drawBresenhamLine(0, y, width, y, gridColor);
    }
}

void Renderer2D::drawCrosshair(int x, int y) {
    glm::vec3 crosshairColor(1.0f, 1.0f, 0.0f);
    int size = 10;
    drawBresenhamLine(x - size, y, x + size, y, crosshairColor);
    drawBresenhamLine(x, y - size, x, y + size, crosshairColor);
}

void Renderer2D::addLine(const Line2D& line) {
    lines.push_back(line);
}

void Renderer2D::addCircle(const Circle2D& circle) {
    circles.push_back(circle);
}

void Renderer2D::clearElements() {
    lines.clear();
    circles.clear();
}

void Renderer2D::uploadPixels() {
    if (pixels.empty()) return;
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, pixels.size() * sizeof(glm::vec2), pixels.data(), GL_DYNAMIC_DRAW);
    
    glPointSize(2.0f);
    glDrawArrays(GL_POINTS, 0, pixels.size());
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
