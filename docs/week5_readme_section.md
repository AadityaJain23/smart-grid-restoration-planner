Overview
Week 5 extends the Smart Grid Power Distribution and Restoration Planner with priority-aware electricity allocation.

The implementation maximizes total feasible electricity delivery while lexicographically prioritizing consumers according to their assigned priority classes.

Priority 1 consumers are optimized first, followed by Priority 2 and Priority 3.

The implementation uses a capacity-constrained graph model rather than a physical AC power-flow simulation.
Main Features
- Maximum feasible electricity delivery.
- Lexicographic consumer priority optimization.
- Residual-network construction.
- BFS-based augmenting-path search.
- Negative-cycle detection and cancellation.
- Generator supply and consumer demand constraints.
- Transmission-line capacity and failure handling.
- Consumer-level allocation reporting.
- Integration with CLI option 24.
Source Files
File	Purpose
include/priority_flow.h	Public API and data structures
src/priority_flow.c	Priority-Aware Flow implementation
src/maxflow.c	Existing Maximum Flow implementation
src/main.c	CLI integration

Build Requirements
- GCC supporting C11.
- Windows PowerShell or a compatible command-line environment.
- No external runtime libraries beyond the standard C library and math library.
Compile the Complete Application
From the repository root, run:
Run the Application
.\smartgrid.exe


Load the original network using menu option 1 and the file:

data/network.txt

To test Maximum Flow, select option 23.

To test Priority-Aware Flow, select option 24.

For the limited-supply scenario, load:

data/network_60mw.txt
Verified Network Results
Network	Total Delivered	Priority 1	Priority 2	Priority 3
Original 100 MW	100 MW	20 MW	10 MW	70 MW
Limited 60 MW	60 MW	20 MW	10 MW	30 MW

Both flow algorithms maximize total feasible throughput. Priority-Aware Flow additionally optimizes its distribution among priority classes.
Automated Integration Tests
Run:
powershell -NoProfile -ExecutionPolicy Bypass `
    -File tests/run_week5_lecture10.ps1


The script compiles and executes six integration regression programs.

The reported successful regression summary is:
Total Tests : 6
Passed      : 6
Failed      : 0


Regression results are saved in:

docs/test_results/week5_lecture10.csv
Technical Documentation
- Priority Flow Technical Design
- Code Quality Audit
- Documentation Inventory
Known Limitations
- The system models capacity-constrained electricity allocation rather than AC power flow.
- Extremely small capacities may be affected by numerical tolerances.
- Negative-cycle cancellation uses an iteration safety limit.
- A polynomial worst-case bound for the current cycle-cancellation implementation has not been established.
- Multiple allocations within the same priority class may be equally optimal.
Outstanding Historical Test Issue
The historical test_priority_negative_cycle.c program currently has an unresolved dependency on runPriorityFlowPrototype().

The production solver, strict production build and six Lecture 10 integration regression programs have passed their reported verification checks.

The historical test compatibility issue remains open and must be resolved before claiming that all historical tests pass.