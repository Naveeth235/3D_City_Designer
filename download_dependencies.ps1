# Script to download GLFW and GLM dependencies

$ErrorActionPreference = "Stop"

Write-Host "Downloading dependencies for 3D City Designer..." -ForegroundColor Green

# Create libs directories
$libsDir = "libs"
New-Item -ItemType Directory -Force -Path "$libsDir\glfw" | Out-Null
New-Item -ItemType Directory -Force -Path "$libsDir\glm" | Out-Null

# Download GLM (header-only library)
Write-Host "Downloading GLM..." -ForegroundColor Yellow
$glmUrl = "https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip"
$glmZip = "glm.zip"
Invoke-WebRequest -Uri $glmUrl -OutFile $glmZip
Expand-Archive -Path $glmZip -DestinationPath $libsDir -Force
Move-Item -Path "$libsDir\glm\glm" -Destination "$libsDir\glm_temp" -Force
Remove-Item -Path "$libsDir\glm" -Recurse -Force
Move-Item -Path "$libsDir\glm_temp" -Destination "$libsDir\glm" -Force
Remove-Item -Path $glmZip -Force
Write-Host "GLM installed successfully!" -ForegroundColor Green

# Download GLFW precompiled binaries for Windows
Write-Host "Downloading GLFW..." -ForegroundColor Yellow
$glfwUrl = "https://github.com/glfw/glfw/releases/download/3.3.8/glfw-3.3.8.bin.WIN64.zip"
$glfwZip = "glfw.zip"
Invoke-WebRequest -Uri $glfwUrl -OutFile $glfwZip
Expand-Archive -Path $glfwZip -DestinationPath $libsDir -Force

# Organize GLFW files
$glfwExtracted = Get-ChildItem -Path $libsDir -Filter "glfw-*" -Directory | Select-Object -First 1
if ($glfwExtracted) {
    Copy-Item -Path "$($glfwExtracted.FullName)\include\*" -Destination "$libsDir\glfw\include" -Recurse -Force
    New-Item -ItemType Directory -Force -Path "$libsDir\glfw\lib" | Out-Null
    Copy-Item -Path "$($glfwExtracted.FullName)\lib-vc2022\glfw3.lib" -Destination "$libsDir\glfw\lib\" -Force
    Remove-Item -Path $glfwExtracted.FullName -Recurse -Force
}
Remove-Item -Path $glfwZip -Force
Write-Host "GLFW installed successfully!" -ForegroundColor Green

Write-Host "`nAll dependencies installed successfully!" -ForegroundColor Green
Write-Host "You can now run CMake to configure the project." -ForegroundColor Cyan
