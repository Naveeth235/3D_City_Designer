#ifndef RENDERER3D_H
#define RENDERER3D_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"
#include "texture.h"
#include "citygenerator.h"

struct Camera {
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    
    float yaw;
    float pitch;
    float fov;
    
    Camera();
    void updateVectors();
    glm::mat4 getViewMatrix();
};

struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
    
    Mesh();
    ~Mesh();
    void setup();
    void draw();
};

class Renderer3D {
public:
    Renderer3D();
    ~Renderer3D();
    
    void init(int screenWidth, int screenHeight);
    void render(const CityGenerator& cityGen);
    
    void updateCamera(float deltaTime, bool* keys, float mouseOffsetX, float mouseOffsetY);
    void setProjection(int width, int height);
    void updateTimeOfDay(float deltaTime);
    
    Camera& getCamera() { return camera; }
    float getTimeOfDay() const { return timeOfDay; }
    void setTimeSpeed(float speed) { timeSpeed = speed; }
    
private:
    Shader shader;
    Texture buildingTexture1;
    Texture buildingTexture2;
    Texture roadTexture;
    Texture grassTexture;
    Texture fountainTexture;
    
    Camera camera;
    int width, height;
    
    float timeOfDay; // 0.0 to 24.0 hours
    float timeSpeed; // Speed multiplier for time progression
    
    Mesh groundMesh;
    Mesh buildingMesh;
    Mesh roadMesh;
    Mesh parkMesh;
    
    void createGroundMesh(int size);
    void createBuildingMesh(const Building& building);
    void createRoadMesh(const std::vector<Road>& roads);
    void createParkMesh(const Park& park);
    
    void renderGround(int size);
    void renderBuildings(const std::vector<Building>& buildings);
    void renderRoads(const std::vector<Road>& roads);
    void renderParks(const std::vector<Park>& parks);
    void renderVehicles(const std::vector<Vehicle>& vehicles);
    void renderStreetLights(const std::vector<StreetLight>& lights);
    
    Mesh createCubeMesh(float width, float height, float depth);
    Mesh createCylinderMesh(float radius, float height, int segments);
    
    glm::vec3 getSkyColor() const;
    glm::vec3 getSunLightColor() const;
    glm::vec3 getSunPosition() const;
    bool isNightTime() const;
};

#endif
