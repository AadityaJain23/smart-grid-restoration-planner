
#ifndef MST_H
#define MST_H

#include "graph.h"

/*
 * Result of Prim's Minimum Spanning Tree algorithm.
 * Array positions are graph node indices, not node IDs.
 */
typedef struct {
    double key[MAX_NODES];
    int parent[MAX_NODES];
    int parentEdge[MAX_NODES];
    int inMST[MAX_NODES];

    int startIndex;
    int selectedEdges;
    double totalCost;
    int isConnected;
} PrimResult;

/*
 * Compute Prim's MST using active transmission lines.
 *
 * Returns 1 if the algorithm executes successfully.
 * Returns 0 for invalid input or calculation failure.
 *
 * A disconnected network is reported through
 * result->isConnected == 0.
 */
int primMinimumSpanningTree(
    const Graph *graph,
    const AdjList *list,
    int startId,
    PrimResult *result
);

/* Display a complete MST or report disconnection. */
void displayPrimMST(
    const Graph *graph,
    const PrimResult *result
);


/*
 * Result of Kruskal's Minimum Spanning Tree algorithm.
 *
 * selectedEdgeIndices[] stores original Graph.edges[]
 * array indices, not node IDs.
 */
typedef struct {
    int selectedEdgeIndices[MAX_NODES - 1];
    int selectedEdges;
    double totalCost;
    int isConnected;
} KruskalResult;

/*
 * Compute a minimum spanning tree using Kruskal.
 *
 * Returns:
 * 1 = calculation completed
 * 0 = invalid input or calculation failure
 *
 * isConnected indicates whether a complete MST exists.
 */
int kruskalMinimumSpanningTree(
    const Graph *graph,
    const AdjList *list,
    KruskalResult *result
);

/* Display Kruskal's selected transmission lines. */
void displayKruskalMST(
    const Graph *graph,
    const KruskalResult *result
);


#endif
