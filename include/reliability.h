
#ifndef RELIABILITY_H
#define RELIABILITY_H

#include "graph.h"

/*
 * Week 6: Smart Grid Reliability Analysis
 *
 * Bridge detection:
 * Identifies critical transmission lines.
 *
 * Articulation-point detection:
 * Identifies critical network nodes.
 */

typedef struct {

    /* Original graph edge indices */
    int bridgeEdgeIndices[MAX_EDGES];
    int bridgeCount;

    /* Original graph node indices */
    int articulationNodeIndices[MAX_NODES];
    int articulationCount;

} ReliabilityResult;

/*
 * Find critical active transmission lines.
 * Returns 1 on success, 0 on failure.
 */
int findCriticalBridges(
    const Graph *graph,
    const AdjList *list,
    ReliabilityResult *result
);

/*
 * Find critical network nodes.
 * Returns 1 on success, 0 on failure.
 */
int findArticulationPoints(
    const Graph *graph,
    const AdjList *list,
    ReliabilityResult *result
);

#endif
