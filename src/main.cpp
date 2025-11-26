// ============================================================================
// INTERACTIVE 3D CITY DESIGNER
// Computer Graphics Assignment - Part 1
// 
// This application demonstrates:
// - Basic OpenGL Lines for roads and boundaries
// - Bresenham's Line Algorithm for pixel-perfect 2D road layouts
// - Midpoint Circle Algorithm for circular parks and fountains
// - 3D Model Rendering with Texture Mapping for buildings
// - User-driven interactive city design
// ============================================================================

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <limits>

#include "citygenerator.h"
#include "renderer2d.h"
#include "renderer3d.h"
#include "textrenderer.h"

// ============================================================================
// WINDOW CONFIGURATION
// ============================================================================
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// ============================================================================
// APPLICATION STATE
// ============================================================================
enum class AppMode {
    MODE_2D,    // Top-down planning view (Bresenham lines & Midpoint circles)
    MODE_3D     // 3D exploration view (textured buildings)
};

AppMode currentMode = AppMode::MODE_2D;

// ============================================================================
// USER-CONFIGURABLE PARAMETERS (Set at startup via console input)
// ============================================================================
int userLayoutSize = 600;           // City grid size
int userNumBuildings = 20;          // Number of buildings
RoadType userRoadType = RoadType::GRID;  // Road pattern (grid/radial/random)
SkylineType userSkylineType = SkylineType::MID_RISE;  // Building heights
int userParkRadius = 50;            // Park/fountain size (Midpoint Circle)
int userTextureTheme = 0;           // Building facade texture (0-2)

// ============================================================================
// OBJECT SELECTION & MOVEMENT
// ============================================================================
int selectedBuildingIndex = -1;     // Currently selected building (-1 = none)
const float MOVE_SPEED = 5.0f;      // Movement speed with arrow keys

// ============================================================================
// NEW BUILDING CREATION MODE
// ============================================================================
bool isAddingNewBuilding = false;   // True when in "Add Building" mode
Building newBuildingPreview;        // Preview of building being placed
const int DEFAULT_BUILDING_WIDTH = 40;
const int DEFAULT_BUILDING_DEPTH = 40;
const int DEFAULT_BUILDING_HEIGHT = 80;

// ============================================================================
// CAMERA & INPUT STATE
// ============================================================================
double lastX = SCREEN_WIDTH / 2.0;
double lastY = SCREEN_HEIGHT / 2.0;
bool firstMouse = true;
bool keys[6] = {false};             // W, S, A, D, Space, Shift for 3D camera
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
CityGenerator cityGen;
Renderer2D* renderer2D = nullptr;
Renderer3D* renderer3D = nullptr;
TextRenderer* textRenderer = nullptr;
bool showHelp = true;

// ============================================================================
// FUNCTION DECLARATIONS
// ============================================================================
void getUserInputs();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void selectNearestBuilding(const glm::vec2& worldPos);
void moveSelectedBuilding(int dx, int dy);
void displayWelcomeMessage();
void displayControls();
glm::vec2 screenToWorld(const glm::vec2& screenPos, int screenWidth, int screenHeight);

