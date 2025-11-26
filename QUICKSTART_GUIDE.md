# Quick Start Guide - Interactive 3D City Designer

## 🚀 How to Run

### Step 1: Open Terminal
Open PowerShell and navigate to the Release folder:
```powershell
cd "C:\Users\DELL LATITUDE 5480\Documents\SLIIT\Y03S01\SE3032 - Graphics and Visualization\City Designer\3D_City_Designer\build\bin\Release"
```

### Step 2: Run the Program
```powershell
.\Interactive3DCityDesigner.exe
```

### Step 3: Configure Your City
The program will ask for the following inputs:

#### 1. **City Layout Size** (400-800 recommended)
```
Example: 600
What it does: Sets the size of your city grid (600x600)
```

#### 2. **Number of Buildings** (5-50)
```
Example: 20
What it does: How many buildings to generate
```

#### 3. **Road Network Pattern** (1-3)
```
Options:
  1 - Grid (perpendicular roads like Manhattan)
  2 - Radial (roads from center like Paris)
  3 - Random (organic pattern like old European cities)

Example: 1
```

#### 4. **Skyline Pattern** (1-3)
```
Options:
  1 - Low-rise (suburban, 20-50 units high)
  2 - Mid-rise (mixed urban, 50-100 units high)
  3 - Skyscraper (downtown, 100-200 units high)

Example: 2
```

#### 5. **Park/Fountain Size** (20-100)
```
Example: 50
What it does: Radius of the circular central park
Uses Midpoint Circle Algorithm!
```

#### 6. **Building Texture Theme** (0-2)
```
Options:
  0 - Modern glass/concrete
  1 - Classic brick
  2 - Mixed style

Example: 0
```

### Step 4: Explore Your City!

The application window will open showing your city in **2D Planning Mode**.

---

## 🎮 Controls

### 2D Planning Mode (Default Start)

**Select Building:**
- Click on any building with **Left Mouse Button**
- Selected building turns **yellow**

**Move Building:**
- Use **Arrow Keys** (↑↓←→) to move selected building
- Building stays within city bounds
- Console shows position updates

**Add NEW Building:**
- Press **N** to enter Add Building mode
- Building preview appears in **cyan**
- Use **Arrow Keys** (↑↓←→) to position
- Press **+/-** to adjust width
- Press **[/]** to adjust depth
- Press **Page Up/Down** to adjust height
- Press **ENTER** to place the building
- Press **ESC** to cancel

**Switch to 3D:**
- Press **ENTER** to explore in 3D

**Toggle Help:**
- Press **H** to show/hide on-screen controls

---

### 3D Exploration Mode

**Camera Movement:**
- **W** - Move forward
- **A** - Move left
- **S** - Move backward
- **D** - Move right
- **SPACE** - Move up
- **SHIFT** - Move down

**Look Around:**
- Hold **Right Mouse Button** and drag to look around
- (Cursor will be hidden in 3D mode)

**Time Control:**
- **T** - Fast forward time (10x speed) - watch day/night cycle
- **Y** - Normal time speed

**Switch to 2D:**
- Press **ENTER** to return to planning mode

---

## 💡 Tips for Best Experience

### Recommended Configurations

**For Assignment Demo:**
```
Layout Size: 600
Buildings: 20
Road Pattern: 1 (Grid)
Skyline: 2 (Mid-rise)
Park Radius: 50
Texture: 0 (Modern)
```

**For Dense City:**
```
Layout Size: 600
Buildings: 40
Road Pattern: 1 (Grid)
Skyline: 3 (Skyscraper)
Park Radius: 40
Texture: 0 (Modern)
```

**For Scenic Town:**
```
Layout Size: 800
Buildings: 15
Road Pattern: 3 (Random)
Skyline: 1 (Low-rise)
Park Radius: 60
Texture: 1 (Brick)
```

---

## 🎯 What to Show for Assignment

### 1. Algorithm Demonstrations

**Bresenham's Line Algorithm:**
- Visible in 2D mode - all roads
- Try different road patterns (Grid, Radial, Random)
- Zoom in to see pixel-perfect line rendering

**Midpoint Circle Algorithm:**
- Visible in 2D mode - central park
- Try different park sizes (20-100 radius)
- Notice smooth circle edges

**3D Texture Mapping:**
- Switch to 3D mode with ENTER
- Walk close to buildings to see textures
- Try different texture themes

**Basic OpenGL Lines:**
- 2D mode - building outlines
- Clear boundary visualization

### 2. Interactive Features

**Building Selection & Movement:**
1. In 2D mode, click a building
2. It turns yellow (selected)
3. Use arrow keys to move it
4. Show console output logging the movement
5. Try moving to edge - it stops at boundary

**Adding New Buildings:**
1. In 2D mode, press **N** to enter Add Building mode
2. A cyan preview building appears at the center
3. Use arrow keys to position it
4. Press **+/-** to adjust width (20-100)
5. Press **[/]** to adjust depth (20-100)
6. Press **Page Up/Down** to adjust height (20-200)
7. Press **ENTER** to place it in the city
8. The new building will appear in 3D mode with textures!
9. Press **ESC** to cancel if you change your mind

