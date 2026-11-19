# Week 5 — Reproducibility Verification
Objective
Verify that the Smart Grid Power Distribution and Restoration Planner can be compiled, executed and tested using the documented offline C11 workflow.
Environment
- Language: C11
- Compiler: GCC
- Interface: Command-line application
- Operating system: Windows
- Shell: PowerShell
- External runtime dependencies: None beyond the C standard library and math library
Production Build
The application is compiled from the source files in src/, including priority_flow.c and maxflow.c.

Strict compiler flags:

-std=c11 -Wall -Wextra -Wpedantic -Werror

Build result: To be recorded after verification.
Automated Regression
Regression script:

tests/run_week5_lecture10.ps1

The script executes six independent integration test programs.

Regression result: To be recorded after verification.
Network Verification
Original Network
Input: data/network.txt

Expected total delivery: 100 MW

Expected Priority Flow allocation: P1=20 MW, P2=10 MW, P3=70 MW.
Limited Network
Input: data/network_60mw.txt

Expected total delivery: 60 MW

Expected Priority Flow allocation: P1=20 MW, P2=10 MW, P3=30 MW.
Evidence
- docs/test_results/week5_lecture10.csv
- docs/week5_testing_report.md
- docs/week5_performance_report.md
Outstanding Issue
The historical test_priority_negative_cycle.c program has an unresolved dependency on runPriorityFlowPrototype().

This does not invalidate the reported production regression results, but complete historical regression compatibility remains pending.
Final Status
Reproducibility verification pending completion of the clean build, regression and CLI checks.

After running the verification commands, update the pending fields with the actual results.

Do not mark the reproducibility record as passed before completing the checks.