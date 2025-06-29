# Ray Tracing Scene Test Script
# Tests all 10 scene files automatically

Write-Host "Ray Tracing Scene Test Suite" -ForegroundColor Green
Write-Host "============================" -ForegroundColor Green

# Check if raytracer exists
if (-not (Test-Path "raytracer_headless.exe")) {
    Write-Host "Error: raytracer_headless.exe not found. Please compile first:" -ForegroundColor Red
    Write-Host "g++ -o raytracer_headless.exe code\raytracer_headless.cpp intersection_implementations.cpp stb_image_impl.cpp"
    exit 1
}

# Create output directory for test results
$outputDir = "test_results"
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir
    Write-Host "Created output directory: $outputDir" -ForegroundColor Yellow
}

# Scene files to test
$sceneFiles = @(
    "test_scenes\scene_01_basic_spheres.txt",
    "test_scenes\scene_02_triangles.txt", 
    "test_scenes\scene_03_quadrics.txt",
    "test_scenes\scene_04_mixed_simple.txt",
    "test_scenes\scene_05_lighting_test.txt",
    "test_scenes\scene_06_reflection_test.txt",
    "test_scenes\scene_07_complex_scene.txt",
    "test_scenes\scene_08_shadows.txt",
    "test_scenes\scene_09_materials.txt",
    "test_scenes\scene_10_original.txt",
    "test_scenes\scene_11_colorful_spheres.txt",
    "test_scenes\scene_12_pyramid_with_spheres.txt",
    "test_scenes\scene_13_mixed_objects_colorful.txt",
    "test_scenes\scene_14_many_spheres.txt",
    "test_scenes\scene_15_triangle_complex.txt",
    "test_scenes\scene_16_complex_mixed.txt",
    "test_scenes\scene_17_reflective_spheres.txt",
    "test_scenes\scene_18_quadric_garden.txt"
)

$sceneNames = @(
    "Basic Spheres",
    "Triangle Objects",
    "Quadric Objects (Fixed)", 
    "Mixed Simple",
    "Lighting Test",
    "Reflection Test",
    "Complex Scene",
    "Shadow Test",
    "Material Test",
    "Original Scene",
    "Colorful Spheres",
    "Pyramid with Spheres",
    "Mixed Colorful Objects",
    "Many Spheres",
    "Complex Triangles",
    "Complex Mixed Scene",
    "Reflective Spheres",
    "Quadric Garden"
)

$totalScenes = $sceneFiles.Count
$successCount = 0
$failCount = 0

Write-Host "`nTesting $totalScenes scenes..." -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan

for ($i = 0; $i -lt $sceneFiles.Count; $i++) {
    $sceneFile = $sceneFiles[$i]
    $sceneName = $sceneNames[$i]
    $sceneNum = $i + 1
    
    Write-Host "`n[$sceneNum/$totalScenes] Testing: $sceneName" -ForegroundColor Yellow
    Write-Host "Scene file: $sceneFile"
    
    # Check if scene file exists
    if (-not (Test-Path $sceneFile)) {
        Write-Host "  ERROR: Scene file not found!" -ForegroundColor Red
        $failCount++
        continue
    }
    
    # Copy scene file to scene.txt (required by raytracer)
    Copy-Item $sceneFile "scene.txt" -Force
    
    # Run raytracer with timeout
    $timeout = 120 # 2 minutes timeout
    Write-Host "  Rendering... (timeout: ${timeout}s)" -ForegroundColor Gray
    
    try {
        # Use headless raytracer with direct scene file argument
        $outputName = "test_results\scene_$('{0:D2}' -f $sceneNum)_$($sceneName -replace ' ', '_').bmp"
        $process = Start-Process -FilePath ".\raytracer_headless.exe" -ArgumentList $sceneFile, $outputName -NoNewWindow -PassThru
        $process.WaitForExit($timeout * 1000)
        
        if (-not $process.HasExited) {
            Write-Host "  TIMEOUT: Killing process after ${timeout}s" -ForegroundColor Red
            $process.Kill()
            $failCount++
            continue
        }
        
        if ($process.ExitCode -eq 0) {
            # Check if output was generated with the specified name
            if (Test-Path $outputName) {
                Write-Host "  SUCCESS: Saved as $outputName" -ForegroundColor Green
                $successCount++
            } else {
                Write-Host "  ERROR: No output file generated at $outputName" -ForegroundColor Red
                $failCount++
            }
        } else {
            Write-Host "  ERROR: Raytracer returned exit code $($process.ExitCode)" -ForegroundColor Red
            $failCount++
        }
    }
    catch {
        Write-Host "  ERROR: Failed to run raytracer - $($_.Exception.Message)" -ForegroundColor Red
        $failCount++
    }
}

# Clean up
if (Test-Path "scene.txt") {
    Remove-Item "scene.txt" -Force
}

# Summary
Write-Host "`n========================================" -ForegroundColor Cyan
Write-Host "TEST SUMMARY" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Total scenes tested: $totalScenes"
Write-Host "Successful renders: $successCount" -ForegroundColor Green
Write-Host "Failed renders: $failCount" -ForegroundColor Red
Write-Host "Success rate: $([math]::Round(($successCount / $totalScenes) * 100, 1))%"

if ($successCount -eq $totalScenes) {
    Write-Host "`nAll tests passed! 🎉" -ForegroundColor Green
} elseif ($successCount -gt 0) {
    Write-Host "`nSome tests passed. Check failed scenes for issues." -ForegroundColor Yellow
} else {
    Write-Host "`nAll tests failed. Check raytracer compilation and scene files." -ForegroundColor Red
}

Write-Host "`nOutput images saved in: $outputDir\" -ForegroundColor Cyan