**Dual-View System:**
1. Start in 2D - show layout
2. Press ENTER to switch to 3D
3. Walk through the city you designed
4. Press ENTER to return to 2D
5. Make changes (move buildings, add new ones)
6. Switch back to 3D to see updates

### 3. User Configuration

**Show that ALL parameters are user-controlled:**
- Run program twice with different configurations
- Show how each parameter changes the city
- Demonstrate validation (try invalid inputs)

---

## 📊 What Each Feature Demonstrates

| Feature | Algorithm/Concept | Where to See It |
|---------|------------------|-----------------|
| Roads in 2D | Bresenham's Line | Gray lines connecting points |
| Park in 2D | Midpoint Circle | Green circle in center |
| Buildings in 3D | Texture Mapping | Walk close to buildings |
| Building outlines | Basic OpenGL Lines | Rectangles in 2D |
| Movement | Interactive Graphics | Arrow keys + selection |

---

## ⚠️ Troubleshooting

**Program doesn't start:**
- Make sure you're in the `build\bin\Release` folder
- Check that `assets` and `shaders` folders are present

**Can't see buildings in 3D:**
- They might be behind you
- Use WASD to move around
- Use right-mouse to look around

**Can't select building:**
- Make sure you're in 2D mode (not 3D)
- Click inside the building rectangle
- Look for yellow highlight

**Building won't move:**
- Make sure it's selected (yellow)
- Check if you're at the boundary
- Console will show "outside bounds" message

---

## 🎬 Demo Script for Presentation

1. **Start Program**
   ```
   "I'll now demonstrate the Interactive 3D City Designer"
   ```

2. **Configuration Phase**
   ```
   "First, the system asks for user configuration..."
   - Show entering each parameter
   - Explain what each does
   ```

3. **2D View**
   ```
   "Here's the generated city in 2D planning mode..."
   - Point out roads (Bresenham's Line)
   - Point out park (Midpoint Circle)
   - Show building outlines (OpenGL Lines)
   ```

4. **Building Movement**
   ```
   "Users can select and move buildings interactively..."
   - Click a building
   - Move with arrow keys
   - Show console output
   ```

5. **3D View**
   ```
   "Now let's explore in 3D..."
   - Press ENTER
   - Walk through city
   - Point out textured buildings
   - Show day/night cycle (press T)
   ```

6. **Return to 2D**
   ```
   "We can switch back anytime..."
   - Press ENTER
   - Show that movements persist
   ```

7. **Conclusion**
   ```
   "This demonstrates all required algorithms plus interactive features"
   ```

---

## 📝 Console Output Examples

**Successful Run:**
```
========================================
  INTERACTIVE 3D CITY DESIGNER
========================================

FEATURES DEMONSTRATED:
  ✓ Basic OpenGL Lines
  ✓ Bresenham's Line Algorithm
  ✓ Midpoint Circle Algorithm
  ✓ 3D Texture Mapping
  ✓ Interactive object placement
========================================

[Configuration inputs...]

[GENERATING CITY...]
[CITY GENERATED SUCCESSFULLY]

CITY IS READY! Opening 3D window...

[SELECT] Building #5 selected at (250, 300)
         Use arrow keys to move it!
[MOVE] Building moved to (255, 300)
[MODE] Switched to 3D exploration
[TIME] Fast forward (10x)
[MODE] Switched to 2D planning
```

---

## ✅ Assignment Checklist

Before submitting, verify:

- [ ] All user inputs work (layout, buildings, roads, skyline, park, texture)
- [ ] Roads visible in 2D (Bresenham's Line)
- [ ] Park visible in 2D (Midpoint Circle)
- [ ] Buildings visible in 3D with textures
- [ ] Building selection works (click in 2D)
- [ ] Building movement works (arrow keys)
- [ ] 2D/3D switching works (ENTER key)
- [ ] Console output is clear
- [ ] On-screen help displays correctly
- [ ] No crashes or errors

---

## 🎓 Key Points for Grading

**Technical Implementation (40%):**
- ✅ Bresenham's Line: See roads in 2D
- ✅ Midpoint Circle: See park in 2D
- ✅ 3D Textures: See buildings in 3D
- ✅ Stable: No crashes

**Creativity & Design (30%):**
- ✅ Novel: Full user control + interactive editing
- ✅ Integrated: All algorithms serve clear purposes
- ✅ Polished: Clear UI, smooth controls

**Presentation (20%):**
- ✅ Clear: Console + on-screen explanations
- ✅ Engaging: Interactive, dynamic
- ✅ Professional: Complete documentation

**Code Quality (10%):**
- ✅ Readable: Well-commented
- ✅ Organized: Clean structure
- ✅ Appropriate: Modern OpenGL

---

**You're all set! Good luck with your presentation! 🎉**
