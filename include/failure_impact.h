
#ifndef FAILURE_IMPACT_H
#define FAILURE_IMPACT_H

#include "graph.h"

typedef struct {
    int failedLineCount;

    int disconnectedConsumers;
    int criticalDisconnected;

    double disconnectedDemand;
    double reachableDemand;

} FailureImpactResult;

/*
 * Simulate simultaneous line failures without
 * modifying the original network.
 *
 * failedEdgeIndices contains original edge-array indices.
 *
 * Returns 1 on success, 0 on failure.
 */
int analyzeMultipleLineFailures(
    const Graph *graph,
    const int failedEdgeIndices[],
    int failureCount,
    FailureImpactResult *result
);

void displayFailureImpactReport(
    const FailureImpactResult *result
);

#endif
