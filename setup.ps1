# Setup script for Interactive 3D City Designer
# This script helps set up the project on Windows

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Interactive 3D City Designer Setup" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Check if Python is installed
Write-Host "Checking Python installation..." -ForegroundColor Yellow
try {
    $pythonVersion = python --version 2>&1
    Write-Host "✓ Python found: $pythonVersion" -ForegroundColor Green
} catch {
    Write-Host "✗ Python not found. Please install Python 3." -ForegroundColor Red
    exit 1
}

# Check if CMake is installed
Write-Host "Checking CMake installation..." -ForegroundColor Yellow
try {
    $cmakeVersion = cmake --version 2>&1 | Select-Object -First 1
    Write-Host "✓ CMake found: $cmakeVersion" -ForegroundColor Green
} catch {
    Write-Host "✗ CMake not found. Please install CMake 3.15+." -ForegroundColor Red
    exit 1
}

# Create directories
Write-Host "`nCreating directories..." -ForegroundColor Yellow
$dirs = @("libs/glad/include", "libs/glad/src", "include", "assets")
foreach ($dir in $dirs) {
    if (-not (Test-Path $dir)) {
        New-Item -ItemType Directory -Path $dir -Force | Out-Null
        Write-Host "✓ Created $dir" -ForegroundColor Green
    } else {
        Write-Host "✓ Directory exists: $dir" -ForegroundColor Green
    }
}

# Download stb_image.h
Write-Host "`nDownloading stb_image.h..." -ForegroundColor Yellow
$stbPath = "include/stb_image.h"
if (-not (Test-Path $stbPath)) {
    try {
        Invoke-WebRequest -Uri "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h" -OutFile $stbPath
        Write-Host "✓ Downloaded stb_image.h" -ForegroundColor Green
    } catch {
        Write-Host "✗ Failed to download stb_image.h" -ForegroundColor Red
        Write-Host "  Please download manually from: https://github.com/nothings/stb" -ForegroundColor Yellow
    }
} else {
    Write-Host "✓ stb_image.h already exists" -ForegroundColor Green
}

# Install Python dependencies
Write-Host "`nInstalling Python dependencies..." -ForegroundColor Yellow
try {
    python -m pip install --quiet Pillow numpy
    Write-Host "✓ Installed Pillow and numpy" -ForegroundColor Green
} catch {
    Write-Host "✗ Failed to install Python packages" -ForegroundColor Red
}

# Generate textures
Write-Host "`nGenerating textures..." -ForegroundColor Yellow
if (Test-Path "generate_textures.py") {
    try {
        python generate_textures.py
        Write-Host "✓ Textures generated successfully" -ForegroundColor Green
    } catch {
        Write-Host "✗ Failed to generate textures" -ForegroundColor Red
    }
} else {
    Write-Host "✗ generate_textures.py not found" -ForegroundColor Red
}

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "  Setup Complete!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Next steps:" -ForegroundColor Yellow
Write-Host "1. Download GLAD from https://glad.dav1d.de/" -ForegroundColor White
Write-Host "   - OpenGL 3.3+, Core Profile" -ForegroundColor White
Write-Host "   - Extract to libs/glad/" -ForegroundColor White
Write-Host ""
Write-Host "2. Install GLFW and GLM:" -ForegroundColor White
Write-Host "   vcpkg install glfw3:x64-windows glm:x64-windows" -ForegroundColor White
Write-Host ""
Write-Host "3. Build the project:" -ForegroundColor White
Write-Host "   mkdir build" -ForegroundColor White
Write-Host "   cd build" -ForegroundColor White
Write-Host "   cmake .. -DCMAKE_TOOLCHAIN_FILE=`"C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake`"" -ForegroundColor White
Write-Host "   cmake --build . --config Release" -ForegroundColor White
Write-Host ""
