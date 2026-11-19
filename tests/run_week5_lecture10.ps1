
$ErrorActionPreference = "Stop"

# Run from the repository root, regardless
# of the terminal's initial working directory.
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root

$tests = @(
    "test_priority_system_invariants",
    "test_priority_cross_algorithm",
    "test_priority_restoration_consistency",
    "test_priority_capacity_boundaries",
    "test_priority_state_preservation",
    "test_priority_determinism"
)

$passed = 0
$failed = 0

$results = @()

New-Item -ItemType Directory -Force `
    -Path "docs/test_results" | Out-Null

foreach ($test in $tests) {

    Write-Host ""
    Write-Host "========================================"
    Write-Host "Testing: $test"
    Write-Host "========================================"

    $source = "tests/$test.c"
    $output = "$test.exe"

    # Remove any previous executable so
    # stale binaries cannot be reused.
    if (Test-Path $output) {
        Remove-Item $output -Force
    }

    if (!(Test-Path $source)) {

        Write-Host "[FAIL] Missing source: $source"

        $failed++

        $results += [PSCustomObject]@{
            Test = $test
            Status = "FAIL"
            Reason = "Missing source"
        }

        continue
    }

    # Compile with the same settings used
    # throughout Week 5.
    gcc -std=c11 -Wall -Wextra -Wpedantic `
        -Wno-unused-function -Iinclude `
        $source `
        src/graph.c `
        src/maxflow.c `
        src/priority_flow.c `
        -o $output -lm

    if ($LASTEXITCODE -ne 0) {

        Write-Host "[FAIL] Compilation: $test"

        $failed++

        $results += [PSCustomObject]@{
            Test = $test
            Status = "FAIL"
            Reason = "Compilation"
        }

        continue
    }

    # Run the newly compiled executable.
    & ".\$output"

    if ($LASTEXITCODE -eq 0) {

        Write-Host "[PASS] $test"

        $passed++

        $results += [PSCustomObject]@{
            Test = $test
            Status = "PASS"
            Reason = "Completed"
        }
    }
    else {

        Write-Host "[FAIL] Execution: $test"

        $failed++

        $results += [PSCustomObject]@{
            Test = $test
            Status = "FAIL"
            Reason = "Execution"
        }
    }
}

Write-Host ""
Write-Host "========================================"
Write-Host " WEEK 5 LECTURE 10 REGRESSION SUMMARY"
Write-Host "========================================"
Write-Host "Total Tests : $($tests.Count)"
Write-Host "Passed      : $passed"
Write-Host "Failed      : $failed"
Write-Host "========================================"

$results | Export-Csv `
    -Path "docs/test_results/week5_lecture10.csv" `
    -NoTypeInformation

if ($failed -eq 0) {
    Write-Host "[PASS] All Lecture 10 integration tests"
    exit 0
}
else {
    Write-Host "[FAIL] Lecture 10 integration regression"
    exit 1
}
