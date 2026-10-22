# Smart Grid Planner — Input and Output Specification

## 1. Objective

Develop an offline C-based smart-grid network model that supports graph construction, validation, connectivity analysis, failure simulation, and network reporting.

## 2. Input Data

### Node Records

Each node contains:

- Unique node ID
- Component name
- Component type
- Power demand (MW)
- Generation supply (MW)
- Restoration priority

Supported node types:

- Generator
- Substation
- Consumer

### Transmission-Line Records

Each transmission line contains:

- Source node ID
- Destination node ID
- Distance (km)
- Maximum transfer capacity (MW)
- Routing-cost score
- Operational status (active/inactive)

### Network File

The first value specifies the number of nodes, followed by node records. The next value specifies the number of transmission lines, followed by edge records.

## 3. Current Week 1 Outputs

The application produces:

- Node and transmission-line tables
- Adjacency-list connectivity
- Direct connection checks
- Active degree of nodes
- Isolated-node identification
- Generator reachability
- Disconnected consumer identification
- Total supply and demand
- Reachable and disconnected demand
- Active and failed line counts
- Critical disconnected consumer count
- Aggregate active line capacity and cost scores

## 4. Future Project Outputs

Later algorithms will calculate:

- Selected power routes
- Capacity-constrained power allocation
- Critical-load restoration decisions
- Routing or operational costs
- Served and unserved demand

These outputs are not implemented in the Week 1 foundation.

## 5. Assumptions

- The initial network is undirected.
- Node and edge arrays have fixed maximum capacities.
- The adjacency list contains only active transmission lines.
- Priority 1 represents the highest restoration importance.
- Power values are measured in MW.
- Line costs are simplified model scores.
- Generator reachability does not guarantee feasible power delivery.
- The implementation is a simplified network simulator, not a physical AC power-flow solver.