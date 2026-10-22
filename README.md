# Smart Grid Power Distribution and Restoration Planner

**Design and Analysis of Algorithms (DAA) — Project-Based Learning**

## 1. Project Overview

The Smart Grid Power Distribution and Restoration Planner is a C-based, command-line application designed to simulate an electrical power distribution network.

The system represents substations, power lines, generation sources, and consumer loads using data structures and graph algorithms.

Its primary objective is to analyze power distribution routes, identify network failures, and plan restoration strategies while considering factors such as transmission costs, network connectivity, line capacities, and critical load priorities.

The project is developed in two phases and focuses on implementing and comparing algorithms studied in Design and Analysis of Algorithms (DAA).

## 2. Project Objectives

- Model an electrical distribution network using graphs.
- Represent substations, transmission lines, and consumer loads using C structures.
- Implement fundamental algorithms without relying on external algorithm libraries.
- Find cost-efficient routes through the network.
- Analyze network connectivity and infrastructure requirements.
- Simulate power-line and substation failures.
- Develop restoration strategies that prioritize critical loads.
- Compare algorithms based on execution time, complexity, and performance.
- Run the complete application offline.

## 3. Technology Stack

| Component | Technology |
|---|---|
| Programming Language | C |
| Interface | Command Line Interface (CLI) |
| Compiler | GCC |
| Development Environment | VS Code / Terminal |
| Version Control | Git and GitHub |
| Data Storage | Local CSV / Text Files |
| Platform | Windows / Linux |
| External Dependencies | None required |

## 4. Project Structure

```text
SmartGrid-Restoration/
│
├── src/                # C source code and algorithms
├── include/            # Header files and structures
├── data/               # Smart grid datasets
├── tests/              # Algorithm testing
├── docs/               # Documentation and diagrams
├── Phase_I/            # Phase I evaluation materials
├── Phase_II/           # Phase II evaluation materials
│
├── README.md           # Project documentation
└── .gitignore          # Git ignored files
```

## 5. Planned Features

### Phase I — Core Algorithm Implementation

- Create and manage a small smart grid network.
- Represent the network using weighted graphs.
- Implement sorting and searching algorithms.
- Implement greedy algorithms for routing and infrastructure analysis.
- Implement divide-and-conquer algorithms.
- Simulate basic power-line failures.
- Display network information and algorithm results through the CLI.
- Analyze theoretical time and space complexity.

### Phase II — Advanced Optimization and Restoration

- Extend the network with additional constraints.
- Implement dynamic programming algorithms.
- Develop load restoration and prioritization strategies.
- Compare alternative algorithms for selected optimization problems.
- Evaluate performance using different input sizes.
- Analyze restoration effectiveness and network reliability.
- Generate comparative results and final documentation.

## 6. Planned Algorithms

| Algorithm | Application |
|---|---|
| Merge Sort | Sort loads, substations, or power-line records |
| Quick Sort | Alternative sorting implementation |
| Binary Search | Search sorted network records |
| Dijkstra's Algorithm | Find minimum-weight network routes |
| Prim's Algorithm | Compute a minimum spanning tree |
| Kruskal's Algorithm | Alternative minimum spanning tree computation |
| Activity Selection | Select compatible maintenance activities |
| Interval Partitioning | Schedule overlapping maintenance tasks |
| Huffman Coding | Compress network event information |
| Maximum Subarray | Analyze contiguous sequences of load variations |
| Dynamic Programming | Solve selected optimization and restoration problems |

Additional syllabus algorithms will be incorporated according to the project requirements.

**Note:** Shortest-path and minimum-spanning-tree algorithms provide useful network planning results, but do not independently guarantee electrically feasible power flow. Capacity and demand constraints will be checked separately in the simulation.

## 7. Getting Started

### Prerequisites

Install:

- GCC Compiler
- Git
- VS Code (optional)

Verify the compiler:

```bash
gcc --version
```

### Clone the Repository

```bash
git clone <repository-url>
cd SmartGrid-Restoration
```

Replace `<repository-url>` with the actual GitHub repository URL.