// ============================================================================
// MAIN ENTRY POINT
// ============================================================================
int main() {
    // Display welcome message
    displayWelcomeMessage();
    
    // ===== USER INPUT PHASE =====
    // Gather all user preferences before starting the application
    getUserInputs();
    
    // ===== GLFW INITIALIZATION =====
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // Create window
    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, 
                                         "Interactive 3D City Designer", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // ===== GLAD INITIALIZATION =====
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Enable OpenGL features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // ===== RENDERER INITIALIZATION =====
    renderer2D = new Renderer2D();
    renderer2D->init(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    renderer3D = new Renderer3D();
    renderer3D->init(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    textRenderer = new TextRenderer();
    textRenderer->init(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // ===== CITY GENERATION =====
    // Generate city based on user inputs
    std::cout << "\n[GENERATING CITY...]" << std::endl;
    cityGen.generateCity(userNumBuildings, userLayoutSize, userRoadType, userSkylineType);
    
    // Add park with user-specified radius (using Midpoint Circle Algorithm)
    Park centralPark;
    centralPark.center = Point2D(userLayoutSize / 2, userLayoutSize / 2);
    centralPark.radius = userParkRadius;
    cityGen.addPark(centralPark);
    
    std::cout << "[CITY GENERATED SUCCESSFULLY]" << std::endl;
    std::cout << "\n========================================" << std::endl;
    std::cout << "CITY IS READY! Opening 3D window..." << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    displayControls();
    
    // ===== MAIN RENDERING LOOP =====
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time for smooth animations
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        processInput(window);
        
        // Update animations in 3D mode
        if (currentMode == AppMode::MODE_3D) {
            renderer3D->updateTimeOfDay(deltaTime);
            cityGen.updateVehicles(deltaTime);
        }
        
        // Clear screen
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // ===== 2D RENDERING (BRESENHAM & MIDPOINT CIRCLE) =====
        if (currentMode == AppMode::MODE_2D) {
            glDisable(GL_DEPTH_TEST);
            
            renderer2D->clearElements();
            
            // Draw roads using Bresenham's Line Algorithm
            for (const auto& road : cityGen.getRoads()) {
                Line2D line(road.start, road.end, glm::vec3(0.3f, 0.3f, 0.3f));
                renderer2D->addLine(line);
            }
            
            // Draw parks using Midpoint Circle Algorithm
            for (const auto& park : cityGen.getParks()) {
                glm::vec3 parkColor = glm::vec3(0.0f, 0.8f, 0.2f); // Green
                Circle2D circle(park.center, park.radius, parkColor);
                renderer2D->addCircle(circle);
            }
            
            // Draw building outlines
            const auto& buildings = cityGen.getBuildings();
            for (size_t i = 0; i < buildings.size(); ++i) {
                const auto& building = buildings[i];
                
                // Highlight selected building
                glm::vec3 buildingColor = (i == selectedBuildingIndex) ? 
                    glm::vec3(1.0f, 1.0f, 0.0f) :  // Yellow if selected
                    glm::vec3(0.7f, 0.7f, 0.9f);    // Gray-blue otherwise
                
                int x1 = building.position.x;
                int y1 = building.position.y;
                int x2 = building.position.x + building.size.x;
                int y2 = building.position.y + building.size.y;
                
                renderer2D->addLine(Line2D(Point2D(x1, y1), Point2D(x2, y1), buildingColor));
                renderer2D->addLine(Line2D(Point2D(x2, y1), Point2D(x2, y2), buildingColor));
                renderer2D->addLine(Line2D(Point2D(x2, y2), Point2D(x1, y2), buildingColor));
                renderer2D->addLine(Line2D(Point2D(x1, y2), Point2D(x1, y1), buildingColor));
            }
            
            // Draw new building preview if in add mode
            if (isAddingNewBuilding) {
                glm::vec3 previewColor(0.0f, 1.0f, 1.0f); // Cyan for preview
                int x1 = newBuildingPreview.position.x;
                int y1 = newBuildingPreview.position.y;
                int x2 = newBuildingPreview.position.x + newBuildingPreview.size.x;
                int y2 = newBuildingPreview.position.y + newBuildingPreview.size.y;
                
                renderer2D->addLine(Line2D(Point2D(x1, y1), Point2D(x2, y1), previewColor));
                renderer2D->addLine(Line2D(Point2D(x2, y1), Point2D(x2, y2), previewColor));
                renderer2D->addLine(Line2D(Point2D(x2, y2), Point2D(x1, y2), previewColor));
                renderer2D->addLine(Line2D(Point2D(x1, y2), Point2D(x1, y1), previewColor));
            }
            
            renderer2D->render();
            glEnable(GL_DEPTH_TEST);
        }
        // ===== 3D RENDERING (TEXTURED BUILDINGS) =====
        else {
            renderer3D->updateCamera(deltaTime, keys, 0.0f, 0.0f);
            renderer3D->render(cityGen);
        }
        
        // ===== ON-SCREEN UI =====
        if (showHelp) {
            glDisable(GL_DEPTH_TEST);
            
            float y = 10.0f;
            float scale = 1.5f;
            glm::vec3 titleColor(1.0f, 1.0f, 0.3f);
            glm::vec3 textColor(0.9f, 0.9f, 0.9f);
            glm::vec3 highlightColor(0.3f, 1.0f, 0.5f);
            
            textRenderer->renderText("INTERACTIVE CITY DESIGNER", 10, y, scale * 1.2f, titleColor);
            y += 12 * scale;
            
            // Display current view mode
            std::string modeText = currentMode == AppMode::MODE_2D ? "Mode: 2D PLANNING" : "Mode: 3D EXPLORATION";
            textRenderer->renderText(modeText, 10, y, scale, highlightColor);
            y += 12 * scale;
            
            textRenderer->renderText("CONTROLS:", 10, y, scale, highlightColor);
            y += 8 * scale;
            textRenderer->renderText("ENTER - Switch 2D/3D", 10, y, scale * 0.9f, textColor);
            y += 8 * scale;
            textRenderer->renderText("H - Toggle help", 10, y, scale * 0.9f, textColor);
            y += 8 * scale;
            textRenderer->renderText("ESC - Exit", 10, y, scale * 0.9f, textColor);
            y += 12 * scale;
            
            if (currentMode == AppMode::MODE_2D) {
                if (isAddingNewBuilding) {
                    textRenderer->renderText("ADD BUILDING MODE:", 10, y, scale, glm::vec3(0.0f, 1.0f, 1.0f));
                    y += 8 * scale;
                    textRenderer->renderText("Arrow Keys - Position", 10, y, scale * 0.9f, textColor);
                    y += 8 * scale;
                    textRenderer->renderText("+/- - Width", 10, y, scale * 0.9f, textColor);
                    y += 8 * scale;
                    textRenderer->renderText("[ ] - Depth", 10, y, scale * 0.9f, textColor);
                    y += 8 * scale;
                    textRenderer->renderText("PgUp/PgDn - Height", 10, y, scale * 0.9f, textColor);
                    y += 8 * scale;
                    textRenderer->renderText("ENTER - Place", 10, y, scale * 0.9f, glm::vec3(0.0f, 1.0f, 0.0f));
                    y += 8 * scale;
                    textRenderer->renderText("ESC - Cancel", 10, y, scale * 0.9f, glm::vec3(1.0f, 0.5f, 0.0f));
                    y += 8 * scale;
                    
                    // Show current dimensions
                    std::string dimText = "Size: " + std::to_string(newBuildingPreview.size.x) + "x" + 
                                         std::to_string(newBuildingPreview.size.y) + "x" + 
                                         std::to_string(newBuildingPreview.height);
                    textRenderer->renderText(dimText, 10, y, scale * 0.9f, glm::vec3(1.0f, 1.0f, 0.0f));
                } else {
                    textRenderer->renderText("2D MODE:", 10, y, scale, highlightColor);
                    y += 8 * scale;
                    textRenderer->renderText("Click - Select building", 10, y, scale * 0.9f, textColor);
                    y += 8 * scale;
                    textRenderer->renderText("Arrow Keys - Move building", 10, y, scale * 0.9f, textColor);
                    y += 8 * scale;
                    textRenderer->renderText("N - Add new building", 10, y, scale * 0.9f, textColor);
                    y += 8 * scale;
                    
                    if (selectedBuildingIndex >= 0) {
                        textRenderer->renderText("Building SELECTED!", 10, y, scale, glm::vec3(1.0f, 1.0f, 0.0f));
                        y += 8 * scale;
                    }
                }
            } else {
                textRenderer->renderText("3D MODE:", 10, y, scale, highlightColor);
                y += 8 * scale;
                textRenderer->renderText("W/A/S/D - Move", 10, y, scale * 0.9f, textColor);
                y += 8 * scale;
                textRenderer->renderText("SPACE/SHIFT - Up/Down", 10, y, scale * 0.9f, textColor);
                y += 8 * scale;
                textRenderer->renderText("Right Mouse - Look", 10, y, scale * 0.9f, textColor);
                y += 8 * scale;
                textRenderer->renderText("T/Y - Time speed", 10, y, scale * 0.9f, textColor);
            }
            
            glEnable(GL_DEPTH_TEST);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // ===== CLEANUP =====
    delete renderer2D;
    delete renderer3D;
    delete textRenderer;
    
    glfwTerminate();
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Thank you for using Interactive City Designer!" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    return 0;
}

// ============================================================================
// USER INPUT FUNCTIONS
// ============================================================================

/**
 * Gather all user inputs via console before starting the graphical application
 * This allows users to customize:
 * - City size and building count
 * - Road network pattern (demonstrates Bresenham's Line Algorithm)
 * - Skyline pattern (building height distribution)
 * - Park size (demonstrates Midpoint Circle Algorithm)
 * - Texture theme for building facades
 */
void getUserInputs() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  CITY CONFIGURATION" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Layout size input
    std::cout << "Enter city layout size (recommended 400-800): ";
    std::cin >> userLayoutSize;
    if (std::cin.fail() || userLayoutSize < 200 || userLayoutSize > 1000) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        userLayoutSize = 600;
        std::cout << "Invalid input. Using default: 600\n" << std::endl;
    }
    
    // Number of buildings
    std::cout << "Enter number of buildings (5-50): ";
    std::cin >> userNumBuildings;
    if (std::cin.fail() || userNumBuildings < 5 || userNumBuildings > 50) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        userNumBuildings = 20;
        std::cout << "Invalid input. Using default: 20\n" << std::endl;
    }
    
    // Road network pattern (Bresenham's Line Algorithm demonstration)
    std::cout << "\nRoad Network Pattern (demonstrates Bresenham's Line Algorithm):" << std::endl;
    std::cout << "  1 - Grid (perpendicular roads)" << std::endl;
    std::cout << "  2 - Radial (roads from center)" << std::endl;
    std::cout << "  3 - Random (organic pattern)" << std::endl;
    std::cout << "Enter choice (1-3): ";
    int roadChoice;
    std::cin >> roadChoice;
    if (std::cin.fail() || roadChoice < 1 || roadChoice > 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        roadChoice = 1;
        std::cout << "Invalid input. Using default: Grid\n" << std::endl;
    }
    switch(roadChoice) {
        case 1: userRoadType = RoadType::GRID; break;
        case 2: userRoadType = RoadType::RADIAL; break;
        case 3: userRoadType = RoadType::RANDOM; break;
        default: userRoadType = RoadType::GRID;
    }
    
    // Skyline pattern (building height distribution)
    std::cout << "\nSkyline Pattern (building height distribution):" << std::endl;
    std::cout << "  1 - Low-rise (suburban, 20-50 units)" << std::endl;
    std::cout << "  2 - Mid-rise (mixed urban, 50-100 units)" << std::endl;
    std::cout << "  3 - Skyscraper (downtown, 100-200 units)" << std::endl;
    std::cout << "Enter choice (1-3): ";
    int skylineChoice;
    std::cin >> skylineChoice;
    if (std::cin.fail() || skylineChoice < 1 || skylineChoice > 3) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        skylineChoice = 2;
        std::cout << "Invalid input. Using default: Mid-rise\n" << std::endl;
    }
    switch(skylineChoice) {
        case 1: userSkylineType = SkylineType::LOW_RISE; break;
        case 2: userSkylineType = SkylineType::MID_RISE; break;
        case 3: userSkylineType = SkylineType::SKYSCRAPER; break;
        default: userSkylineType = SkylineType::MID_RISE;
    }
    
    // Park/fountain size (Midpoint Circle Algorithm demonstration)
    std::cout << "\nPark/Fountain size (demonstrates Midpoint Circle Algorithm):" << std::endl;
    std::cout << "Enter radius (20-100): ";
    std::cin >> userParkRadius;
    if (std::cin.fail() || userParkRadius < 20 || userParkRadius > 100) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        userParkRadius = 50;
        std::cout << "Invalid input. Using default: 50\n" << std::endl;
    }
    
    // Texture theme for building facades (3D texture mapping demonstration)
    std::cout << "\nBuilding Texture Theme (3D texture mapping):" << std::endl;
    std::cout << "  0 - Modern glass/concrete" << std::endl;
    std::cout << "  1 - Classic brick" << std::endl;
    std::cout << "  2 - Mixed style" << std::endl;
    std::cout << "Enter choice (0-2): ";
    std::cin >> userTextureTheme;
    if (std::cin.fail() || userTextureTheme < 0 || userTextureTheme > 2) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        userTextureTheme = 0;
        std::cout << "Invalid input. Using default: Modern\n" << std::endl;
    }
    
    // Clear input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    // Summary
    std::cout << "\n========================================" << std::endl;
    std::cout << "CONFIGURATION SUMMARY:" << std::endl;
    std::cout << "  Layout Size: " << userLayoutSize << "x" << userLayoutSize << std::endl;
    std::cout << "  Buildings: " << userNumBuildings << std::endl;
    std::cout << "  Road Pattern: " << (roadChoice == 1 ? "Grid" : roadChoice == 2 ? "Radial" : "Random") << std::endl;
    std::cout << "  Skyline: " << (skylineChoice == 1 ? "Low-rise" : skylineChoice == 2 ? "Mid-rise" : "Skyscraper") << std::endl;
    std::cout << "  Park Radius: " << userParkRadius << std::endl;
    std::cout << "  Texture Theme: " << userTextureTheme << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void displayWelcomeMessage() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  INTERACTIVE 3D CITY DESIGNER" << std::endl;
    std::cout << "  Computer Graphics Assignment - Part 1" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nFEATURES DEMONSTRATED:" << std::endl;
    std::cout << "  ✓ Basic OpenGL Lines (roads, boundaries)" << std::endl;
    std::cout << "  ✓ Bresenham's Line Algorithm (2D roads)" << std::endl;
    std::cout << "  ✓ Midpoint Circle Algorithm (parks)" << std::endl;
    std::cout << "  ✓ 3D Texture Mapping (buildings)" << std::endl;
    std::cout << "  ✓ Interactive object placement & movement" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

