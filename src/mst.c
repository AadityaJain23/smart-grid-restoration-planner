
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "mst.h"
#include "sorting.h"
#include "dsu.h"

/* Find a node's array index using its logical ID. */
static int findMSTNodeIndex(
    const Graph *graph,
    int nodeId
) {
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

/* Find the unselected vertex with minimum key. */
static int findMinimumKeyNode(
    const PrimResult *result,
    int nodeCount
) {
    int minIndex = -1;
    double minKey = INFINITY;

    for (int i = 0; i < nodeCount; i++) {
        if (!result->inMST[i] &&
            result->key[i] < minKey) {

            minKey = result->key[i];
            minIndex = i;
        }
    }

    return minIndex;
}

/* Compute a minimum spanning tree using Prim. */
int primMinimumSpanningTree(
    const Graph *graph,
    const AdjList *list,
    int startId,
    PrimResult *result
) {
    if (graph == NULL ||
        list == NULL ||
        result == NULL) {
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

    int startIndex = findMSTNodeIndex(
        graph, startId
    );

    if (startIndex == -1) {
        return 0;
    }

    /* Validate all stored edge costs. */
    for (int i = 0; i < graph->edgeCount; i++) {
        if (!isfinite(graph->edges[i].cost) ||
            graph->edges[i].cost < 0.0) {
            return 0;
        }
    }

    /* Initialize result arrays. */
    for (int i = 0; i < MAX_NODES; i++) {
        result->key[i] = INFINITY;
        result->parent[i] = -1;
        result->parentEdge[i] = -1;
        result->inMST[i] = 0;
    }

    result->startIndex = startIndex;
    result->selectedEdges = 0;
    result->totalCost = 0.0;
    result->isConnected = 0;

    result->key[startIndex] = 0.0;

    int includedCount = 0;

    /* Main Prim loop. */
    for (int count = 0;
         count < graph->nodeCount;
         count++) {

        int u = findMinimumKeyNode(
            result, graph->nodeCount
        );

        /* No more reachable vertices. */
        if (u == -1) {
            break;
        }

        result->inMST[u] = 1;
        includedCount++;

        /* Add the selected connecting edge. */
        if (u != startIndex) {
            double weight = result->key[u];

            if (result->parentEdge[u] == -1 ||
                result->parent[u] == -1) {
                return 0;
            }

            if (result->totalCost > DBL_MAX - weight) {
                return 0;
            }

            result->totalCost += weight;
            result->selectedEdges++;
        }

        /* Traverse active neighboring edges. */
        int current = list->head[u];

        while (current != -1) {
            AdjEntry entry = list->entries[current];

            int v = entry.neighborIndex;
            int edgeIndex = entry.edgeIndex;

            const Edge *edge =
                &graph->edges[edgeIndex];

            double weight = edge->cost;

            /* Prim's greedy update. */
            if (!result->inMST[v] &&
                edge->active == 1 &&
                weight < result->key[v]) {

                result->key[v] = weight;
                result->parent[v] = u;
                result->parentEdge[v] = edgeIndex;
            }

            current = entry.next;
        }
    }

    result->isConnected =
        (includedCount == graph->nodeCount);

    return 1;
}

/* Display the selected minimum spanning tree. */
void displayPrimMST(
    const Graph *graph,
    const PrimResult *result
) {
    if (graph == NULL || result == NULL ||
        graph->nodeCount <= 0 ||
        graph->nodeCount > MAX_NODES ||
        result->startIndex < 0 ||
        result->startIndex >= graph->nodeCount) {

        printf("Error: Invalid Prim result.\n");
        return;
    }

    printf("\n========== PRIM MST ==========\n");

    if (!result->isConnected) {
        printf("Complete MST not possible.\n");
        printf("Reason: Active network is disconnected.\n");
        return;
    }

    printf("%-8s %-8s %-12s\n",
           "From", "To", "Cost");

    printf("------------------------------\n");

    for (int i = 0; i < graph->nodeCount; i++) {
        if (i == result->startIndex) {
            continue;
        }

        int parent = result->parent[i];
        int edgeIndex = result->parentEdge[i];

        if (parent < 0 ||
            parent >= graph->nodeCount ||
            edgeIndex < 0 ||
            edgeIndex >= graph->edgeCount) {
            printf("Error: Invalid MST edge.\n");
            return;
        }

        printf("%-8d %-8d %-12.2f\n",
               graph->nodes[parent].id,
               graph->nodes[i].id,
               graph->edges[edgeIndex].cost);
    }

    printf("------------------------------\n");

    printf("Selected Edges: %d\n",
           result->selectedEdges);

    printf("Total MST Cost: %.2f\n",
           result->totalCost);

    printf("Status: Complete MST Found\n");
}


/* Compute a minimum spanning tree using Kruskal. */
int kruskalMinimumSpanningTree(
    const Graph *graph,
    const AdjList *list,
    KruskalResult *result
) {
    if (graph == NULL ||
        list == NULL ||
        result == NULL) {
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

    /* Validate every stored transmission-line cost. */
    for (int i = 0; i < graph->edgeCount; i++) {
        if (!isfinite(graph->edges[i].cost) ||
            graph->edges[i].cost < 0.0) {
            return 0;
        }
    }

    /* Prepare the output structure. */
    KruskalResult candidate = {0};

    /* Initialize DSU for all graph vertices. */
    DSU dsu;

    if (!dsuInitialize(&dsu, graph->nodeCount)) {
        return 0;
    }

    /* Extract original edge indices. */
    int edgeIndices[MAX_EDGES];

    int edgeCount = extractEdgeIndices(
        graph, edgeIndices
    );

    if (edgeCount < 0 ||
        edgeCount > MAX_EDGES) {
        return 0;
    }

    /* Reuse Week 2 Quick Sort. */
    if (edgeCount > 1) {
        quickSortEdges(
            graph, edgeIndices, 0, edgeCount - 1
        );
    }

    /* Process edges in ascending cost order. */
    for (int i = 0; i < edgeCount; i++) {
        int edgeIndex = edgeIndices[i];

        if (edgeIndex < 0 ||
            edgeIndex >= graph->edgeCount) {
            return 0;
        }

        const Edge *edge = &graph->edges[edgeIndex];

        /* Failed transmission lines cannot be selected. */
        if (edge->active != 1) {
            continue;
        }

        int u = findMSTNodeIndex(graph, edge->from);
        int v = findMSTNodeIndex(graph, edge->to);

        if (u == -1 || v == -1 || u == v) {
            return 0;
        }

        int rootU = dsuFind(&dsu, u);
        int rootV = dsuFind(&dsu, v);

        if (rootU == -1 || rootV == -1) {
            return 0;
        }

        /* Same component means this edge creates a cycle. */
        if (rootU == rootV) {
            continue;
        }

        /* Prevent overflow before adding edge cost. */
        if (candidate.totalCost > DBL_MAX - edge->cost) {
            return 0;
        }

        int merged = dsuUnion(&dsu, u, v);

        if (merged != 1) {
            return 0;
        }

        /* Select the edge without modifying Graph.edges[]. */
        candidate.selectedEdgeIndices[
            candidate.selectedEdges
        ] = edgeIndex;

        candidate.selectedEdges++;
        candidate.totalCost += edge->cost;

        /* A complete MST has V - 1 edges. */
        if (candidate.selectedEdges ==
            graph->nodeCount - 1) {
            break;
        }
    }

    candidate.isConnected =
        (candidate.selectedEdges ==
         graph->nodeCount - 1);

    *result = candidate;

    return 1;
}


/* Display Kruskal's minimum spanning tree. */
void displayKruskalMST(
    const Graph *graph,
    const KruskalResult *result
) {
    if (graph == NULL || result == NULL ||
        graph->nodeCount <= 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES ||
        result->selectedEdges < 0 ||
        result->selectedEdges >= MAX_NODES) {

        printf("Error: Invalid Kruskal result.\n");
        return;
    }

    printf("\n========== KRUSKAL MST ==========\n");

    if (!result->isConnected) {
        printf("Complete MST not possible.\n");
        printf("Reason: Active network is disconnected.\n");
        printf("Selected Forest Edges: %d\n",
               result->selectedEdges);
        return;
    }

    printf("%-8s %-8s %-12s\n",
           "From", "To", "Cost");

    printf("------------------------------\n");

    for (int i = 0; i < result->selectedEdges; i++) {
        int edgeIndex = result->selectedEdgeIndices[i];

        if (edgeIndex < 0 ||
            edgeIndex >= graph->edgeCount) {
            printf("Error: Invalid selected edge.\n");
            return;
        }

        const Edge *edge = &graph->edges[edgeIndex];

        printf("%-8d %-8d %-12.2f\n",
               edge->from,
               edge->to,
               edge->cost);
    }

    printf("------------------------------\n");

    printf("Selected Edges: %d\n",
           result->selectedEdges);

    printf("Total MST Cost: %.2f\n",
           result->totalCost);

    printf("Status: Complete MST Found\n");
}

