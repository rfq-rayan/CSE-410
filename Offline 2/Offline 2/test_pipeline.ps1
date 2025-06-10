# Comprehensive Testing Script for Graphics Pipeline
# Usage: powershell -ExecutionPolicy Bypass -File test_pipeline.ps1

param(
    [string]$TestCase = "all",
    [switch]$Verbose
)

Write-Host "Offile - 2 -- Raster Based Graphics Pipeline " -ForegroundColor Green
Write-Host ("=" * 60) -ForegroundColor Green

# Compile the code
Write-Host "`nCompiling code.cpp..." -ForegroundColor Yellow
$compileResult = & g++ -std=c++17 -o code.exe code.cpp 2>&1
if ($LASTEXITCODE -ne 0) {
    Write-Host "Compilation failed!" -ForegroundColor Red
    Write-Host $compileResult
    exit 1
}
Write-Host "Compilation successful!" -ForegroundColor Green

# Function to compare two files
function Compare-OutputFiles {
    param($file1, $file2, $stageName)
    
    if (-not (Test-Path $file1)) {
        Write-Host "    $stageName`: FAILED - Output file missing: $file1" -ForegroundColor Red
        return $false
    }
    
    if (-not (Test-Path $file2)) {
        Write-Host "    $stageName`: FAILED - Reference file missing: $file2" -ForegroundColor Red
        return $false
    }
    
    $content1 = Get-Content $file1 -ErrorAction SilentlyContinue
    $content2 = Get-Content $file2 -ErrorAction SilentlyContinue
    
    $diff = Compare-Object $content1 $content2
    
    if ($diff) {
        Write-Host "    $stageName`: FAILED - Output differs from reference" -ForegroundColor Red
        if ($Verbose) {
            Write-Host "      First few differences:" -ForegroundColor Yellow
            $diff | Select-Object -First 3 | ForEach-Object {
                if ($_.SideIndicator -eq "<=") {
                    Write-Host "        Your output:  $($_.InputObject)" -ForegroundColor Red
                } else {
                    Write-Host "        Expected:     $($_.InputObject)" -ForegroundColor Green
                }
            }
        }
        return $false
    } else {
        Write-Host "    $stageName`: PASSED" -ForegroundColor Green
        return $true
    }
}

# Function to test a single test case
function Test-SingleCase {
    param($caseNumber)
    
    Write-Host "`nTesting Case $caseNumber" -ForegroundColor Yellow
    Write-Host ("-" * 30)
    
    $testDir = "Test Cases\$caseNumber"
    $outputDir = "output_$caseNumber"
    
    # Check if test case exists
    if (-not (Test-Path $testDir)) {
        Write-Host "  Test case $caseNumber not found!" -ForegroundColor Red
        return @{case=$caseNumber; passed=0; total=0}
    }
    
    # Run the pipeline
    Write-Host "  Running pipeline..." -ForegroundColor Cyan
    $runResult = & .\code.exe "$testDir\scene.txt" "$testDir\config.txt" $outputDir 2>&1
    
    if ($LASTEXITCODE -ne 0) {
        Write-Host "  Pipeline execution failed!" -ForegroundColor Red
        Write-Host "  Error: $runResult" -ForegroundColor Red
        return @{case=$caseNumber; passed=0; total=0}
    }
    
    Write-Host "  Pipeline executed successfully" -ForegroundColor Green
    
    # Compare outputs
    $passed = 0
    $total = 0
    
    # Test Stage 1
    if (Test-Path "$testDir\stage1.txt") {
        $total++
        if (Compare-OutputFiles "$outputDir\stage1.txt" "$testDir\stage1.txt" "Stage 1") {
            $passed++
        }
    }
    
    # Test Stage 2
    if (Test-Path "$testDir\stage2.txt") {
        $total++
        if (Compare-OutputFiles "$outputDir\stage2.txt" "$testDir\stage2.txt" "Stage 2") {
            $passed++
        }
    }
    
    # Test Stage 3
    if (Test-Path "$testDir\stage3.txt") {
        $total++
        if (Compare-OutputFiles "$outputDir\stage3.txt" "$testDir\stage3.txt" "Stage 3") {
            $passed++
        }
    }
    
    # # Test Stage 4 (Z-buffer)
    # if (Test-Path "$testDir\z_buffer.txt") {
    #     $total++
    #     if (Compare-OutputFiles "$outputDir\z_buffer.txt" "$testDir\z_buffer.txt" "Stage 4 (Z-buffer)") {
    #         $passed++
    #     }
    # }
    
    # Check if BMP file was generated
    if (Test-Path "$outputDir\out.bmp") {
        Write-Host "    BMP Output: Generated successfully" -ForegroundColor Green
        $bmpSize = (Get-Item "$outputDir\out.bmp").Length
        Write-Host "    BMP Size: $bmpSize bytes" -ForegroundColor Cyan
    } else {
        Write-Host "    BMP Output: FAILED - File not generated" -ForegroundColor Red
    }
    
    Write-Host "  Case $caseNumber Result: $passed/$total stages passed" -ForegroundColor Cyan
    
    return @{case=$caseNumber; passed=$passed; total=$total}
}

# Main testing logic
$results = @()

if ($TestCase -eq "all") {
    # Test all cases
    for ($i = 1; $i -le 4; $i++) {
        $result = Test-SingleCase $i
        $results += $result
    }
} else {
    # Test specific case
    $result = Test-SingleCase $TestCase
    $results += $result
}

# Summary
Write-Host "`n$("=" * 60)" -ForegroundColor Green
Write-Host "SUMMARY RESULTS" -ForegroundColor Green
Write-Host ("-" * 60)

$totalPassed = 0
$totalTests = 0

foreach ($result in $results) {
    $totalPassed += $result.passed
    $totalTests += $result.total
    $percentage = if ($result.total -gt 0) { [math]::Round(($result.passed / $result.total) * 100, 1) } else { 0 }
    Write-Host "Test Case $($result.case): $($result.passed)/$($result.total) passed ($percentage%)" -ForegroundColor $(if ($result.passed -eq $result.total) { "Green" } else { "Yellow" })
}

Write-Host ("-" * 60)
$overallPercentage = if ($totalTests -gt 0) { [math]::Round(($totalPassed / $totalTests) * 100, 1) } else { 0 }
Write-Host "OVERALL: $totalPassed/$totalTests tests passed ($overallPercentage%)" -ForegroundColor Cyan

if ($totalPassed -eq $totalTests -and $totalTests -gt 0) {
    Write-Host "ALL TESTS PASSED! " -ForegroundColor Green
} elseif ($overallPercentage -ge 75) {
    Write-Host "`n✅ Good progress! Most tests are passing." -ForegroundColor Yellow
} else {
    Write-Host "`n❌ Several tests failed. Check the details above." -ForegroundColor Red
}

Write-Host "`nUsage Examples:" -ForegroundColor Cyan
Write-Host "  Test all cases: powershell -ExecutionPolicy Bypass -File test_pipeline.ps1"
Write-Host "  Test case 1: powershell -ExecutionPolicy Bypass -File test_pipeline.ps1 -TestCase 1"
Write-Host "  Verbose mode: powershell -ExecutionPolicy Bypass -File test_pipeline.ps1 -Verbose"
Write-Host "  Manual run: .\code.exe ""Test Cases\1\scene.txt"" ""Test Cases\1\config.txt"" ""my_output"""