void displayControls() {
    std::cout << "\nINTERACTIVE CONTROLS:" << std::endl;
    std::cout << "  ENTER       - Switch between 2D planning and 3D exploration" << std::endl;
    std::cout << "  H           - Toggle help overlay" << std::endl;
    std::cout << "  ESC         - Exit application" << std::endl;
    std::cout << "\n2D MODE (City Planning):" << std::endl;
    std::cout << "  Left Click  - Select a building" << std::endl;
    std::cout << "  Arrow Keys  - Move selected building (↑↓←→)" << std::endl;
    std::cout << "  N           - Start adding NEW building" << std::endl;
    std::cout << "\nADD BUILDING MODE:" << std::endl;
    std::cout << "  Arrow Keys  - Position new building (↑↓←→)" << std::endl;
    std::cout << "  +/-         - Adjust width" << std::endl;
    std::cout << "  [/]         - Adjust depth" << std::endl;
    std::cout << "  Page Up/Dn  - Adjust height" << std::endl;
    std::cout << "  ENTER       - Confirm placement" << std::endl;
    std::cout << "  ESC         - Cancel" << std::endl;
    std::cout << "\n3D MODE (Exploration):" << std::endl;
    std::cout << "  W/A/S/D     - Move camera (forward/left/back/right)" << std::endl;
    std::cout << "  SPACE/SHIFT - Move camera up/down" << std::endl;
    std::cout << "  Right Mouse - Look around (hold and drag)" << std::endl;
    std::cout << "  T/Y         - Time speed (fast/normal)" << std::endl;
    std::cout << "\n========================================\n" << std::endl;
}

