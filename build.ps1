# Build script for Interactive 3D City Designer

param(
    [switch]$Clean,
    [string]$Config = "Release",
    [string]$VcpkgPath = ""
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  Building City Designer" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Clean build
if ($Clean -and (Test-Path "build")) {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force "build"
    Write-Host "✓ Cleaned" -ForegroundColor Green
}

# Create build directory
if (-not (Test-Path "build")) {
    New-Item -ItemType Directory -Path "build" | Out-Null
}

Set-Location "build"

# Configure CMake
Write-Host "`nConfiguring CMake..." -ForegroundColor Yellow
if ($VcpkgPath -ne "") {
    cmake .. -DCMAKE_TOOLCHAIN_FILE="$VcpkgPath/scripts/buildsystems/vcpkg.cmake"
} else {
    cmake ..
}

if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ CMake configuration failed" -ForegroundColor Red
    Set-Location ..
    exit 1
}
Write-Host "✓ Configuration complete" -ForegroundColor Green

# Build
Write-Host "`nBuilding project ($Config)..." -ForegroundColor Yellow
cmake --build . --config $Config

if ($LASTEXITCODE -ne 0) {
    Write-Host "✗ Build failed" -ForegroundColor Red
    Set-Location ..
    exit 1
}
Write-Host "✓ Build complete" -ForegroundColor Green

Set-Location ..

Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "  Build Successful!" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Run the program:" -ForegroundColor Yellow
Write-Host "  cd build/bin" -ForegroundColor White
Write-Host "  .\$Config\Interactive3DCityDesigner.exe" -ForegroundColor White
Write-Host ""
