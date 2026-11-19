
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "maxflow.h"
#include "priority_flow.h"

#define REPEAT_COUNT 100

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

/*
 * Build a deterministic network.
 *
 * Generator 10: 40 MW
 * Substation 20
 * Hospital 30: P1, 20 MW
 * Industry 40: P3, 20 MW
 */
static void buildNetwork(Graph *graph)
{
    memset(graph, 0, sizeof(*graph));

    graph->nodeCount = 4;
    graph->edgeCount = 3;

    graph->nodes[0].id = 10;
    graph->nodes[0].type = GENERATOR;
    graph->nodes[0].supply = 40.0;

    graph->nodes[1].id = 20;
    graph->nodes[1].type = SUBSTATION;

    graph->nodes[2].id = 30;
    graph->nodes[2].type = CONSUMER;
    graph->nodes[2].priority = 1;
    graph->nodes[2].demand = 20.0;

    graph->nodes[3].id = 40;
    graph->nodes[3].type = CONSUMER;
    graph->nodes[3].priority = 3;
    graph->nodes[3].demand = 20.0;

    graph->edges[0].from = 10;
    graph->edges[0].to = 20;
    graph->edges[0].capacity = 40.0;
    graph->edges[0].active = 1;

    graph->edges[1].from = 20;
    graph->edges[1].to = 30;
    graph->edges[1].capacity = 20.0;
    graph->edges[1].active = 1;

    graph->edges[2].from = 20;
    graph->edges[2].to = 40;
    graph->edges[2].capacity = 20.0;
    graph->edges[2].active = 1;
}

/*
 * Execute both solvers repeatedly.
 *
 * Verify:
 * - Correct total throughput
 * - Correct priority allocation
 * - Original graph preservation
 * - Consistent repeated results
 */
static void verifyRepeatedCalculations(
    const char *scenario,
    const Graph *graph,
    double expectedTotal,
    double expectedP1,
    double expectedP3
)
{
    static Graph snapshot;
    static MaxFlowResult maxResult;
    static PriorityFlowResult priorityResult;

    snapshot = *graph;

    for (int run = 0; run < REPEAT_COUNT; run++) {

        memset(&maxResult, 0, sizeof(maxResult));
        memset(&priorityResult, 0,
               sizeof(priorityResult));

        assert(calculateMaximumFlow(
            graph, &maxResult) == 1);

        assert(calculatePriorityFlow(
            graph, &priorityResult) == 1);

        assert(closeMW(
            maxResult.maxFlow,
            priorityResult.totalDelivered));

        assert(closeMW(
            priorityResult.totalDelivered,
            expectedTotal));

        assert(closeMW(
            priorityResult.deliveredByPriority[0],
            expectedP1));

        assert(closeMW(
            priorityResult.deliveredByPriority[1],
            0.0));

        assert(closeMW(
            priorityResult.deliveredByPriority[2],
            expectedP3));

        assert(closeMW(
            priorityResult.deliveredToNode[2],
            expectedP1));

        assert(closeMW(
            priorityResult.deliveredToNode[3],
            expectedP3));

        assert(closeMW(
            priorityResult.totalDelivered +
            priorityResult.totalUnserved,
            priorityResult.totalDemand));

        /*
         * Verify neither algorithm changed
         * the original graph.
         */
        assert(memcmp(
            graph, &snapshot,
            sizeof(Graph)) == 0);
    }

    printf("[PASS] %s (%d repetitions)\n",
           scenario, REPEAT_COUNT);
}

int main(void)
{
    static Graph graph;

    buildNetwork(&graph);

    /* Test 1: Initial network */
    verifyRepeatedCalculations(
        "Initial network",
        &graph, 40.0, 20.0, 20.0);

    /* Test 2: Hospital line failure */
    graph.edges[1].active = 0;

    verifyRepeatedCalculations(
        "Hospital line failed",
        &graph, 20.0, 0.0, 20.0);

    /* Verify failure state remains. */
    assert(graph.edges[1].active == 0);

    printf("[PASS] Failed-line state preserved\n");

    /* Test 3: Restore Hospital line */
    graph.edges[1].active = 1;

    verifyRepeatedCalculations(
        "Hospital line restored",
        &graph, 40.0, 20.0, 20.0);

    /* Verify restored state remains. */
    assert(graph.edges[1].active == 1);

    printf("[PASS] Restored-line state preserved\n");

    printf("\nAll network-state preservation tests passed!\n");

    return 0;
}
