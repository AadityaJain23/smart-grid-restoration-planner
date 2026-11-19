
#include <stdio.h>
#include "failure_impact.h"

int analyzeMultipleLineFailures(
    const Graph *graph,
    const int failedEdgeIndices[],
    int failureCount,
    FailureImpactResult *result
) {
    if (graph == NULL || result == NULL) {
        return 0;
    }

    if (failureCount < 0 ||
        failureCount > graph->edgeCount ||
        (failureCount > 0 && failedEdgeIndices == NULL)) {
        return 0;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES) {
        return 0;
    }

    /*
     * Validate requested failures before modifying
     * the temporary graph.
     */
    int selected[MAX_EDGES] = {0};

    for (int i = 0; i < failureCount; i++) {

        int edgeIndex = failedEdgeIndices[i];

        if (edgeIndex < 0 ||
            edgeIndex >= graph->edgeCount) {
            return 0;
        }

        if (selected[edgeIndex]) {
            return 0;
        }

        if (graph->edges[edgeIndex].active != 1) {
            return 0;
        }

        selected[edgeIndex] = 1;
    }

    /*
     * Copy the graph so the original remains unchanged.
     */
    Graph simulated = *graph;

    for (int i = 0; i < failureCount; i++) {
        simulated.edges[
            failedEdgeIndices[i]
        ].active = 0;
    }

    /*
     * Build adjacency list for the simulated network.
     */
    AdjList simulatedList;

    if (!buildAdjacencyList(
            &simulated,
            &simulatedList)) {
        return 0;
    }

    /*
     * Calculate connectivity and demand statistics.
     */
    NetworkStats stats;

    if (!calculateNetworkStats(
            &simulated,
            &simulatedList,
            &stats)) {
        return 0;
    }

    FailureImpactResult temporary = {0};

    temporary.failedLineCount = failureCount;

    temporary.disconnectedConsumers =
        stats.disconnectedConsumers;

    temporary.criticalDisconnected =
        stats.criticalDisconnected;

    temporary.disconnectedDemand =
        stats.disconnectedDemand;

    temporary.reachableDemand =
        stats.reachableDemand;

    /*
     * Commit result only after successful analysis.
     */
    *result = temporary;

    return 1;
}

void displayFailureImpactReport(
    const FailureImpactResult *result
) {
    if (result == NULL) {
        printf("Error: Invalid failure impact result.\n");
        return;
    }

    printf("\n===== MULTIPLE-LINE FAILURE IMPACT =====\n");

    printf("Newly Failed Lines       : %d\n",
           result->failedLineCount);

    printf("Disconnected Consumers   : %d\n",
           result->disconnectedConsumers);

    printf("Critical Disconnected    : %d\n",
           result->criticalDisconnected);

    printf("Disconnected Demand      : %.2f MW\n",
           result->disconnectedDemand);

    printf("Reachable Demand         : %.2f MW\n",
           result->reachableDemand);

    printf("\nOriginal network remains unchanged.\n");
}
