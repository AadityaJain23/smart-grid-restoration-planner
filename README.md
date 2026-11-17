# Smart Grid Power Distribution and Restoration Planner

## Overview

An offline, menu-driven C11 application for modeling a simplified electricity distribution network, analyzing transmission-line failures, finding economical routes, evaluating minimum-cost connectivity and planning priority-based power allocation.

## Features

- Graph representation using edge lists and adjacency lists
- Network loading and input validation
- Transmission-line failure and restoration simulation
- Generator reachability and disconnected-demand analysis
- Merge Sort and Quick Sort for demand and line-cost analysis
- Iterative and recursive Binary Search
- Maximum Subarray demand-surge analysis
- Fast Exponentiation for demand projection
- Dijkstra's shortest-path algorithm
- Prim's and Kruskal's minimum spanning tree algorithms
- Disjoint Set Union with path compression and union by rank
- Priority-based, capacity-aware restoration heuristic

## Technology

- Language: C11
- Compiler: GCC
- Interface: Command-line interface
- Development environment: VS Code and Windows PowerShell
- Dependencies: Standard C library

## Build

From the repository root:

```powershell
gcc -std=c11 -Wall -Wextra -Wpedantic -Iinclude src/main.c src/graph.c src/sorting.c src/searching.c src/divide_conquer.c src/routing.c src/mst.c src/dsu.c src/restoration.c -o smartgrid.exe -lm
```

## Run

```powershell
.\smartgrid.exe
```

Load a network using menu option 1.

Available sample networks:

- `data/network.txt`
- `data/network_redundant.txt`

The demand-change dataset is stored in `data/demand_changes.txt`.

## Testing

Automated tests are available in the `tests/` directory.

The reported Week 3 test suites cover Dijkstra, DSU, minimum spanning trees and capacity-aware restoration.

## Important Limitations

This project is a graph-based educational simulation.

Its restoration module uses a sequential greedy allocation heuristic and simplified transmission-line capacity accounting. It does not implement physical AC power flow, voltage calculations, reactive power or globally optimal electricity dispatch.

## Future Improvements

- More advanced network-flow optimization
- Improved routing under shared capacity constraints
- Larger network benchmarking
- Additional restoration scenarios
- Enhanced reporting and visualization