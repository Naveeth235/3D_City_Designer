# QUICK START GUIDE

## For Windows Users

### Option 1: Automated Setup (Recommended)

1. **Run setup script** (downloads dependencies):
   ```powershell
   .\setup.ps1
   ```

2. **Install GLFW and GLM via vcpkg**:
   ```powershell
   # If you don't have vcpkg, install it first:
   git clone https://github.com/Microsoft/vcpkg.git
   cd vcpkg
   .\bootstrap-vcpkg.bat
   .\vcpkg integrate install
   
   # Install dependencies
   .\vcpkg install glfw3:x64-windows glm:x64-windows
   ```

3. **Download GLAD**:
   - Visit: https://glad.dav1d.de/
   - Settings: OpenGL 3.3+, Core Profile, C/C++
   - Generate and download
   - Extract `include/` to `libs/glad/include/`
   - Extract `src/glad.c` to `libs/glad/src/`

4. **Build the project**:
   ```powershell
   .\build.ps1 -VcpkgPath "C:\path\to\vcpkg"
   ```

5. **Run**:
   ```powershell
   cd build\bin\Release
   .\Interactive3DCityDesigner.exe
   ```

### Option 2: Manual Setup

1. Install Visual Studio 2019+ with C++ Desktop Development
2. Install CMake (https://cmake.org/download/)
3. Install Python 3 (https://www.python.org/downloads/)
4. Follow steps 2-5 from Option 1

---

## For Linux Users

### Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake libglfw3-dev libglm-dev python3 python3-pip

# Install Python packages
pip3 install Pillow numpy

# Download stb_image.h
curl -o include/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

# Download GLAD
# Visit https://glad.dav1d.de/ and follow instructions in main README

# Generate textures
python3 generate_textures.py

# Build
mkdir build && cd build
cmake ..
cmake --build . -j$(nproc)

# Run
./bin/Interactive3DCityDesigner
```

### Arch Linux

```bash
sudo pacman -S base-devel cmake glfw-x11 glm python python-pip
pip install Pillow numpy
# Continue with steps above
```

---

## For macOS Users

```bash
# Install Homebrew (if not installed)
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake glfw glm python3

# Install Python packages
pip3 install Pillow numpy

# Download stb_image.h
curl -o include/stb_image.h https://raw.githubusercontent.com/nothings/stb/master/stb_image.h

# Download GLAD (see main README)

# Generate textures
python3 generate_textures.py

# Build
mkdir build && cd build
cmake ..
cmake --build . -j$(sysctl -n hw.ncpu)

# Run
./bin/Interactive3DCityDesigner
```

---

## Verification Checklist

Before building, ensure you have:

- [ ] CMake 3.15+
- [ ] C++17 compatible compiler
- [ ] GLFW 3.x installed
- [ ] GLM installed
- [ ] GLAD files in `libs/glad/`
- [ ] `stb_image.h` in `include/`
- [ ] Textures generated in `assets/` (run `generate_textures.py`)
- [ ] All shader files in `shaders/`

---

## Troubleshooting

### "Cannot find GLFW"
- **Windows**: Make sure vcpkg is integrated and you specified the toolchain file
- **Linux**: `sudo apt-get install libglfw3-dev`
- **macOS**: `brew install glfw`

### "Cannot find GLM"
- **Windows**: `vcpkg install glm:x64-windows`
- **Linux**: `sudo apt-get install libglm-dev`
- **macOS**: `brew install glm`

### "Shader files not found"
- Shaders are automatically copied to the build directory
- Check `build/bin/shaders/` exists
- If missing, copy `shaders/` to `build/bin/`

### "Textures not loading"
- Run `python generate_textures.py` first
- Check `assets/` contains .jpg and .png files
- Check `build/bin/assets/` exists

### Black screen / Nothing rendering
- Check OpenGL version: must be 3.3+
- Update graphics drivers
- Check console for error messages

---

## First Time Running

When you first run the program:

1. You'll start in **2D mode** with a grid layout
2. Press **H** to see all controls
3. Try different road types: **1** (Grid), **2** (Radial), **3** (Random)
4. Change skyline: **4** (Low), **5** (Mid), **6** (Skyscrapers)
5. Press **ENTER** to switch to 3D mode
6. Use **W/A/S/D** to move, **Right Mouse** to look around
7. Press **ENTER** again to return to 2D mode

Enjoy designing your city! 🏙️
