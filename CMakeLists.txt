cmake_minimum_required(VERSION 3.15)
project(Interactive3DCityDesigner VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Source files
set(SOURCES
    src/main.cpp
    src/citygenerator.cpp
    src/renderer2d.cpp
    src/renderer3d.cpp
    src/shader.cpp
    src/texture.cpp
    src/textrenderer.cpp
    libs/glad/src/glad.c
)

# Header files
set(HEADERS
    src/citygenerator.h
    src/renderer2d.h
    src/renderer3d.h
    src/shader.h
    src/texture.h
    src/textrenderer.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/libs/glad/include
    ${CMAKE_SOURCE_DIR}/libs
)

# Find OpenGL
find_package(OpenGL REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)

# Platform-specific configurations
if(WIN32)
    # Windows - GLFW
    find_package(glfw3 CONFIG)
    if(glfw3_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    else()
        # Fallback to manual GLFW path
        set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/libs/glfw/include" CACHE PATH "GLFW include directory")
        set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/libs/glfw/lib/glfw3.lib" CACHE FILEPATH "GLFW library")
        target_include_directories(${PROJECT_NAME} PRIVATE ${GLFW_INCLUDE_DIR})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${GLFW_LIBRARY})
    endif()
elseif(APPLE)
    # macOS
    find_package(glfw3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa" "-framework IOKit" "-framework CoreVideo")
else()
    # Linux
    find_package(glfw3 REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE glfw ${CMAKE_DL_LIBS})
endif()

# Copy shaders and assets to build directory
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/shaders
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets
    ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets
)

# Installation
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
install(DIRECTORY shaders DESTINATION bin)
install(DIRECTORY assets DESTINATION bin)
