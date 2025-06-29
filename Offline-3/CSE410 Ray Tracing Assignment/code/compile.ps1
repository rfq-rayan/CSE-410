# Compilation script for ray tracer
Write-Host "Ray Tracer Compilation Script" -ForegroundColor Green
Write-Host "============================" -ForegroundColor Green

# Check if g++ is available
try {
    $gccVersion = g++ --version 2>&1
    Write-Host "Found g++: OK" -ForegroundColor Green
} catch {
    Write-Host "Error: g++ not found. Please install MinGW or MSYS2" -ForegroundColor Red
    exit 1
}

Write-Host "`nCompiling main raytracer (with OpenGL)..."
$compileMain = "g++ -o raytracer.exe 2005062_main.cpp intersection_implementations.cpp stb_image_impl.cpp -lfreeglut -lopengl32 -lglu32"
Write-Host $compileMain
try {
    Invoke-Expression $compileMain
    if (Test-Path "raytracer.exe") {
        Write-Host "SUCCESS: raytracer.exe compiled" -ForegroundColor Green
    } else {
        Write-Host "ERROR: raytracer.exe not created" -ForegroundColor Red
    }
} catch {
    Write-Host "ERROR: Failed to compile main raytracer" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
}

Write-Host "`nCompiling headless raytracer (for testing)..."
$compileHeadless = "g++ -o raytracer_headless.exe raytracer_headless.cpp intersection_implementations.cpp stb_image_impl.cpp -lfreeglut -lopengl32 -lglu32"
Write-Host $compileHeadless
try {
    Invoke-Expression $compileHeadless
    if (Test-Path "raytracer_headless.exe") {
        Write-Host "SUCCESS: raytracer_headless.exe compiled" -ForegroundColor Green
    } else {
        Write-Host "ERROR: raytracer_headless.exe not created" -ForegroundColor Red
    }
} catch {
    Write-Host "ERROR: Failed to compile headless raytracer" -ForegroundColor Red
    Write-Host $_.Exception.Message -ForegroundColor Red
}

Write-Host "`nCompilation complete!" -ForegroundColor Cyan
Write-Host "- Use raytracer.exe for interactive mode (OpenGL preview)"
Write-Host "- Use raytracer_headless.exe for batch processing"
Write-Host "- Run .\code\test_scenes.ps1 to test all scenes"
