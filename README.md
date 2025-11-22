# Interactive 3D City Designer

A complete C++ OpenGL project for designing cities in 2D and exploring them in 3D.

## Features

- **2D City Planning Mode**: Design your city layout using classic computer graphics algorithms
  - Bresenham's Line Algorithm for roads
  - Midpoint Circle Algorithm for parks and roundabouts
  - Grid-based layout system
  
- **3D Exploration Mode**: Navigate through your generated city
  - Textured buildings with varying heights
  - Roads, parks, and fountains
  - First-person camera controls
  - Real-time lighting

- **City Generation Options**:
  - Number of buildings (adjustable)
  - Road network types: Grid, Radial, or Random
  - Skyline types: Low-rise, Mid-rise, or Skyscrapers
  - Procedurally generated parks with fountains

## Requirements

### Dependencies
- **CMake** (3.15 or higher)
- **C++17** compatible compiler
- **OpenGL** 3.3 or higher
- **GLFW** 3.x
- **GLM** (OpenGL Mathematics)
- **GLAD** (OpenGL loader)
- **Python 3** with Pillow (for texture generation)

### Platform Support
- Windows (MSVC, MinGW)
- Linux (GCC, Clang)
- macOS (Clang)

## Building the Project

### Step 1: Install Dependencies

#### Windows (with vcpkg)
```powershell
vcpkg install glfw3:x64-windows glm:x64-windows
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get install libglfw3-dev libglm-dev python3 python3-pip
pip3 install Pillow numpy
```

#### macOS (with Homebrew)
```bash
brew install glfw glm python3
pip3 install Pillow numpy
```

### Step 2: Download GLAD

1. Go to https://glad.dav1d.de/
2. Set Language to C/C++, Specification to OpenGL, API gl to 3.3+, Profile to Core
3. Generate and download the zip file
4. Extract and place the contents:
   - `include/glad/` and `include/KHR/` → project `libs/glad/include/`
   - `src/glad.c` → project `libs/glad/src/`

### Step 3: Download stb_image.h

```powershell
# Windows PowerShell
Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" -OutFile "include/stb_image.h"
```

```bash
# Linux/macOS
curl -o include/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h
```

### Step 4: Generate Textures

```bash
python generate_textures.py
```

This creates procedural textures in the `assets/` folder.

### Step 5: Build with CMake

```bash
# Create build directory
mkdir build
cd build

# Configure (Windows with vcpkg)
cmake .. -DCMAKE_TOOLCHAIN_FILE="C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake"

# Configure (Linux/macOS)
cmake ..

# Build
cmake --build . --config Release

# Run
./bin/Interactive3DCityDesigner  # Linux/macOS
.\bin\Release\Interactive3DCityDesigner.exe  # Windows
```

## Controls

### General Controls
- **ENTER** - Switch between 2D and 3D mode
- **R** - Regenerate city with current parameters
- **H** - Show help/instructions
- **ESC** - Exit application

### City Parameters (2D Mode)
- **1/2/3** - Select road type (Grid/Radial/Random)
- **4/5/6** - Select skyline type (Low-rise/Mid-rise/Skyscraper)
- **+/-** - Increase/Decrease number of buildings

### 3D Navigation
- **W/A/S/D** - Move camera forward/left/backward/right
- **SPACE** - Move camera up
- **LEFT SHIFT** - Move camera down
- **Right Mouse + Move** - Look around

## Project Structure

```
City Designer/
├── CMakeLists.txt              # Build configuration
├── README.md                   # This file
├── generate_textures.py        # Texture generation script
├── src/
│   ├── main.cpp               # Application entry point
│   ├── citygenerator.h/cpp    # City generation logic
│   ├── renderer2d.h/cpp       # 2D rendering with algorithms
│   ├── renderer3d.h/cpp       # 3D rendering with textures
│   ├── shader.h/cpp           # Shader management
│   └── texture.h/cpp          # Texture loading
├── shaders/
│   ├── basic_vert.glsl        # 2D vertex shader
│   ├── basic_frag.glsl        # 2D fragment shader
│   ├── tex_vert.glsl          # 3D vertex shader
│   └── tex_frag.glsl          # 3D fragment shader
├── assets/
│   ├── building1.obj          # Building model 1
│   ├── building2.obj          # Building model 2
│   ├── fountain.obj           # Fountain model
│   ├── building1.jpg          # Building texture 1
│   ├── building2.jpg          # Building texture 2
│   ├── road.jpg               # Road texture
│   ├── grass.jpg              # Grass texture
│   └── fountain.png           # Fountain texture
├── include/
│   └── stb_image.h            # Image loading library
└── libs/
    ├── glad/                  # OpenGL loader
    │   ├── include/
    │   └── src/
    └── glm/                   # Math library (if not system-wide)
```

## Algorithms Implemented

### Bresenham's Line Algorithm
Used for drawing roads and building outlines in 2D mode. Provides efficient rasterization of straight lines.

### Midpoint Circle Algorithm
Used for drawing circular parks and roundabouts. Efficiently draws circles using integer arithmetic.

### 3D Rendering
- Perspective projection with camera transformations
- Phong lighting model with ambient, diffuse, and specular components
- Texture mapping on all 3D models
- Procedural mesh generation for buildings, roads, and parks

## Customization

You can modify city generation parameters in `src/main.cpp`:

```cpp
int numBuildings = 30;        // Number of buildings
int layoutSize = 600;         // City size
RoadType roadType = RoadType::GRID;
SkylineType skylineType = SkylineType::MID_RISE;
```

## Troubleshooting

### Textures not loading
- Ensure you ran `python generate_textures.py`
- Check that `assets/` folder is copied to the build directory
- The program will use default colors if textures fail to load

### Shaders not compiling
- Check that `shaders/` folder is copied to the build directory
- Ensure OpenGL 3.3+ support on your graphics card
- Check console output for shader compilation errors

### GLFW/GLAD not found
- On Windows, ensure vcpkg integration is configured
- On Linux, install development packages
- Check CMake output for missing dependencies

## License

This is an educational project. Feel free to use and modify for learning purposes.

## Credits

- **GLFW** - Window and input handling
- **GLAD** - OpenGL loader
- **GLM** - Mathematics library
- **stb_image** - Image loading
- Algorithms based on classic computer graphics theory

---

**Developed for Graphics and Visualization Course (SE3032)**
