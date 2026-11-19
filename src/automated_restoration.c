
#include <stdio.h>
#include "automated_restoration.h"

/*
 * Week 7 - Automated Priority-Aware Restoration
 *
 * Compares priority-aware allocation before
 * and after simulated transmission failures.
 *
 * The original Graph remains unchanged.
 */

int analyzeAutomatedRestoration(
    const Graph *graph,
    const int failedEdgeIndices[],
    int failureCount,
    AutomatedRestorationResult *result
) {
    if (graph == NULL || result == NULL) {
        return 0;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES) {
        return 0;
    }

    if (failureCount < 0 ||
        failureCount > graph->edgeCount ||
        (failureCount > 0 && failedEdgeIndices == NULL)) {
        return 0;
    }

    /*
     * Validate all requested failures first.
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
     * Create temporary graph.
     */
    Graph simulated = *graph;

    /*
     * Apply failures only to temporary graph.
     */
    for (int i = 0; i < failureCount; i++) {
        simulated.edges[
            failedEdgeIndices[i]
        ].active = 0;
    }

    /*
     * Validate simulated connectivity.
     */
    AdjList simulatedList;

    if (!buildAdjacencyList(
            &simulated,
            &simulatedList) ||
        !validateAdjacencyList(
            &simulated,
            &simulatedList)) {
        return 0;
    }

    /*
     * Calculate both allocation scenarios.
     */
    AutomatedRestorationResult temporary = {0};

    if (!calculatePriorityFlow(
            graph,
            &temporary.beforeFailure)) {
        return 0;
    }

    if (!calculatePriorityFlow(
            &simulated,
            &temporary.afterFailure)) {
        return 0;
    }

    temporary.failedLineCount = failureCount;

    /*
     * Commit only after successful analysis.
     */
    *result = temporary;

    return 1;
}


/*
 * Display automated restoration analysis.
 *
 * This report compares the current network
 * against a simulated failure scenario.
 */
void displayAutomatedRestorationReport(
    const Graph *graph,
    const AutomatedRestorationResult *result
) {
    if (graph == NULL || result == NULL) {
        printf("Error: Invalid restoration report.\n");
        return;
    }

    const PriorityFlowResult *before =
        &result->beforeFailure;

    const PriorityFlowResult *after =
        &result->afterFailure;

    printf("\n========================================\n");
    printf("   AUTOMATED RESTORATION ANALYSIS\n");
    printf("========================================\n");

    printf("Newly Failed Lines : %d\n",
           result->failedLineCount);

    printf("\n--- POWER ALLOCATION COMPARISON ---\n");

    printf("%-24s %12s %12s\n",
           "Metric", "Before", "After");

    printf("--------------------------------"
           "--------------------\n");

    printf("%-24s %12.2f %12.2f\n",
           "Delivered Power (MW)",
           before->totalDelivered,
           after->totalDelivered);

    printf("%-24s %12.2f %12.2f\n",
           "Unserved Demand (MW)",
           before->totalUnserved,
           after->totalUnserved);

    printf("%-24s %12.2f %12.2f\n",
           "Priority 1 (MW)",
           before->deliveredByPriority[0],
           after->deliveredByPriority[0]);

    printf("%-24s %12.2f %12.2f\n",
           "Priority 2 (MW)",
           before->deliveredByPriority[1],
           after->deliveredByPriority[1]);

    printf("%-24s %12.2f %12.2f\n",
           "Priority 3 (MW)",
           before->deliveredByPriority[2],
           after->deliveredByPriority[2]);

    printf("\n--- FAILURE IMPACT ---\n");

    double powerLoss =
        before->totalDelivered -
        after->totalDelivered;

    printf("Change in Delivered Power : %.2f MW\n",
           after->totalDelivered -
           before->totalDelivered);

    if (powerLoss > 1e-7) {
        printf("Lost Deliverable Power    : %.2f MW\n",
               powerLoss);
    } else {
        printf("No reduction in total delivered power.\n");
    }

    printf("\n--- CONSUMER ALLOCATION CHANGES ---\n");

    int changedConsumers = 0;

    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type != CONSUMER) {
            continue;
        }

        double beforeMW =
            before->deliveredToNode[i];

        double afterMW =
            after->deliveredToNode[i];

        double difference = afterMW - beforeMW;

        if (difference > 1e-7 ||
            difference < -1e-7) {

            printf(
                "ID %d | %-20s | "
                "Before: %.2f MW | "
                "After: %.2f MW | "
                "Change: %+.2f MW\n",
                node->id,
                node->name,
                beforeMW,
                afterMW,
                difference
            );

            changedConsumers++;
        }
    }

    if (changedConsumers == 0) {
        printf("No consumer allocation changes.\n");
    }

    printf("\nConsumers With Changes : %d\n",
           changedConsumers);

    printf("\nOriginal network remains unchanged.\n");
    printf("========================================\n");
}

