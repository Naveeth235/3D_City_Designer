#include "citygenerator.h"
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>

CityGenerator::CityGenerator() : layoutSize(600) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void CityGenerator::generateCity(int numBuildings, int layoutSize, RoadType roadType, SkylineType skylineType) {
    clear();
    
    this->layoutSize = layoutSize;
    this->currentRoadType = roadType;
    this->currentSkylineType = skylineType;
    
    generateRoads(roadType, layoutSize);
    generateBuildings(numBuildings, skylineType, layoutSize);
    generateParks(3, layoutSize);
    generateVehicles(8);
    generateStreetLights();
}

void CityGenerator::generateRoads(RoadType type, int size) {
    switch (type) {
        case RoadType::GRID:
            generateGridRoads(size);
            break;
        case RoadType::RADIAL:
            generateRadialRoads(size);
            break;
        case RoadType::RANDOM:
            generateRandomRoads(size);
            break;
    }
}

void CityGenerator::generateGridRoads(int size) {
    int spacing = 100;
    int numLines = size / spacing;
    
    // Vertical roads
    for (int i = 1; i < numLines; ++i) {
        int x = i * spacing;
        roads.push_back({Point2D(x, 0), Point2D(x, size)});
    }
    
    // Horizontal roads
    for (int i = 1; i < numLines; ++i) {
        int y = i * spacing;
        roads.push_back({Point2D(0, y), Point2D(size, y)});
    }
}

void CityGenerator::generateRadialRoads(int size) {
    int centerX = size / 2;
    int centerY = size / 2;
    int numSpokes = 8;
    int radius = size / 2;
    
    // Radial spokes
    for (int i = 0; i < numSpokes; ++i) {
        float angle = (2.0f * 3.14159f * i) / numSpokes;
        int endX = centerX + static_cast<int>(radius * cos(angle));
        int endY = centerY + static_cast<int>(radius * sin(angle));
        roads.push_back({Point2D(centerX, centerY), Point2D(endX, endY)});
    }
    
    // Concentric circles (approximated with line segments)
    int numRings = 3;
    for (int ring = 1; ring <= numRings; ++ring) {
        int ringRadius = (radius * ring) / (numRings + 1);
        int numSegments = 32;
        
        for (int i = 0; i < numSegments; ++i) {
            float angle1 = (2.0f * 3.14159f * i) / numSegments;
            float angle2 = (2.0f * 3.14159f * (i + 1)) / numSegments;
            
            int x1 = centerX + static_cast<int>(ringRadius * cos(angle1));
            int y1 = centerY + static_cast<int>(ringRadius * sin(angle1));
            int x2 = centerX + static_cast<int>(ringRadius * cos(angle2));
            int y2 = centerY + static_cast<int>(ringRadius * sin(angle2));
            
            roads.push_back({Point2D(x1, y1), Point2D(x2, y2)});
        }
    }
}

void CityGenerator::generateRandomRoads(int size) {
    int numRoads = 15;
    
    for (int i = 0; i < numRoads; ++i) {
        int x1 = std::rand() % size;
        int y1 = std::rand() % size;
        int x2 = std::rand() % size;
        int y2 = std::rand() % size;
        
        roads.push_back({Point2D(x1, y1), Point2D(x2, y2)});
    }
}

void CityGenerator::generateBuildings(int numBuildings, SkylineType skylineType, int layoutSize) {
    int attempts = 0;
    int maxAttempts = numBuildings * 10;
    
    while (buildings.size() < static_cast<size_t>(numBuildings) && attempts < maxAttempts) {
        glm::vec2 size(30.0f + (std::rand() % 40), 30.0f + (std::rand() % 40));
        glm::vec2 pos(std::rand() % (layoutSize - static_cast<int>(size.x)), 
                      std::rand() % (layoutSize - static_cast<int>(size.y)));
        
        if (isValidBuildingPosition(pos, size, layoutSize)) {
            Building building;
            building.position = pos;
            building.size = size;
            building.height = getHeightForSkyline(skylineType);
            building.textureIndex = std::rand() % 2;
            
            buildings.push_back(building);
        }
        
        attempts++;
    }
}

void CityGenerator::generateParks(int numParks, int layoutSize) {
    // Create a single centered water pond
    int centerX = layoutSize / 2;
    int centerY = layoutSize / 2;
    int radius = 60; // Nice big pond
    
    parks.push_back({Point2D(centerX, centerY), radius});
}

