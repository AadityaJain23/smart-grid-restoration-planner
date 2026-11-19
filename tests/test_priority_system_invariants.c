
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "maxflow.h"
#include "priority_flow.h"

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

static int verifyNetwork(
    const char *filename,
    double expectedTotal,
    double expectedP1,
    double expectedP2,
    double expectedP3
)
{
    static Graph graph;
    static Graph original;
    static MaxFlowResult maxResult;
    static PriorityFlowResult priorityResult;

    initializeGraph(&graph);

    if (!loadNetworkFromFile(&graph, filename)) {
        printf("[FAIL] Network loading: %s\n", filename);
        return 0;
    }

    original = graph;

    /* Run both production solvers. */
    if (!calculateMaximumFlow(&graph, &maxResult) ||
        !calculatePriorityFlow(&graph, &priorityResult)) {
        printf("[FAIL] Solver execution: %s\n", filename);
        return 0;
    }

    /* Invariant 1: Maximum throughput. */
    assert(closeMW(
        maxResult.maxFlow,
        priorityResult.totalDelivered));

    assert(closeMW(
        priorityResult.totalDelivered,
        expectedTotal));

    printf("[PASS] Maximum throughput consistency\n");

    /* Invariant 2: Supply constraint. */
    assert(priorityResult.totalDelivered >= 0.0);

    assert(priorityResult.totalDelivered <=
        priorityResult.totalSupply + 1e-7);

    printf("[PASS] Generator supply constraint\n");

    /* Invariant 3: Consumer demand. */
    for (int i = 0; i < graph.nodeCount; i++) {

        if (graph.nodes[i].type != CONSUMER) {
            continue;
        }

        double delivered =
            priorityResult.deliveredToNode[i];

        assert(delivered >= -1e-7);

        assert(delivered <=
            graph.nodes[i].demand + 1e-7);
    }

    printf("[PASS] Consumer demand constraints\n");

    /* Invariant 4: Priority accounting. */
    double prioritySum = 0.0;

    for (int p = 0; p < PRIORITY_LEVELS; p++) {
        prioritySum +=
            priorityResult.deliveredByPriority[p];
    }

    assert(closeMW(
        prioritySum,
        priorityResult.totalDelivered));

    assert(closeMW(
        priorityResult.deliveredByPriority[0],
        expectedP1));

    assert(closeMW(
        priorityResult.deliveredByPriority[1],
        expectedP2));

    assert(closeMW(
        priorityResult.deliveredByPriority[2],
        expectedP3));

    printf("[PASS] Priority allocation accounting\n");

    /* Invariant 5: Demand accounting. */
    assert(closeMW(
        priorityResult.totalDelivered +
        priorityResult.totalUnserved,
        priorityResult.totalDemand));

    printf("[PASS] Demand accounting\n");

    /* Original graph must remain unchanged. */
    assert(memcmp(
        &graph, &original, sizeof(Graph)) == 0);

    printf("[PASS] Original graph preserved\n");

    return 1;
}

int main(void)
{
    printf("\n=== Original 100 MW Network ===\n");

    if (!verifyNetwork(
            "data/network.txt",
            100.0, 20.0, 10.0, 70.0)) {
        return 1;
    }

    printf("\n=== Limited 60 MW Network ===\n");

    if (!verifyNetwork(
            "data/network_60mw.txt",
            60.0, 20.0, 10.0, 30.0)) {
        return 1;
    }

    printf("\nAll system invariant tests passed!\n");

    return 0;
}