### Compile the Project

For the initial setup:

```bash
gcc src/main.c -o main.exe
```

### Run the Project

On Windows:

```bash
.\main.exe
```

On Linux:

```bash
gcc src/main.c -o main
./main
```

Compilation instructions will be updated as additional source files are implemented.

## 8. Simulation Assumptions

The initial prototype will use a small, simplified electrical distribution network.

- Substations and other network connection points are represented as graph vertices.
- Power lines are represented as graph edges.
- Edges may have weights such as cost, distance, or capacity.
- Consumer loads have specified demand and priority.
- Failures may temporarily disable substations or transmission lines.
- Restoration decisions must respect available network connectivity and modeled constraints.
- The system is an educational planning simulation, not a physical power-flow solver.

## 9. Testing and Performance Evaluation

The project will be evaluated using:

- Small predefined network examples.
- Normal and failure scenarios.
- Disconnected graphs.
- Alternative route availability.
- Invalid input handling.
- Algorithm correctness tests.
- Execution-time measurements.
- Theoretical time and space complexity.
- Comparative algorithm analysis.

## 10. Development Status

**Current Stage: Phase I — Initial Project Setup**

- [ ] Initialize Git repository
- [ ] Create project folder structure
- [ ] Configure `.gitignore`
- [ ] Verify GCC compiler
- [ ] Compile and run initial C program
- [ ] Design graph data structures
- [ ] Implement initial graph operations
- [ ] Implement and test core algorithms
- [ ] Complete Phase I evaluation
- [ ] Implement Phase II extensions

## 11. Project Guidelines

- Implement core algorithms independently in C.
- Do not use external algorithm libraries for required implementations.
- Keep the application functional without internet connectivity.
- Maintain modular and readable source code.
- Test algorithms with appropriate input cases.
- Document algorithm complexity and implementation decisions.
- Use Git for version control and development history.


## Week 1 — Smart Grid Foundation

### Overview

The Smart Grid Power Distribution and Restoration Planner is an offline C-based project that models electrical distribution networks using graph data structures.

The system represents generators, substations, consumers, and transmission lines. It supports network validation, connectivity analysis, failure simulation, and operational reporting.

### Implemented Features

- Node and transmission-line management
- Edge-list and adjacency-list graph representation
- Network loading from text files
- Input validation
- Interactive command-line interface
- Active and inactive transmission-line management
- Direct connectivity and active-degree analysis
- Isolated-node detection
- Generator reachability analysis
- Disconnected-demand calculation
- Network statistics and reporting
- Automated unit and integration tests

### Technologies

- C11
- GCC
- Standard C library
- Array-based graph data structures

### Compilation

```bash
gcc -std=c11 -Wall -Wextra -Iinclude src/main.c src/graph.c -o smartgrid.exe -lm
```

### Execution

Run from the project root:

```powershell
.\smartgrid.exe
```

Load the sample network using:

```text
data/network.txt
```

### Testing

```bash
gcc -std=c11 -Wall -Wextra -Iinclude tests/test_graph.c src/graph.c -o test_graph.exe -lm
```

Run:

```powershell
.\test_graph.exe
```

### Sample Network

The initial network contains:

- 1 generator
- 3 substations
- 5 consumers
- 8 transmission lines
- 100 MW generation capacity
- 100 MW total demand

### Future Development

The next development stages will integrate sorting, searching, divide-and-conquer, greedy algorithms, and other DAA techniques for network planning and restoration.

Capacity-constrained power allocation and optimized restoration are not part of the Week 1 implementation.

## 12. Future Scope

- Larger smart grid networks.
- Improved fault detection and restoration planning.
- Multi-objective optimization.
- Advanced load balancing.
- Network reliability analysis.
- More detailed simulation of generation and transmission constraints.

---

**Project:** Smart Grid Power Distribution and Restoration Planner  
**Course:** Design and Analysis of Algorithms (DAA)  
**Language:** C  
**Interface:** Command Line Interface  
**Development Model:** Two-Phase Project-Based Learning