
# Smart Grid Scenario Analysis

## Project
Smart Grid Power Distribution and Restoration Planner

## Objective
Evaluate the existing graph-based smart-grid simulation
under normal conditions, transmission-line failures,
and analytical operations.

## Network
- Nodes: 9
- Transmission lines: 8
- Generators: 1
- Substations: 3
- Consumers: 5
- Total generation: 100 MW
- Total demand: 100 MW

## Scenario 1: Normal Operation
All transmission lines are active.

Expected:
- Reachable demand: 100 MW
- Disconnected demand: 0 MW
- Active lines: 8

## Scenario 2: Hospital Line Failure
Failed line: 1-4

Expected:
- Hospital disconnected
- Disconnected demand: 20 MW
- Active lines: 7

## Scenario 3: Substation A Branch Failure
Failed line: 0-1

Expected:
- Hospital disconnected
- Residential Area disconnected
- Disconnected demand: 45 MW

## Scenario 4: Multiple Failures
Failed lines: 0-1 and 0-2

Expected:
- Disconnected demand: 75 MW
- Reachable demand: 25 MW

## Scenario 5: Consumer Sorting
Algorithm: Merge Sort

Expected demand order:
10, 15, 20, 25, 30 MW

## Scenario 6: Transmission-Line Sorting
Algorithm: Quick Sort

Expected cost order:
6, 6, 8, 8, 10, 10, 12, 14

## Scenario 7: Node Lookup
Algorithms:
- Iterative Binary Search
- Recursive Binary Search

Target Node ID: 4
Expected: Hospital

## Scenario 8: Demand Surge
Algorithm: Maximum Subarray

Expected:
- Maximum cumulative increase: 16 MW
- Interval: 1 to 4

## Scenario 9: Demand Projection
Algorithm: Fast Exponentiation

Inputs:
- Initial demand: 100 MW
- Growth: 5% annually
- Years: 5

Expected:
- Projected demand: 127.63 MW

## Limitations
The current system models graph connectivity and
supporting analysis operations.

Reachability does not prove capacity-feasible power delivery.

The current implementation does not yet perform:
- Minimum-cost route optimization
- Capacity-constrained power allocation
- Automatic restoration planning
- AC power-flow simulation

## Test Results
Record actual execution results and dates here.
