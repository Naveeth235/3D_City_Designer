#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <iostream>
#include <string>

#include "citygenerator.h"
#include "renderer2d.h"
#include "renderer3d.h"
#include "textrenderer.h"

// Window dimensions
const unsigned int SCREEN_WIDTH = 800;
const unsigned int SCREEN_HEIGHT = 600;

// Application state
enum class AppMode {
    MODE_2D,
    MODE_3D
};

AppMode currentMode = AppMode::MODE_2D;

// City parameters
int numBuildings = 30;
int layoutSize = 600;
RoadType roadType = RoadType::GRID;
SkylineType skylineType = SkylineType::MID_RISE;
int numParks = 3;

// Mouse state
double lastX = SCREEN_WIDTH / 2.0;
double lastY = SCREEN_HEIGHT / 2.0;
bool firstMouse = true;
bool mousePressed = false;
glm::vec2 mousePos;

// Camera movement
bool keys[6] = {false}; // W, S, A, D, Space, Shift
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Objects
CityGenerator cityGen;
Renderer2D* renderer2D = nullptr;
Renderer3D* renderer3D = nullptr;
TextRenderer* textRenderer = nullptr;

// UI state
bool showHelp = true;

// Function declarations
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void displayInstructions();

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    
    // Configure GLFW
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
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Load GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // Enable features
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Initialize renderers
    renderer2D = new Renderer2D();
    renderer2D->init(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    renderer3D = new Renderer3D();
    renderer3D->init(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    textRenderer = new TextRenderer();
    textRenderer->init(SCREEN_WIDTH, SCREEN_HEIGHT);
    
    // Generate initial city
    cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
    
    // Display instructions
    displayInstructions();
    
    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Process input
        processInput(window);
        
        // Update time-of-day and vehicles in 3D mode
        if (currentMode == AppMode::MODE_3D) {
            renderer3D->updateTimeOfDay(deltaTime);
            cityGen.updateVehicles(deltaTime);
        }
        
        // Render
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if (currentMode == AppMode::MODE_2D) {
            // 2D mode - city planning
            glDisable(GL_DEPTH_TEST);
            
            // Convert city data to 2D drawable elements
            renderer2D->clearElements();
            
            // Add roads
            for (const auto& road : cityGen.getRoads()) {
                Line2D line(road.start, road.end, glm::vec3(0.3f, 0.3f, 0.3f));
                renderer2D->addLine(line);
            }
            
            // Add parks
            for (const auto& park : cityGen.getParks()) {
                Circle2D circle(park.center, park.radius, glm::vec3(0.0f, 0.8f, 0.2f));
                renderer2D->addCircle(circle);
            }
            
            // Add building outlines (with glow at night)
            bool isNight = renderer3D->getTimeOfDay() < 6.0f || renderer3D->getTimeOfDay() >= 19.0f;
            for (const auto& building : cityGen.getBuildings()) {
                glm::vec3 buildingColor = isNight ? glm::vec3(1.0f, 0.9f, 0.5f) : glm::vec3(0.7f, 0.7f, 0.9f);
                
                int x1 = building.position.x;
                int y1 = building.position.y;
                int x2 = building.position.x + building.size.x;
                int y2 = building.position.y + building.size.y;
                
                renderer2D->addLine(Line2D(Point2D(x1, y1), Point2D(x2, y1), buildingColor));
                renderer2D->addLine(Line2D(Point2D(x2, y1), Point2D(x2, y2), buildingColor));
                renderer2D->addLine(Line2D(Point2D(x2, y2), Point2D(x1, y2), buildingColor));
                renderer2D->addLine(Line2D(Point2D(x1, y2), Point2D(x1, y1), buildingColor));
            }
            
            renderer2D->render();
            
            glEnable(GL_DEPTH_TEST);
        } else {
            // 3D mode - city exploration
            renderer3D->updateCamera(deltaTime, keys, 0.0f, 0.0f);
            renderer3D->render(cityGen);
        }
        
        // Render on-screen help UI
        if (showHelp) {
            glDisable(GL_DEPTH_TEST);
            
            float y = 10.0f;
            float scale = 1.8f;
            glm::vec3 titleColor(1.0f, 1.0f, 0.3f);
            glm::vec3 textColor(0.9f, 0.9f, 0.9f);
            glm::vec3 highlightColor(0.3f, 1.0f, 0.5f);
            
            textRenderer->renderText("INTERACTIVE 3D CITY DESIGNER", 10, y, scale, titleColor);
            y += 12 * scale;
            
            textRenderer->renderText("CONTROLS:", 10, y, scale, highlightColor);
            y += 10 * scale;
            textRenderer->renderText("ENTER - Switch 2D/3D", 10, y, scale, textColor);
            y += 10 * scale;
            textRenderer->renderText("R - Regenerate city", 10, y, scale, textColor);
            y += 10 * scale;
            textRenderer->renderText("T/Y - Time speed", 10, y, scale, textColor);
            y += 10 * scale;
            textRenderer->renderText("H - Toggle this help", 10, y, scale, textColor);
            y += 10 * scale;
            textRenderer->renderText("ESC - Exit", 10, y, scale, textColor);
            y += 14 * scale;
            
            textRenderer->renderText("CITY:", 10, y, scale, highlightColor);
            y += 10 * scale;
            textRenderer->renderText("1/2/3 - Road type", 10, y, scale, textColor);
            y += 10 * scale;
            textRenderer->renderText("4/5/6 - Skyline", 10, y, scale, textColor);
            y += 10 * scale;
            textRenderer->renderText("+/- - Buildings", 10, y, scale, textColor);
            y += 14 * scale;
            
            textRenderer->renderText("3D NAV:", 10, y, scale, highlightColor);
            y += 10 * scale;
            textRenderer->renderText("W/A/S/D - Move", 10, y, scale, textColor);
            y += 10 * scale;
            textRenderer->renderText("SPACE/SHIFT - Up/Down", 10, y, scale, textColor);
            y += 10 * scale;
            textRenderer->renderText("Right Mouse - Look", 10, y, scale, textColor);
            
            glEnable(GL_DEPTH_TEST);
        }
        
        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    // Cleanup
    delete renderer2D;
    delete renderer3D;
    delete textRenderer;
    
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (renderer3D) {
        renderer3D->setProjection(width, height);
    }
    if (textRenderer) {
        textRenderer->setProjection(width, height);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mousePressed = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        mousePos = glm::vec2(xpos, ypos);
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mousePressed = false;
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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        // Mode switching
        if (key == GLFW_KEY_ENTER) {
            if (currentMode == AppMode::MODE_2D) {
                currentMode = AppMode::MODE_3D;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                std::cout << "Switched to 3D mode" << std::endl;
            } else {
                currentMode = AppMode::MODE_2D;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                std::cout << "Switched to 2D mode" << std::endl;
            }
        }
        
        // City regeneration
        if (key == GLFW_KEY_R) {
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "City regenerated" << std::endl;
        }
        
        // Road type switching
        if (key == GLFW_KEY_1) {
            roadType = RoadType::GRID;
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "Road type: GRID" << std::endl;
        }
        if (key == GLFW_KEY_2) {
            roadType = RoadType::RADIAL;
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "Road type: RADIAL" << std::endl;
        }
        if (key == GLFW_KEY_3) {
            roadType = RoadType::RANDOM;
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "Road type: RANDOM" << std::endl;
        }
        
        // Skyline type switching
        if (key == GLFW_KEY_4) {
            skylineType = SkylineType::LOW_RISE;
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "Skyline: LOW RISE" << std::endl;
        }
        if (key == GLFW_KEY_5) {
            skylineType = SkylineType::MID_RISE;
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "Skyline: MID RISE" << std::endl;
        }
        if (key == GLFW_KEY_6) {
            skylineType = SkylineType::SKYSCRAPER;
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "Skyline: SKYSCRAPER" << std::endl;
        }
        
        // Building count adjustment
        if (key == GLFW_KEY_KP_ADD || key == GLFW_KEY_EQUAL) {
            numBuildings += 5;
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "Buildings: " << numBuildings << std::endl;
        }
        if (key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_MINUS) {
            numBuildings = std::max(5, numBuildings - 5);
            cityGen.generateCity(numBuildings, layoutSize, roadType, skylineType);
            std::cout << "Buildings: " << numBuildings << std::endl;
        }
        
        // Time control
        if (key == GLFW_KEY_T) {
            // Speed up time
            float currentSpeed = renderer3D->getTimeOfDay();
            renderer3D->setTimeSpeed(10.0f);
            std::cout << "Time speed: Fast (10x)" << std::endl;
        }
        if (key == GLFW_KEY_Y) {
            // Normal time
            renderer3D->setTimeSpeed(1.0f);
            std::cout << "Time speed: Normal (1x)" << std::endl;
        }
        if (key == GLFW_KEY_N) {
            // Toggle day/night for testing
            float currentTime = renderer3D->getTimeOfDay();
            if (currentTime >= 6.0f && currentTime < 19.0f) {
                // Set to night
                std::cout << "Time set to night (22:00)" << std::endl;
            } else {
                // Set to day
                std::cout << "Time set to day (12:00)" << std::endl;
            }
        }
        
        // Help
        if (key == GLFW_KEY_H) {
            showHelp = !showHelp;
            std::cout << "Help overlay: " << (showHelp ? "ON" : "OFF") << std::endl;
        }
    }
    
    // Camera movement keys
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
}

void displayInstructions() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "  INTERACTIVE 3D CITY DESIGNER" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\nCONTROLS:" << std::endl;
    std::cout << "  ENTER       - Switch between 2D/3D mode" << std::endl;
    std::cout << "  R           - Regenerate city" << std::endl;
    std::cout << "  T/Y         - Time speed (Fast/Normal)" << std::endl;
    std::cout << "  H           - Show this help" << std::endl;
    std::cout << "  ESC         - Exit application" << std::endl;
    std::cout << "\nCITY PARAMETERS:" << std::endl;
    std::cout << "  1/2/3       - Road type (Grid/Radial/Random)" << std::endl;
    std::cout << "  4/5/6       - Skyline (Low/Mid/Skyscraper)" << std::endl;
    std::cout << "  +/-         - Increase/Decrease buildings" << std::endl;
    std::cout << "\n3D NAVIGATION:" << std::endl;
    std::cout << "  W/A/S/D     - Move camera" << std::endl;
    std::cout << "  SPACE/SHIFT - Move up/down" << std::endl;
    std::cout << "  Right Mouse - Look around" << std::endl;
    std::cout << "\nALGORITHMS & FEATURES:" << std::endl;
    std::cout << "  - Bresenham's Line (roads)" << std::endl;
    std::cout << "  - Midpoint Circle (water pond)" << std::endl;
    std::cout << "  - 3D Texture Mapping (buildings)" << std::endl;
    std::cout << "  - Day/Night Cycle with dynamic lighting" << std::endl;
    std::cout << "  - Animated traffic vehicles" << std::endl;
    std::cout << "  - Street lights (glow at night)" << std::endl;
    std::cout << "  - Beautiful centered water pond" << std::endl;
    std::cout << "========================================\n" << std::endl;
}
