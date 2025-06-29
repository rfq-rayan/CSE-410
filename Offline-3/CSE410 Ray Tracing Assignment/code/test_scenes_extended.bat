@echo off
echo Ray Tracing Extended Test Suite
echo ===============================

REM Check if headless raytracer exists
if not exist raytracer_headless.exe (
    echo ERROR: raytracer_headless.exe not found. Please compile first:
    echo See compile.ps1 or compile.bat
    pause
    exit /b 1
)

REM Create output directory
if not exist test_results mkdir test_results

set /a count=0
set /a success=0
set /a failed=0

echo.
echo Testing 18 scenes...
echo ========================================

REM Test all scenes
for %%s in (
    "test_scenes\scene_01_basic_spheres.txt:Basic_Spheres"
    "test_scenes\scene_02_triangles.txt:Triangle_Objects"
    "test_scenes\scene_03_quadrics.txt:Quadric_Objects_Fixed"
    "test_scenes\scene_04_mixed_simple.txt:Mixed_Simple"
    "test_scenes\scene_05_lighting_test.txt:Lighting_Test"
    "test_scenes\scene_06_reflection_test.txt:Reflection_Test"
    "test_scenes\scene_07_complex_scene.txt:Complex_Scene"
    "test_scenes\scene_08_shadows.txt:Shadow_Test"
    "test_scenes\scene_09_materials.txt:Material_Test"
    "test_scenes\scene_10_original.txt:Original_Scene"
    "test_scenes\scene_11_colorful_spheres.txt:Colorful_Spheres"
    "test_scenes\scene_12_pyramid_with_spheres.txt:Pyramid_with_Spheres"
    "test_scenes\scene_13_mixed_objects_colorful.txt:Mixed_Colorful_Objects"
    "test_scenes\scene_14_many_spheres.txt:Many_Spheres"
    "test_scenes\scene_15_triangle_complex.txt:Complex_Triangles"
    "test_scenes\scene_16_complex_mixed.txt:Complex_Mixed_Scene"
    "test_scenes\scene_17_reflective_spheres.txt:Reflective_Spheres"
    "test_scenes\scene_18_quadric_garden.txt:Quadric_Garden"
) do (
    set /a count+=1
    for /f "tokens=1,2 delims=:" %%a in (%%s) do (
        echo.
        echo [!count!/18] Testing: %%b
        if exist "%%a" (
            raytracer_headless.exe "%%a"
            if exist output.bmp (
                move output.bmp "test_results\%%b.bmp" >nul 2>&1
                echo   SUCCESS: %%b.bmp created
                set /a success+=1
            ) else (
                echo   ERROR: No output generated
                set /a failed+=1
            )
        ) else (
            echo   ERROR: Scene file not found: %%a
            set /a failed+=1
        )
    )
)

echo.
echo ========================================
echo Test Summary:
echo   Successful: %success%
echo   Failed: %failed%
echo   Total: %count%
echo.
echo Output files are in the test_results directory.
echo ========================================

pause
