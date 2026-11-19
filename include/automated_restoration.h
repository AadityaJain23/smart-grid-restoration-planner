
#ifndef AUTOMATED_RESTORATION_H
#define AUTOMATED_RESTORATION_H

#include "graph.h"
#include "priority_flow.h"

typedef struct {

    /* Priority-aware allocation before failures */
    PriorityFlowResult beforeFailure;

    /* Priority-aware allocation after failures */
    PriorityFlowResult afterFailure;

    /* Number of newly simulated line failures */
    int failedLineCount;

} AutomatedRestorationResult;

/*
 * Analyze power allocation before and after
 * simulated transmission-line failures.
 *
 * The original graph must remain unchanged.
 *
 * Returns 1 on success, 0 on failure.
 */
int analyzeAutomatedRestoration(
    const Graph *graph,
    const int failedEdgeIndices[],
    int failureCount,
    AutomatedRestorationResult *result
);

/*
 * Display before-and-after priority-aware
 * power allocation comparison.
 */
void displayAutomatedRestorationReport(
    const Graph *graph,
    const AutomatedRestorationResult *result
);

#endif
