#include "renderer3d.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>
#include <iostream>
#include <algorithm>

// Camera implementation
Camera::Camera() 
    : position(300.0f, 150.0f, 500.0f),
      front(0.0f, 0.0f, -1.0f),
      up(0.0f, 1.0f, 0.0f),
      yaw(-90.0f),
      pitch(0.0f),
      fov(45.0f) {
    updateVectors();
}

void Camera::updateVectors() {
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(direction);
    
    right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));
    up = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

// Mesh implementation
Mesh::Mesh() : VAO(0), VBO(0), EBO(0) {}

Mesh::~Mesh() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Mesh::setup() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Texture coordinate attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glBindVertexArray(0);
}

void Mesh::draw() {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

// Renderer3D implementation
Renderer3D::Renderer3D() : width(800), height(600), timeOfDay(12.0f), timeSpeed(1.0f) {}

Renderer3D::~Renderer3D() {}

void Renderer3D::init(int screenWidth, int screenHeight) {
    width = screenWidth;
    height = screenHeight;
    
    // Load shader
    shader.load("shaders/tex_vert.glsl", "shaders/tex_frag.glsl");
    
    // Load textures with fallback colors
    buildingTexture1.load("assets/building1.jpg", true);
    buildingTexture2.load("assets/building2.jpg", true);
    roadTexture.load("assets/road.jpg", true);
    grassTexture.load("assets/grass.jpg", true);
    fountainTexture.load("assets/fountain.png", true);
    
    // Set projection
    setProjection(width, height);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
}

void Renderer3D::setProjection(int w, int h) {
    width = w;
    height = h;
    
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov), 
                                           (float)width / (float)height, 
                                           0.1f, 1000.0f);
    
    shader.use();
    shader.setMat4("projection", projection);
}

void Renderer3D::render(const CityGenerator& cityGen) {
    // Set sky color based on time of day
    glm::vec3 skyColor = getSkyColor();
    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    shader.use();
    
    // Set view matrix
    glm::mat4 view = camera.getViewMatrix();
    shader.setMat4("view", view);
    
    // Set dynamic lighting based on time of day
    glm::vec3 sunPos = getSunPosition();
    glm::vec3 sunColor = getSunLightColor();
    shader.setVec3("lightPos", sunPos);
    shader.setVec3("lightColor", sunColor);
    shader.setVec3("viewPos", camera.position);
    shader.setFloat("emissive", 0.0f); // Normal lighting by default
    shader.setInt("useTexture", 1); // Use texture by default
    shader.setVec3("materialColor", glm::vec3(1.0f, 1.0f, 1.0f)); // Default white
    
    // Setup street lights as point lights if it's night
    const auto& streetLights = cityGen.getStreetLights();
    if (isNightTime() && !streetLights.empty()) {
        int numLights = std::min(static_cast<int>(streetLights.size()), 50);
        shader.setInt("numPointLights", numLights);
        
        for (int i = 0; i < numLights; i++) {
            std::string posName = "pointLightPositions[" + std::to_string(i) + "]";
            std::string colorName = "pointLightColors[" + std::to_string(i) + "]";
            
            shader.setVec3(posName, streetLights[i].position);
            shader.setVec3(colorName, glm::vec3(1.0f, 0.9f, 0.5f)); // Warm yellow light
        }
    } else {
        shader.setInt("numPointLights", 0);
    }
    
    // Render scene components
    renderGround(cityGen.getLayoutSize());
    renderRoads(cityGen.getRoads());
    renderBuildings(cityGen.getBuildings());
    renderParks(cityGen.getParks());
    renderVehicles(cityGen.getVehicles());
    
    // Render street lights at night
    if (isNightTime()) {
        renderStreetLights(cityGen.getStreetLights());
    }
}

