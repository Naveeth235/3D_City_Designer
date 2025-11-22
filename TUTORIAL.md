# TUTORIAL: Understanding the Interactive 3D City Designer

## Architecture Overview

This project follows a modular design pattern with clear separation of concerns:

```
┌─────────────────────────────────────────────────────────────┐
│                         main.cpp                             │
│                    (Application Loop)                        │
└────────────┬──────────────────┬─────────────────────────────┘
             │                  │
    ┌────────▼────────┐  ┌─────▼──────────┐
    │  Renderer2D     │  │  Renderer3D     │
    │  (2D Planning)  │  │  (3D Explore)   │
    └────────┬────────┘  └─────┬──────────┘
             │                  │
             └────────┬─────────┘
                      │
             ┌────────▼────────┐
             │  CityGenerator  │
             │  (Data Model)   │
             └─────────────────┘
```

## Core Components

### 1. CityGenerator (citygenerator.h/.cpp)

**Purpose**: Generates the logical city data structure

**Key Methods**:
- `generateCity()` - Main generation function
- `generateRoads()` - Creates road networks
- `generateBuildings()` - Places buildings with collision detection
- `generateParks()` - Creates circular parks

**Data Structures**:
```cpp
struct Building {
    glm::vec2 position;  // 2D position
    glm::vec2 size;      // Width and depth
    float height;        // Building height
    int textureIndex;    // Which texture to use
};

struct Road {
    Point2D start, end;  // Line segment
};

struct Park {
    Point2D center;      // Circle center
    int radius;          // Circle radius
};
```

**Road Generation Algorithms**:

1. **Grid Layout**:
   ```
   Creates evenly spaced perpendicular roads
   Visual: # (hashtag pattern)
   ```

2. **Radial Layout**:
   ```
   Spokes from center + concentric circles
   Visual: * (star pattern)
   ```

3. **Random Layout**:
   ```
   Randomly placed connecting roads
   Visual: ~ (organic pattern)
   ```

### 2. Renderer2D (renderer2d.h/.cpp)

**Purpose**: Draws the 2D city planning view

**Core Algorithms**:

#### Bresenham's Line Algorithm
```cpp
void drawBresenhamLine(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    
    while (true) {
        pixels.push_back(point(x, y));
        if (x == x2 && y == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 < dx) { err += dx; y += sy; }
    }
}
```

**Why Bresenham?**
- Efficient: uses only integer arithmetic
- Accurate: perfect pixel-level lines
- Classic: fundamental computer graphics algorithm

#### Midpoint Circle Algorithm
```cpp
void drawMidpointCircle(int cx, int cy, int radius) {
    int x = 0, y = radius;
    int d = 1 - radius;
    
    while (x < y) {
        plot8Points(cx, cy, x, y);  // Use symmetry
        x++;
        if (d < 0) {
            d += 2 * x + 1;
        } else {
            y--;
            d += 2 * (x - y) + 1;
        }
    }
}
```

**Why Midpoint Circle?**
- Symmetry: draws 8 points per iteration
- Efficient: integer arithmetic only
- Smooth: perfect circular arcs

### 3. Renderer3D (renderer3d.h/.cpp)

**Purpose**: Renders the 3D city exploration view

**Key Components**:

#### Camera System
```cpp
struct Camera {
    glm::vec3 position;  // Where camera is
    glm::vec3 front;     // Where it looks
    glm::vec3 up;        // Up direction
    float yaw, pitch;    // Rotation angles
};
```

**Camera Controls**:
- WASD: Move in plane
- Space/Shift: Move up/down
- Mouse: Rotate view (FPS-style)

#### Mesh Generation

**Cube (Buildings)**:
- 8 vertices (corners)
- 6 faces (sides)
- Texture coordinates for each face
- Normal vectors for lighting

**Cylinder (Parks/Fountains)**:
- Generated with trigonometry
- Segments determine smoothness
- Caps on top and bottom

#### Lighting Model (Phong)
```glsl
// Fragment shader
vec3 ambient = ambientStrength * lightColor;
vec3 diffuse = diff * lightColor;
vec3 specular = spec * lightColor;
vec3 result = (ambient + diffuse + specular) * texture;
```

### 4. Shader System (shader.h/.cpp)

**Purpose**: Manages GLSL shader programs

**Shaders in Project**:

1. **basic_vert.glsl / basic_frag.glsl** (2D)
   - Orthographic projection
   - Solid colors
   - Simple pixel rendering

2. **tex_vert.glsl / tex_frag.glsl** (3D)
   - Perspective projection
   - Texture sampling
   - Phong lighting calculations

### 5. Texture System (texture.h/.cpp)

**Purpose**: Loads and manages textures

**Uses stb_image.h**:
- Single-header library
- Supports JPG, PNG, BMP, etc.
- Simple API

## Data Flow

