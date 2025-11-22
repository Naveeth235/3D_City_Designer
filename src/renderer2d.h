#ifndef RENDERER2D_H
#define RENDERER2D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"

struct Point2D {
    int x, y;
    Point2D(int x = 0, int y = 0) : x(x), y(y) {}
};

struct Line2D {
    Point2D start, end;
    glm::vec3 color;
    Line2D(Point2D s, Point2D e, glm::vec3 c = glm::vec3(1.0f)) 
        : start(s), end(e), color(c) {}
};

struct Circle2D {
    Point2D center;
    int radius;
    glm::vec3 color;
    Circle2D(Point2D c, int r, glm::vec3 col = glm::vec3(0.0f, 1.0f, 0.0f)) 
        : center(c), radius(r), color(col) {}
};

class Renderer2D {
public:
    Renderer2D();
    ~Renderer2D();
    
    void init(int screenWidth, int screenHeight);
    void clear();
    void render();
    
    // Drawing primitives using algorithms
    void drawPixel(int x, int y, glm::vec3 color = glm::vec3(1.0f));
    void drawBresenhamLine(int x1, int y1, int x2, int y2, glm::vec3 color = glm::vec3(1.0f));
    void drawMidpointCircle(int cx, int cy, int radius, glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f));
    
    // Grid and guides
    void drawGrid(int gridSize, int cellSize);
    void drawCrosshair(int x, int y);
    
    // Store drawn elements
    void addLine(const Line2D& line);
    void addCircle(const Circle2D& circle);
    void clearElements();
    
    std::vector<Line2D>& getLines() { return lines; }
    std::vector<Circle2D>& getCircles() { return circles; }
    
private:
    Shader shader;
    unsigned int VAO, VBO;
    int width, height;
    
    std::vector<glm::vec2> pixels;
    std::vector<Line2D> lines;
    std::vector<Circle2D> circles;
    
    void uploadPixels();
    void plot8Points(int cx, int cy, int x, int y, glm::vec3 color);
};

#endif