void Renderer3D::renderGround(int size) {
    grassTexture.bind(0);
    shader.setInt("diffuseTexture", 0);
    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(size / 2.0f, -1.0f, size / 2.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f, size));
    shader.setMat4("model", model);
    
    // Create ground quad
    float vertices[] = {
        // Positions          // Normals           // TexCoords
        -0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f, 0.0f, -0.5f,   0.0f, 1.0f, 0.0f,   10.0f, 0.0f,
         0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   10.0f, 10.0f,
        -0.5f, 0.0f,  0.5f,   0.0f, 1.0f, 0.0f,   0.0f, 10.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Renderer3D::renderBuildings(const std::vector<Building>& buildings) {
    for (const auto& building : buildings) {
        if (building.textureIndex == 0)
            buildingTexture1.bind(0);
        else
            buildingTexture2.bind(0);
        
        shader.setInt("diffuseTexture", 0);
        
        Mesh mesh = createCubeMesh(building.size.x, building.height, building.size.y);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(building.position.x + building.size.x / 2.0f, 
                                               building.height / 2.0f, 
                                               building.position.y + building.size.y / 2.0f));
        shader.setMat4("model", model);
        
        mesh.draw();
        
        // Add glowing windows at night
        if (isNightTime()) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, 0);
            
            // Create small window cubes on building facade - multiple sides
            int numWindowRows = static_cast<int>(building.height / 12.0f);
            if (numWindowRows < 1) numWindowRows = 1;
            
            for (int i = 0; i < numWindowRows; ++i) {
                float windowY = 8.0f + i * 12.0f;
                
                // Front windows (multiple across facade)
                int windowsPerRow = static_cast<int>(building.size.x / 15.0f);
                if (windowsPerRow < 1) windowsPerRow = 1;
                
                for (int w = 0; w < windowsPerRow; ++w) {
                    float windowX = building.position.x + 10.0f + w * 15.0f;
                    if (windowX > building.position.x + building.size.x - 10.0f) continue;
                    
                    Mesh window = createCubeMesh(4.0f, 3.0f, 0.8f);
                    model = glm::mat4(1.0f);
                    model = glm::translate(model, glm::vec3(
                        windowX,
                        windowY,
                        building.position.y + building.size.y / 2.0f + building.size.y / 2.0f + 0.5f
                    ));
                    shader.setMat4("model", model);
                    
                    // Bright warm yellow glow - EMISSIVE!
                    shader.setFloat("emissive", 1.0f); // Enable glow
                    shader.setVec3("lightColor", glm::vec3(1.0f, 0.9f, 0.4f)); // Bright warm yellow
                    window.draw();
                    shader.setFloat("emissive", 0.0f); // Disable glow
                }
            }
            
            // Restore lighting
            glm::vec3 sunColor = getSunLightColor();
            glm::vec3 sunPos = getSunPosition();
            shader.setVec3("lightPos", sunPos);
            shader.setVec3("lightColor", sunColor);
            shader.setFloat("emissive", 0.0f);
        }
    }
}

void Renderer3D::renderRoads(const std::vector<Road>& roads) {
    roadTexture.bind(0);
    shader.setInt("diffuseTexture", 0);
    
    for (const auto& road : roads) {
        glm::vec3 start(road.start.x, 0.0f, road.start.y);
        glm::vec3 end(road.end.x, 0.0f, road.end.y);
        
        glm::vec3 direction = end - start;
        float length = glm::length(direction);
        direction = glm::normalize(direction);
        
        glm::vec3 center = (start + end) / 2.0f;
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, center);
        
        float angle = atan2(direction.z, direction.x);
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::scale(model, glm::vec3(length, 0.5f, 8.0f));
        
        shader.setMat4("model", model);
        
        Mesh mesh = createCubeMesh(1.0f, 1.0f, 1.0f);
        mesh.draw();
    }
}

void Renderer3D::renderParks(const std::vector<Park>& parks) {
    // Render beautiful blue water pond
    for (const auto& park : parks) {
        // Create blue water surface - thicker for better visibility
        Mesh waterMesh = createCylinderMesh(park.radius, 3.0f, 32);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(park.center.x, 1.5f, park.center.y));
        shader.setMat4("model", model);
        
        // Use material color with full emissive for bright blue water
        shader.setInt("useTexture", 0); // Don't use texture
        shader.setVec3("materialColor", glm::vec3(0.2f, 0.6f, 1.0f)); // Bright blue water
        shader.setFloat("emissive", 1.0f); // Full emissive - pure color
        
        waterMesh.draw();
        
        // Restore defaults for other objects
        shader.setInt("useTexture", 1);
        shader.setFloat("emissive", 0.0f);
        
        // Add decorative fountain in center
        fountainTexture.bind(0);
        Mesh fountain = createCylinderMesh(5.0f, 15.0f, 16);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(park.center.x, 7.5f, park.center.y));
        shader.setMat4("model", model);
        
        fountain.draw();
    }
}