// ============================================================================
// CALLBACK FUNCTIONS
// ============================================================================

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (renderer3D) {
        renderer3D->setProjection(width, height);
    }
    if (textRenderer) {
        textRenderer->setProjection(width, height);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (currentMode == AppMode::MODE_3D) {
        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }
        
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos;
        
        lastX = xpos;
        lastY = ypos;
        
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            renderer3D->updateCamera(deltaTime, keys, xoffset, yoffset);
        }
    }
}

/**
 * Handle keyboard input
 * - ENTER: Switch between 2D and 3D modes
 * - Arrow keys: Move selected building in 2D mode
 * - WASD: Camera movement in 3D mode
 * - H: Toggle help
 * - ESC: Exit
 */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        // Mode switching
        if (key == GLFW_KEY_ENTER) {
            if (currentMode == AppMode::MODE_2D) {
                currentMode = AppMode::MODE_3D;
                selectedBuildingIndex = -1; // Deselect when entering 3D
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                std::cout << "[MODE] Switched to 3D exploration" << std::endl;
            } else {
                currentMode = AppMode::MODE_2D;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                std::cout << "[MODE] Switched to 2D planning" << std::endl;
            }
        }
        
        // Toggle help
        if (key == GLFW_KEY_H) {
            showHelp = !showHelp;
            std::cout << "[UI] Help overlay: " << (showHelp ? "ON" : "OFF") << std::endl;
        }
        
        // Start new building mode (N key in 2D)
        if (currentMode == AppMode::MODE_2D && key == GLFW_KEY_N && !isAddingNewBuilding) {
            isAddingNewBuilding = true;
            selectedBuildingIndex = -1; // Deselect any selected building
            
            // Initialize new building at center
            newBuildingPreview.position.x = userLayoutSize / 2 - DEFAULT_BUILDING_WIDTH / 2;
            newBuildingPreview.position.y = userLayoutSize / 2 - DEFAULT_BUILDING_DEPTH / 2;
            newBuildingPreview.size.x = DEFAULT_BUILDING_WIDTH;
            newBuildingPreview.size.y = DEFAULT_BUILDING_DEPTH;
            newBuildingPreview.height = DEFAULT_BUILDING_HEIGHT;
            
            std::cout << "[ADD] Entering Add Building mode. Use arrows to position, +/- [] PgUp/PgDn to adjust size, ENTER to place, ESC to cancel" << std::endl;
        }
        
        // Handle new building mode controls
        if (isAddingNewBuilding && currentMode == AppMode::MODE_2D) {
            // Movement
            if (key == GLFW_KEY_UP) {
                newBuildingPreview.position.y += MOVE_SPEED;
                if (newBuildingPreview.position.y + newBuildingPreview.size.y > userLayoutSize) {
                    newBuildingPreview.position.y = userLayoutSize - newBuildingPreview.size.y;
                }
            }
            if (key == GLFW_KEY_DOWN) {
                newBuildingPreview.position.y -= MOVE_SPEED;
                if (newBuildingPreview.position.y < 0) {
                    newBuildingPreview.position.y = 0;
                }
            }
            if (key == GLFW_KEY_LEFT) {
                newBuildingPreview.position.x -= MOVE_SPEED;
                if (newBuildingPreview.position.x < 0) {
                    newBuildingPreview.position.x = 0;
                }
            }
            if (key == GLFW_KEY_RIGHT) {
                newBuildingPreview.position.x += MOVE_SPEED;
                if (newBuildingPreview.position.x + newBuildingPreview.size.x > userLayoutSize) {
                    newBuildingPreview.position.x = userLayoutSize - newBuildingPreview.size.x;
                }
            }
            
            // Width adjustment (+/-)
            if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) {
                newBuildingPreview.size.x += 5;
                if (newBuildingPreview.size.x > 100) newBuildingPreview.size.x = 100;
                std::cout << "[SIZE] Width: " << newBuildingPreview.size.x << std::endl;
            }
            if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
                newBuildingPreview.size.x -= 5;
                if (newBuildingPreview.size.x < 20) newBuildingPreview.size.x = 20;
                std::cout << "[SIZE] Width: " << newBuildingPreview.size.x << std::endl;
            }
            
            // Depth adjustment ([ ])
            if (key == GLFW_KEY_LEFT_BRACKET) {
                newBuildingPreview.size.y -= 5;
                if (newBuildingPreview.size.y < 20) newBuildingPreview.size.y = 20;
                std::cout << "[SIZE] Depth: " << newBuildingPreview.size.y << std::endl;
            }
            if (key == GLFW_KEY_RIGHT_BRACKET) {
                newBuildingPreview.size.y += 5;
                if (newBuildingPreview.size.y > 100) newBuildingPreview.size.y = 100;
                std::cout << "[SIZE] Depth: " << newBuildingPreview.size.y << std::endl;
            }
            
            // Height adjustment (Page Up/Down)
            if (key == GLFW_KEY_PAGE_UP) {
                newBuildingPreview.height += 10;
                if (newBuildingPreview.height > 200) newBuildingPreview.height = 200;
                std::cout << "[SIZE] Height: " << newBuildingPreview.height << std::endl;
            }
            if (key == GLFW_KEY_PAGE_DOWN) {
                newBuildingPreview.height -= 10;
                if (newBuildingPreview.height < 20) newBuildingPreview.height = 20;
                std::cout << "[SIZE] Height: " << newBuildingPreview.height << std::endl;
            }
            
            // Confirm placement (ENTER)
            if (key == GLFW_KEY_ENTER) {
                cityGen.addBuilding(newBuildingPreview);
                std::cout << "[ADD] Building placed at (" << newBuildingPreview.position.x 
                          << ", " << newBuildingPreview.position.y << ") - Size: " 
                          << newBuildingPreview.size.x << "x" << newBuildingPreview.size.y 
                          << "x" << newBuildingPreview.height << std::endl;
                isAddingNewBuilding = false;
            }
            
            // Cancel (ESC in add mode)
            if (key == GLFW_KEY_ESCAPE) {
                isAddingNewBuilding = false;
                std::cout << "[ADD] Building placement cancelled" << std::endl;
            }
        }
        // Arrow keys for building movement in 2D mode (only when not adding)
        else if (currentMode == AppMode::MODE_2D && selectedBuildingIndex >= 0 && !isAddingNewBuilding) {
            if (key == GLFW_KEY_UP) {
                moveSelectedBuilding(0, MOVE_SPEED);
            }
            if (key == GLFW_KEY_DOWN) {
                moveSelectedBuilding(0, -MOVE_SPEED);
            }
            if (key == GLFW_KEY_LEFT) {
                moveSelectedBuilding(-MOVE_SPEED, 0);
            }
            if (key == GLFW_KEY_RIGHT) {
                moveSelectedBuilding(MOVE_SPEED, 0);
            }
        }
        
        // Time control in 3D mode
        if (currentMode == AppMode::MODE_3D) {
            if (key == GLFW_KEY_T) {
                renderer3D->setTimeSpeed(10.0f);
                std::cout << "[TIME] Fast forward (10x)" << std::endl;
            }
            if (key == GLFW_KEY_Y) {
                renderer3D->setTimeSpeed(1.0f);
                std::cout << "[TIME] Normal speed (1x)" << std::endl;
            }
        }
    }
    
    // Camera movement keys for 3D mode
    if (key == GLFW_KEY_W) keys[0] = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_S) keys[1] = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_A) keys[2] = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_D) keys[3] = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_SPACE) keys[4] = (action != GLFW_RELEASE);
    if (key == GLFW_KEY_LEFT_SHIFT) keys[5] = (action != GLFW_RELEASE);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    
    // Mouse click for building selection in 2D mode
    if (currentMode == AppMode::MODE_2D && 
        glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        
        static bool clickProcessed = false;
        if (!clickProcessed) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            glm::vec2 worldPos = screenToWorld(glm::vec2(xpos, ypos), width, height);
            selectNearestBuilding(worldPos);
            clickProcessed = true;
        }
    } else {
        static bool clickProcessed = false;
        clickProcessed = false;
    }
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * Convert screen coordinates to world coordinates for 2D view
 */
