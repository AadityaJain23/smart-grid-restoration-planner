
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "routing.h"

/* Convert a logical node ID into its graph array index. */
static int findRoutingNodeIndex(const Graph *graph, int nodeId) {
    if (graph == NULL) {
        return -1;
    }

    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i].id == nodeId) {
            return i;
        }
    }

    return -1;
}

/* Find the unvisited node with the smallest distance. */
static int findMinimumDistanceNode(
    const DijkstraResult *result,
    int nodeCount
) {
    int minIndex = -1;
    double minDistance = INFINITY;

    for (int i = 0; i < nodeCount; i++) {
        if (!result->visited[i] &&
            result->distance[i] < minDistance) {
            minDistance = result->distance[i];
            minIndex = i;
        }
    }

    return minIndex;
}

/* Calculate shortest paths from a source node ID. */
int dijkstraShortestPaths(
    const Graph *graph,
    const AdjList *list,
    int sourceId,
    DijkstraResult *result
) {
    if (graph == NULL || list == NULL || result == NULL) {
        return 0;
    }

    if (graph->nodeCount <= 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES) {
        return 0;
    }

    if (!validateAdjacencyList(graph, list)) {
        return 0;
    }

    int sourceIndex = findRoutingNodeIndex(graph, sourceId);

    if (sourceIndex == -1) {
        return 0;
    }

    /* Validate routing costs, including inactive edges. */
    for (int i = 0; i < graph->edgeCount; i++) {
        double cost = graph->edges[i].cost;

        if (!isfinite(cost) || cost < 0.0) {
            return 0;
        }
    }

    /* Initialize output only after validating the input. */
    result->sourceIndex = sourceIndex;

    for (int i = 0; i < MAX_NODES; i++) {
        result->distance[i] = INFINITY;
        result->parent[i] = -1;
        result->visited[i] = 0;
    }

    result->distance[sourceIndex] = 0.0;

    /* Main Dijkstra loop. */
    for (int count = 0; count < graph->nodeCount; count++) {
        int u = findMinimumDistanceNode(
            result, graph->nodeCount
        );

        if (u == -1) {
            break;
        }

        result->visited[u] = 1;

        int current = list->head[u];

        while (current != -1) {
            AdjEntry entry = list->entries[current];
            int v = entry.neighborIndex;

            const Edge *edge = &graph->edges[entry.edgeIndex];

            if (!result->visited[v] && edge->active == 1) {
                double weight = edge->cost;

                /* Prevent overflow in distance addition. */
                if (result->distance[u] <= DBL_MAX - weight) {
                    double candidate =
                        result->distance[u] + weight;

                    if (candidate < result->distance[v]) {
                        result->distance[v] = candidate;
                        result->parent[v] = u;
                    }
                }
            }

            current = entry.next;
        }
    }

    return 1;
}

/* Display a reconstructed shortest path. */
int displayShortestPath(
    const Graph *graph,
    const DijkstraResult *result,
    int destinationId
) {
    if (graph == NULL || result == NULL ||
        graph->nodeCount <= 0 ||
        graph->nodeCount > MAX_NODES) {
        return 0;
    }

    int destinationIndex =
        findRoutingNodeIndex(graph, destinationId);

    if (destinationIndex == -1 ||
        result->sourceIndex < 0 ||
        result->sourceIndex >= graph->nodeCount) {
        return 0;
    }

    if (!isfinite(result->distance[destinationIndex])) {
        printf("No active route exists to node %d.\n",
               destinationId);
        return 0;
    }

    int path[MAX_NODES];
    int pathLength = 0;
    int current = destinationIndex;

    /* Follow parent indices backwards. */
    while (current != -1 && pathLength < graph->nodeCount) {
        if (current < 0 || current >= graph->nodeCount) {
            return 0;
        }

        path[pathLength++] = current;

        if (current == result->sourceIndex) {
            break;
        }

        current = result->parent[current];
    }

    if (pathLength == 0 ||
        path[pathLength - 1] != result->sourceIndex) {
        return 0;
    }

    printf("\n========== SHORTEST PATH ==========\n");
    printf("Source: %s (ID %d)\n",
           graph->nodes[result->sourceIndex].name,
           graph->nodes[result->sourceIndex].id);

    printf("Destination: %s (ID %d)\n",
           graph->nodes[destinationIndex].name,
           destinationId);

    printf("Path: ");

    for (int i = pathLength - 1; i >= 0; i--) {
        printf("%d", graph->nodes[path[i]].id);

        if (i > 0) {
            printf(" -> ");
        }
    }

    printf("\nTotal Routing Cost: %.2f\n",
           result->distance[destinationIndex]);

    printf("Status: Route Found\n");

    return 1;
}
