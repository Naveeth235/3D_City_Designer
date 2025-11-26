#ifndef CITYGENERATOR_H
#define CITYGENERATOR_H

#include <vector>
#include <glm/glm.hpp>
#include "renderer2d.h"

enum class RoadType {
    GRID,
    RADIAL,
    RANDOM
};

enum class SkylineType {
    LOW_RISE,
    MID_RISE,
    SKYSCRAPER
};

struct Building {
    glm::vec2 position;
    glm::vec2 size;
    float height;
    int textureIndex;
};

struct Road {
    Point2D start;
    Point2D end;
};

struct Park {
    Point2D center;
    int radius;
};

struct Vehicle {
    glm::vec3 position;
    glm::vec3 direction;
    float speed;
    int pathIndex;
    std::vector<glm::vec3> path;
};

struct StreetLight {
    glm::vec3 position;
};

class CityGenerator {
public:
    CityGenerator();
    
    void generateCity(int numBuildings, int layoutSize, RoadType roadType, SkylineType skylineType);
    void generateSampleCity(); // Generate a nice sample city with 20 buildings
    void generateRoads(RoadType type, int layoutSize);
    void generateBuildings(int numBuildings, SkylineType skylineType, int layoutSize);
    void generateParks(int numParks, int layoutSize);
    
    void clear();
    
    // Getters
    const std::vector<Building>& getBuildings() const { return buildings; }
    std::vector<Building>& getBuildings() { return buildings; } // Non-const for editing
    const std::vector<Road>& getRoads() const { return roads; }
    const std::vector<Park>& getParks() const { return parks; }
    const std::vector<Vehicle>& getVehicles() const { return vehicles; }
    const std::vector<StreetLight>& getStreetLights() const { return streetLights; }
    
    int getLayoutSize() const { return layoutSize; }
    
    void updateVehicles(float deltaTime);
    
    // Manual object placement
    void addBuilding(const Building& building);
    void addRoad(const Road& road);
    void addPark(const Park& park);
    
    // Delete objects
    bool deleteBuildingAt(const glm::vec2& pos);
    bool deleteRoadAt(const glm::vec2& pos, float threshold);
    bool deleteParkAt(const glm::vec2& pos);
    
private:
    std::vector<Building> buildings;
    std::vector<Road> roads;
    std::vector<Park> parks;
    std::vector<Vehicle> vehicles;
    std::vector<StreetLight> streetLights;
    
    int layoutSize;
    RoadType currentRoadType;
    SkylineType currentSkylineType;
    
    void generateGridRoads(int size);
    void generateRadialRoads(int size);
    void generateRandomRoads(int size);
    void generateVehicles(int numVehicles);
    void generateStreetLights();
    
    bool isValidBuildingPosition(const glm::vec2& pos, const glm::vec2& size, int layoutSize);
    float getHeightForSkyline(SkylineType type);
};

#endif