glm::vec2 screenToWorld(const glm::vec2& screenPos, int screenWidth, int screenHeight) {
    float worldX = (screenPos.x / screenWidth) * userLayoutSize;
    float worldY = ((screenHeight - screenPos.y) / screenHeight) * userLayoutSize;
    return glm::vec2(worldX, worldY);
}

/**
 * Select the nearest building to the clicked position
 * Used for interactive building movement with arrow keys
 */
void selectNearestBuilding(const glm::vec2& worldPos) {
    const auto& buildings = cityGen.getBuildings();
    selectedBuildingIndex = -1;
    
    for (size_t i = 0; i < buildings.size(); ++i) {
        const auto& building = buildings[i];
        
        // Check if click is inside building bounds
        if (worldPos.x >= building.position.x && 
            worldPos.x <= building.position.x + building.size.x &&
            worldPos.y >= building.position.y && 
            worldPos.y <= building.position.y + building.size.y) {
            
            selectedBuildingIndex = i;
            std::cout << "[SELECT] Building #" << i << " selected at (" 
                      << building.position.x << ", " << building.position.y << ")" << std::endl;
            std::cout << "         Use arrow keys to move it!" << std::endl;
            return;
        }
    }
    
    // If no building was clicked, deselect
    if (selectedBuildingIndex == -1) {
        std::cout << "[SELECT] No building at cursor position" << std::endl;
    }
}

/**
 * Move the currently selected building by dx, dy pixels
 * Demonstrates interactive object manipulation
 */
void moveSelectedBuilding(int dx, int dy) {
    if (selectedBuildingIndex < 0) return;
    
    auto& buildings = cityGen.getBuildings();
    if (selectedBuildingIndex >= buildings.size()) return;
    
    auto& building = buildings[selectedBuildingIndex];
    
    // Calculate new position
    float newX = building.position.x + dx;
    float newY = building.position.y + dy;
    
    // Keep building within bounds
    if (newX >= 0 && newX + building.size.x <= userLayoutSize &&
        newY >= 0 && newY + building.size.y <= userLayoutSize) {
        
        building.position.x = newX;
        building.position.y = newY;
        
        std::cout << "[MOVE] Building moved to (" << newX << ", " << newY << ")" << std::endl;
    } else {
        std::cout << "[MOVE] Cannot move building outside city bounds" << std::endl;
    }
}
