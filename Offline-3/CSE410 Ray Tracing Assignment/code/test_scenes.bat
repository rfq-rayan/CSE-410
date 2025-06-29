@echo off
echo Ray Tracing Scene Test Suite
echo ============================

REM Check if raytracer exists
if not exist raytracer.exe (
    echo ERROR: raytracer.exe not found. Please compile first:
    echo g++ -o raytracer.exe 2005062_main.cpp intersection_implementations.cpp stb_image_impl.cpp -lfreeglut -lopengl32 -lglu32
    pause
    exit /b 1
)

REM Create output directory
if not exist test_results mkdir test_results

echo.
echo Testing 10 scenes...
echo ========================================

REM Scene 1: Basic Spheres
echo.
echo [1/10] Testing: Basic Spheres
copy "code\test_scenes\scene_01_basic_spheres.txt" "scene.txt" >nul
raytracer.exe "code\test_scenes\scene_01_basic_spheres.txt"
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_01_basic_spheres.bmp" >nul
        echo   SUCCESS: scene_01_basic_spheres.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 2: Pyramid
echo.
echo [2/10] Testing: Pyramid Triangles  
copy "code\test_scenes\scene_02_pyramid.txt" "scene.txt" >nul
raytracer.exe "code\test_scenes\scene_02_pyramid.txt"
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_02_pyramid.bmp" >nul
        echo   SUCCESS: scene_02_pyramid.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 3: Quadrics
echo.
echo [3/10] Testing: Quadric Objects
copy "code\test_scenes\scene_03_quadrics.txt" "scene.txt" >nul
raytracer.exe "code\test_scenes\scene_03_quadrics.txt"
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_03_quadrics.bmp" >nul
        echo   SUCCESS: scene_03_quadrics.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 4: Reflective Spheres
echo.
echo [4/10] Testing: Reflective Spheres
copy "code\test_scenes\scene_04_reflective_spheres.txt" "scene.txt" >nul
raytracer.exe
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_04_reflective_spheres.bmp" >nul
        echo   SUCCESS: scene_04_reflective_spheres.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 5: Mixed Objects
echo.
echo [5/10] Testing: Mixed Objects
copy "code\test_scenes\scene_05_mixed_objects.txt" "scene.txt" >nul
raytracer.exe
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_05_mixed_objects.bmp" >nul
        echo   SUCCESS: scene_05_mixed_objects.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 6: Single Quadric
echo.
echo [6/10] Testing: Single Quadric
copy "code\test_scenes\scene_06_single_quadric.txt" "scene.txt" >nul
raytracer.exe
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_06_single_quadric.bmp" >nul
        echo   SUCCESS: scene_06_single_quadric.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 7: Hexagon Base
echo.
echo [7/10] Testing: Hexagon Base
copy "code\test_scenes\scene_07_hexagon_base.txt" "scene.txt" >nul
raytracer.exe
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_07_hexagon_base.bmp" >nul
        echo   SUCCESS: scene_07_hexagon_base.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 8: Complex Scene
echo.
echo [8/10] Testing: Complex Scene
copy "code\test_scenes\scene_08_complex_scene.txt" "scene.txt" >nul
raytracer.exe
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_08_complex_scene.bmp" >nul
        echo   SUCCESS: scene_08_complex_scene.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 9: Floor Only
echo.
echo [9/10] Testing: Floor Only
copy "code\test_scenes\scene_09_floor_only.txt" "scene.txt" >nul
raytracer.exe
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_09_floor_only.bmp" >nul
        echo   SUCCESS: scene_09_floor_only.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Scene 10: Original Scene
echo.
echo [10/10] Testing: Original Scene
copy "code\test_scenes\scene_10_original.txt" "scene.txt" >nul
raytracer.exe
if exist Output_*.bmp (
    for %%f in (Output_*.bmp) do (
        move "%%f" "test_results\scene_10_original.bmp" >nul
        echo   SUCCESS: scene_10_original.bmp
    )
) else (
    echo   ERROR: No output generated
)

REM Cleanup
if exist scene.txt del scene.txt

echo.
echo ========================================
echo TEST COMPLETE
echo ========================================
echo Check test_results\ folder for output images
pause
