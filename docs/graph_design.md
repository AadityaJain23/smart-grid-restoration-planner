# Smart Grid Graph Design

## 1. Objective

Represent a small smart-grid power distribution network using nodes and edges in C, supporting future routing, scheduling, and restoration algorithms.

## 2. Graph Representation

The network is represented as an undirected, weighted, capacitated graph using arrays of Node and Edge structures.

- Maximum nodes: 50
- Maximum edges: 200
- Initial network: 9 nodes and 8 edges

## 3. Node Properties

Each node stores an ID, name, component type, electricity demand, electricity supply, and restoration priority.

Node types are Generator, Substation, and Consumer.

## 4. Edge Properties

Each edge stores its source and destination node IDs, distance, transfer capacity, routing-cost score, and operational status.

## 5. Network Assumptions

- All distances are measured in kilometers.
- Demand, supply, and capacity are measured in MW.
- Transmission costs are simplified nonnegative scores.
- Priority 1 is the highest priority.
- Inactive edges remain stored but are unavailable for operational routing.
- The initial graph models connectivity and simplified transfer limits, not physical AC power flow.
- Parallel edges and self-loops are not allowed.

## 6. Graph Operations

The implementation supports:

1. initializeGraph()
2. addNode()
3. addEdge()
4. displayNodes()
5. displayEdges()
6. loadNetworkFromFile()

## 7. Testing

Tests verify node insertion, edge insertion, invalid inputs, duplicate detection, file loading, vertex degrees, supply-demand totals, and simulated line failures.

## 8. Future Extensions

The graph will support Dijkstra's shortest-path algorithm, Prim's and Kruskal's minimum spanning tree algorithms, and future outage-restoration logic.