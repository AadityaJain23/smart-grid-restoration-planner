
#ifndef ROUTING_H
#define ROUTING_H

#include "graph.h"

/*
 * Stores the result of Dijkstra's algorithm.
 * All array positions represent graph node indices,
 * not necessarily node IDs.
 */
typedef struct {
    double distance[MAX_NODES];
    int parent[MAX_NODES];
    int visited[MAX_NODES];
    int sourceIndex;
} DijkstraResult;

/*
 * Run Dijkstra from the specified source node ID.
 *
 * Returns:
 * 1 = success
 * 0 = invalid input or algorithm failure
 *
 * Unreachable nodes have distance INFINITY
 * and parent -1.
 */
int dijkstraShortestPaths(
    const Graph *graph,
    const AdjList *list,
    int sourceId,
    DijkstraResult *result
);

/*
 * Display the shortest path to a destination node ID.
 *
 * Returns:
 * 1 = path displayed
 * 0 = invalid input or unreachable destination
 */
int displayShortestPath(
    const Graph *graph,
    const DijkstraResult *result,
    int destinationId
);

#endif
