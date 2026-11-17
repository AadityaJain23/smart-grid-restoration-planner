
#ifndef RESTORATION_H
#define RESTORATION_H

#include "graph.h"

/*
 * Simplified capacity-aware restoration results.
 *
 * All node-indexed arrays use original graph
 * array positions, not logical node IDs.
 */
typedef struct {
    double allocated[MAX_NODES];
    double remainingDemand[MAX_NODES];
    double remainingSupply[MAX_NODES];

    double residualCapacity[MAX_EDGES];
    double edgeAllocated[MAX_EDGES];

    double totalDemand;
    double totalAllocated;
    double totalUnserved;

    int fullyServedConsumers;
    int partiallyServedConsumers;
    int unservedConsumers;
} RestorationResult;

/*
 * Allocate power using consumer priorities,
 * available generator supply and residual
 * transmission-line capacities.
 *
 * Returns:
 * 1 = calculation completed successfully
 * 0 = invalid input or calculation failure
 */
int planPowerRestoration(
    const Graph *graph,
    const AdjList *list,
    RestorationResult *result
);

/* Display the simulated allocation report. */
void displayRestorationReport(
    const Graph *graph,
    const RestorationResult *result
);

#endif
