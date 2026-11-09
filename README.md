
# Smart Grid Power Distribution and Restoration Planner

## Overview

A C11-based, offline, menu-driven smart-grid simulation
for representing electricity infrastructure, analyzing
network connectivity, simulating transmission-line
failures, and performing demand-related analysis.

## Current Features

### Network Foundation
- Generator, substation, and consumer records
- Edge-list and adjacency-list graph representation
- Network loading and validation
- Transmission-line failure and restoration
- Generator reachability
- Disconnected-demand reporting
- Network statistics

### Sorting and Searching
- Merge Sort: consumer demand ranking
- Quick Sort: transmission-line cost ranking
- Iterative Binary Search: node-ID lookup
- Recursive Binary Search: node-ID lookup

### Demand Analysis
- Maximum Subarray: cumulative demand-surge analysis
- Fast Exponentiation: compound demand projection

## Technology

- Language: C11
- Compiler: GCC
- Interface: Command Line
- Platform: Windows / PowerShell
- External dependencies: None

## Build

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/main.c src/graph.c src/sorting.c src/searching.c src/divide_conquer.c -o smartgrid.exe -lm
```

## Run

```powershell
.\smartgrid.exe
```

Load `data/network.txt` using menu option 1.

## Testing

Automated tests are available in the `tests/` folder.

See `docs/testing.md` for compilation commands
and verification scenarios.

## Current Limitations

The current system evaluates graph connectivity
and supporting demand analysis.

It does not yet implement:
- Capacity-constrained power allocation
- Minimum-cost power routing
- Automatic restoration optimization
- Physical AC power-flow simulation

## Future Development

Add meaningful graph optimization and restoration
algorithms while preserving the existing modular
architecture.