Mesh Renderer3D::createCubeMesh(float width, float height, float depth) {
    Mesh mesh;
    
    float w = width / 2.0f;
    float h = height / 2.0f;
    float d = depth / 2.0f;
    
    mesh.vertices = {
        // Positions          // Normals           // TexCoords
        // Front face
        -w, -h,  d,   0.0f,  0.0f,  1.0f,   0.0f, 0.0f,
         w, -h,  d,   0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
         w,  h,  d,   0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -w,  h,  d,   0.0f,  0.0f,  1.0f,   0.0f, 1.0f,
        
        // Back face
        -w, -h, -d,   0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        -w,  h, -d,   0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
         w,  h, -d,   0.0f,  0.0f, -1.0f,   0.0f, 1.0f,
         w, -h, -d,   0.0f,  0.0f, -1.0f,   0.0f, 0.0f,
        
        // Top face
        -w,  h, -d,   0.0f,  1.0f,  0.0f,   0.0f, 1.0f,
        -w,  h,  d,   0.0f,  1.0f,  0.0f,   0.0f, 0.0f,
         w,  h,  d,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,
         w,  h, -d,   0.0f,  1.0f,  0.0f,   1.0f, 1.0f,
        
        // Bottom face
        -w, -h, -d,   0.0f, -1.0f,  0.0f,   0.0f, 0.0f,
         w, -h, -d,   0.0f, -1.0f,  0.0f,   1.0f, 0.0f,
         w, -h,  d,   0.0f, -1.0f,  0.0f,   1.0f, 1.0f,
        -w, -h,  d,   0.0f, -1.0f,  0.0f,   0.0f, 1.0f,
        
        // Right face
         w, -h, -d,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
         w,  h, -d,   1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
         w,  h,  d,   1.0f,  0.0f,  0.0f,   1.0f, 1.0f,
         w, -h,  d,   1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        
        // Left face
        -w, -h, -d,  -1.0f,  0.0f,  0.0f,   1.0f, 0.0f,
        -w, -h,  d,  -1.0f,  0.0f,  0.0f,   0.0f, 0.0f,
        -w,  h,  d,  -1.0f,  0.0f,  0.0f,   0.0f, 1.0f,
        -w,  h, -d,  -1.0f,  0.0f,  0.0f,   1.0f, 1.0f
    };
    
    mesh.indices = {
        0, 1, 2,  2, 3, 0,    // Front
        4, 5, 6,  6, 7, 4,    // Back
        8, 9, 10, 10, 11, 8,  // Top
        12, 13, 14, 14, 15, 12, // Bottom
        16, 17, 18, 18, 19, 16, // Right
        20, 21, 22, 22, 23, 20  // Left
    };
    
    mesh.setup();
    return mesh;
}

Mesh Renderer3D::createCylinderMesh(float radius, float height, int segments) {
    Mesh mesh;
    
    float halfHeight = height / 2.0f;
    
    // Generate vertices
    for (int i = 0; i <= segments; ++i) {
        float angle = (2.0f * 3.14159f * i) / segments;
        float x = radius * cos(angle);
        float z = radius * sin(angle);
        
        // Top vertex
        mesh.vertices.insert(mesh.vertices.end(), {
            x, halfHeight, z,
            0.0f, 1.0f, 0.0f,
            (float)i / segments, 1.0f
        });
        
        // Bottom vertex
        mesh.vertices.insert(mesh.vertices.end(), {
            x, -halfHeight, z,
            0.0f, -1.0f, 0.0f,
            (float)i / segments, 0.0f
        });
    }
    
    // Generate indices
    for (int i = 0; i < segments; ++i) {
        int topCurrent = i * 2;
        int bottomCurrent = i * 2 + 1;
        int topNext = (i + 1) * 2;
        int bottomNext = (i + 1) * 2 + 1;
        
        // Side face
        mesh.indices.push_back(topCurrent);
        mesh.indices.push_back(bottomCurrent);
        mesh.indices.push_back(topNext);
        
        mesh.indices.push_back(topNext);
        mesh.indices.push_back(bottomCurrent);
        mesh.indices.push_back(bottomNext);
    }
    
    mesh.setup();
    return mesh;
}

void Renderer3D::updateCamera(float deltaTime, bool* keys, float mouseOffsetX, float mouseOffsetY) {
    float velocity = 100.0f * deltaTime;
    
    if (keys[0]) // W
        camera.position += camera.front * velocity;
    if (keys[1]) // S
        camera.position -= camera.front * velocity;
    if (keys[2]) // A
        camera.position -= camera.right * velocity;
    if (keys[3]) // D
        camera.position += camera.right * velocity;
    if (keys[4]) // Space
        camera.position += camera.up * velocity;
    if (keys[5]) // Left Shift
        camera.position -= camera.up * velocity;
    
    // Mouse movement
    float sensitivity = 0.1f;
    camera.yaw += mouseOffsetX * sensitivity;
    camera.pitch += mouseOffsetY * sensitivity;
    
    // Constrain pitch
    if (camera.pitch > 89.0f)
        camera.pitch = 89.0f;
    if (camera.pitch < -89.0f)
        camera.pitch = -89.0f;
    
    camera.updateVectors();
}

void Renderer3D::updateTimeOfDay(float deltaTime) {
    timeOfDay += deltaTime * timeSpeed / 60.0f; // Convert to hours
    if (timeOfDay >= 24.0f) {
        timeOfDay -= 24.0f;
    }
}