### 2D Mode Flow
```
User Input → CityGenerator → Generate Data
                ↓
         Renderer2D reads data
                ↓
    Apply Bresenham/Midpoint Circle
                ↓
         Upload to GPU → Render
```

### 3D Mode Flow
```
CityGenerator Data → Renderer3D
                ↓
    Create 3D Meshes (cubes, cylinders)
                ↓
         Apply Textures
                ↓
    Transform (Model-View-Projection)
                ↓
         Lighting Calculation → Render
```

### Mode Switching
```
Press ENTER
    ↓
Toggle: MODE_2D ↔ MODE_3D
    ↓
Switch projection matrix
Switch render function
```

## OpenGL Pipeline

### 2D Rendering Pipeline
```
1. Set orthographic projection
2. Clear screen
3. For each line/circle:
   - Calculate pixels (CPU)
   - Upload to VBO
   - Draw points (GL_POINTS)
```

### 3D Rendering Pipeline
```
1. Set perspective projection
2. Clear depth buffer
3. For each object:
   - Set model matrix (position/scale/rotation)
   - Bind texture
   - Set lighting uniforms
   - Draw mesh (GL_TRIANGLES)
```

## Mathematical Concepts

### 2D Transformations
```
Orthographic Projection:
x_ndc = (x - left) / (right - left) * 2 - 1
y_ndc = (y - bottom) / (top - bottom) * 2 - 1

Used for: 2D grid-based planning view
```

### 3D Transformations
```
Perspective Projection:
[x_clip]   [Projection]   [View]   [Model]   [x]
[y_clip] = [   Matrix  ] × [Matrix] × [Matrix] × [y]
[z_clip]                                        [z]
[w_clip]                                        [1]

Where:
- Model: Object → World space
- View: World → Camera space
- Projection: Camera → Screen space
```

### Lighting Calculations
```
Ambient:  La = ka × lightColor
Diffuse:  Ld = kd × (N · L) × lightColor
Specular: Ls = ks × (R · V)^n × lightColor

Final: L = La + Ld + Ls
```

## Performance Considerations

### Optimization Techniques Used:

1. **Static Geometry**
   - Buildings/roads generated once
   - Stored in VBOs (GPU memory)
   - Only camera transforms each frame

2. **Culling**
   - Backface culling enabled
   - Depth testing prevents overdraw

3. **Batch Drawing**
   - Group similar objects
   - Minimize texture switches

4. **Efficient Algorithms**
   - Bresenham: No floating point
   - Midpoint Circle: Symmetry exploitation

## Extension Ideas

Want to enhance the project? Try:

1. **Advanced Features**:
   - Save/load city layouts
   - Day/night cycle
   - Weather effects (rain, fog)
   - Animated fountains
   - Traffic simulation

2. **Graphics Enhancements**:
   - Shadow mapping
   - Skybox
   - Post-processing (bloom, DOF)
   - Normal mapping
   - Ambient occlusion

3. **Gameplay Elements**:
   - Resource management
   - Building placement by mouse
   - Population simulation
   - Economic system

4. **Algorithm Variations**:
   - DDA line algorithm
   - Flood fill for districts
   - Voronoi diagrams for zones
   - Perlin noise for terrain

## Common Issues & Solutions

### Issue: Black textures
**Solution**: 
- Check texture files exist
- Verify file paths in code
- Run generate_textures.py

### Issue: No 3D rendering
**Solution**:
- Enable depth testing: `glEnable(GL_DEPTH_TEST)`
- Clear depth buffer: `glClear(GL_DEPTH_BUFFER_BIT)`
- Check camera position

### Issue: Shader compilation errors
**Solution**:
- Verify GLSL version (330 core)
- Check uniform names match
- Enable error checking in shader.cpp

### Issue: Performance problems
**Solution**:
- Reduce building count
- Lower polygon count on cylinders
- Use simpler lighting model

## Learning Resources

### Computer Graphics:
- "Computer Graphics: Principles and Practice" by Foley et al.
- LearnOpenGL.com - Excellent tutorials
- "Real-Time Rendering" by Akenine-Möller et al.

### Algorithms:
- Bresenham's algorithm: Classic rasterization
- Midpoint circle: Efficient circle drawing
- Phong shading: Realistic lighting

### OpenGL:
- docs.gl - Quick reference
- opengl.org - Official documentation
- GPU Gems series - Advanced techniques

## Conclusion

This project demonstrates:
- ✓ Classic 2D algorithms (Bresenham, Midpoint Circle)
- ✓ Modern 3D graphics (shaders, textures, lighting)
- ✓ Software architecture (modularity, separation)
- ✓ Real-time rendering (60 FPS target)
- ✓ User interaction (keyboard, mouse)

The combination of 2D planning and 3D exploration creates a unique interactive experience that showcases both traditional and modern graphics programming techniques.

**Happy coding! 🎨🏙️**