bool CityGenerator::isValidBuildingPosition(const glm::vec2& pos, const glm::vec2& size, int layoutSize) {
    // Check overlap with pond (centered at layoutSize/2 with radius 60)
    float pondCenterX = layoutSize / 2.0f;
    float pondCenterY = layoutSize / 2.0f;
    float pondRadius = 60.0f;
    float minSafeDistance = pondRadius + 10.0f; // 10 unit buffer
    
    // Check all corners and center of building against pond
    glm::vec2 buildingCenter = pos + size * 0.5f;
    float distToPondCenter = std::sqrt(
        (buildingCenter.x - pondCenterX) * (buildingCenter.x - pondCenterX) +
        (buildingCenter.y - pondCenterY) * (buildingCenter.y - pondCenterY)
    );
    
    // Check if building's bounding circle overlaps with pond
    float buildingRadius = std::sqrt(size.x * size.x + size.y * size.y) * 0.5f;
    if (distToPondCenter < minSafeDistance + buildingRadius) {
        return false;
    }
    
    // Check overlap with existing buildings
    for (const auto& building : buildings) {
        if (pos.x < building.position.x + building.size.x + 10.0f &&
            pos.x + size.x + 10.0f > building.position.x &&
            pos.y < building.position.y + building.size.y + 10.0f &&
            pos.y + size.y + 10.0f > building.position.y) {
            return false;
        }
    }
    
    return true;
}

float CityGenerator::getHeightForSkyline(SkylineType type) {
    switch (type) {
        case SkylineType::LOW_RISE:
            return 20.0f + (std::rand() % 30);
        case SkylineType::MID_RISE:
            return 50.0f + (std::rand() % 50);
        case SkylineType::SKYSCRAPER:
            return 100.0f + (std::rand() % 100);
        default:
            return 50.0f;
    }
}

void CityGenerator::clear() {
    buildings.clear();
    roads.clear();
    parks.clear();
    vehicles.clear();
    streetLights.clear();
}

void CityGenerator::generateVehicles(int numVehicles) {
    if (roads.empty()) return;
    
    for (int i = 0; i < numVehicles; ++i) {
        const Road& road = roads[std::rand() % roads.size()];
        
        Vehicle vehicle;
        vehicle.position = glm::vec3(road.start.x, 5.0f, road.start.y);
        
        glm::vec3 roadEnd(road.end.x, 5.0f, road.end.y);
        vehicle.direction = glm::normalize(roadEnd - vehicle.position);
        vehicle.speed = 20.0f + (std::rand() % 20); // 20-40 units per second
        vehicle.pathIndex = 0;
        
        // Create simple path along the road
        vehicle.path.push_back(vehicle.position);
        vehicle.path.push_back(roadEnd);
        
        vehicles.push_back(vehicle);
    }
}

void CityGenerator::generateStreetLights() {
    // Place street lights along roads at regular intervals
    for (const auto& road : roads) {
        float dx = road.end.x - road.start.x;
        float dy = road.end.y - road.start.y;
        float length = std::sqrt(dx * dx + dy * dy);
        
        int numLights = static_cast<int>(length / 50.0f);
        
        for (int i = 1; i < numLights; ++i) {
            float t = static_cast<float>(i) / numLights;
            float x = road.start.x + dx * t;
            float y = road.start.y + dy * t;
            
            StreetLight light;
            light.position = glm::vec3(x, 15.0f, y); // 15 units high
            streetLights.push_back(light);
        }
    }
}

void CityGenerator::updateVehicles(float deltaTime) {
    for (auto& vehicle : vehicles) {
        if (vehicle.path.size() < 2) continue;
        
        // Move vehicle along its direction
        vehicle.position += vehicle.direction * vehicle.speed * deltaTime;
        
        // Check if reached end of current path segment
        glm::vec3 target = vehicle.path[vehicle.pathIndex + 1];
        float distToTarget = glm::length(target - vehicle.position);
        
        if (distToTarget < 5.0f) {
            // Move to next path segment or loop back
            vehicle.pathIndex++;
            if (vehicle.pathIndex >= static_cast<int>(vehicle.path.size()) - 1) {
                // Loop back to start
                vehicle.pathIndex = 0;
                vehicle.position = vehicle.path[0];
            }
            
            // Update direction for next segment
            if (vehicle.pathIndex < static_cast<int>(vehicle.path.size()) - 1) {
                target = vehicle.path[vehicle.pathIndex + 1];
                vehicle.direction = glm::normalize(target - vehicle.position);
            }
        }
    }
}