glm::vec3 Renderer3D::getSkyColor() const {
    // Dawn: 5-7, Day: 7-17, Dusk: 17-19, Night: 19-5
    if (timeOfDay >= 7.0f && timeOfDay < 17.0f) {
        // Daytime - bright blue sky
        return glm::vec3(0.5f, 0.7f, 0.9f);
    } else if (timeOfDay >= 5.0f && timeOfDay < 7.0f) {
        // Dawn - orange/pink gradient
        float t = (timeOfDay - 5.0f) / 2.0f;
        glm::vec3 dawnColor(0.9f, 0.5f, 0.3f);
        glm::vec3 dayColor(0.5f, 0.7f, 0.9f);
        return glm::mix(dawnColor, dayColor, t);
    } else if (timeOfDay >= 17.0f && timeOfDay < 19.0f) {
        // Dusk - orange/purple gradient
        float t = (timeOfDay - 17.0f) / 2.0f;
        glm::vec3 dayColor(0.5f, 0.7f, 0.9f);
        glm::vec3 duskColor(0.8f, 0.4f, 0.5f);
        return glm::mix(dayColor, duskColor, t);
    } else {
        // Night - dark blue
        return glm::vec3(0.05f, 0.05f, 0.15f);
    }
}

glm::vec3 Renderer3D::getSunLightColor() const {
    if (timeOfDay >= 7.0f && timeOfDay < 17.0f) {
        // Bright daylight
        return glm::vec3(1.0f, 1.0f, 0.95f);
    } else if (timeOfDay >= 5.0f && timeOfDay < 7.0f) {
        // Dawn - warm light
        float t = (timeOfDay - 5.0f) / 2.0f;
        return glm::mix(glm::vec3(1.0f, 0.6f, 0.3f), glm::vec3(1.0f, 1.0f, 0.95f), t);
    } else if (timeOfDay >= 17.0f && timeOfDay < 19.0f) {
        // Dusk - warm light fading
        float t = (timeOfDay - 17.0f) / 2.0f;
        return glm::mix(glm::vec3(1.0f, 1.0f, 0.95f), glm::vec3(1.0f, 0.5f, 0.2f), t);
    } else {
        // Night - dim moonlight
        return glm::vec3(0.2f, 0.2f, 0.3f);
    }
}

glm::vec3 Renderer3D::getSunPosition() const {
    // Sun moves in an arc across the sky
    float angle = (timeOfDay / 24.0f) * 2.0f * 3.14159f;
    float x = 300.0f + 400.0f * cos(angle);
    float y = 200.0f + 300.0f * sin(angle);
    float z = 300.0f;
    return glm::vec3(x, y, z);
}

bool Renderer3D::isNightTime() const {
    return timeOfDay < 6.0f || timeOfDay >= 19.0f;
}

void Renderer3D::renderVehicles(const std::vector<Vehicle>& vehicles) {
    roadTexture.bind(0);
    shader.setInt("diffuseTexture", 0);
    
    for (const auto& vehicle : vehicles) {
        Mesh carMesh = createCubeMesh(8.0f, 4.0f, 4.0f);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, vehicle.position);
        
        // Rotate car to face direction
        float angle = atan2(vehicle.direction.z, vehicle.direction.x);
        model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
        
        shader.setMat4("model", model);
        carMesh.draw();
    }
}

void Renderer3D::renderStreetLights(const std::vector<StreetLight>& lights) {
    // Render glowing street lights at night - disable texturing for solid colors
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    // Save current light settings
    glm::vec3 originalLightPos = getSunPosition();
    glm::vec3 originalLightColor = getSunLightColor();
    
    for (const auto& light : lights) {
        // Create light pole (dark gray) - normal rendering
        Mesh pole = createCylinderMesh(0.5f, 14.0f, 8);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(light.position.x, 7.0f, light.position.z));
        shader.setMat4("model", model);
        
        shader.setFloat("emissive", 0.0f);
        shader.setVec3("lightColor", glm::vec3(0.3f, 0.3f, 0.3f));
        pole.draw();
        
        // Create glowing bulb at top - EMISSIVE BRIGHT GLOW!
        Mesh bulb = createCubeMesh(4.0f, 4.0f, 4.0f); // Even bigger
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(light.position.x, 16.0f, light.position.z));
        shader.setMat4("model", model);
        
        // PURE EMISSIVE GLOW - no lighting calculation!
        shader.setFloat("emissive", 1.0f); // Enable emissive mode
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 0.6f)); // Bright yellow-white
        bulb.draw();
        shader.setFloat("emissive", 0.0f); // Disable emissive
    }
    
    // Restore original lighting
    shader.setVec3("lightPos", originalLightPos);
    shader.setVec3("lightColor", originalLightColor);
}
