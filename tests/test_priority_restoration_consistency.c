
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

/*
 * Build a deterministic four-node network.
 *
 * Generator: ID 10, supply 40 MW
 * Substation: ID 20
 * Hospital: ID 30, P1, demand 20 MW
 * Industry: ID 40, P3, demand 20 MW
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
 * Verify both solvers agree on throughput
 * and Priority Flow respects the expected
 * consumer allocation.
 */
static void checkScenario(
    const char *name,
    const Graph *graph,
    double expectedTotal,
    double expectedP1,
    double expectedP3
)
{
    static Graph original;
    static MaxFlowResult maxResult;
    static PriorityFlowResult priorityResult;

    original = *graph;

    memset(&maxResult, 0, sizeof(maxResult));
    memset(&priorityResult, 0, sizeof(priorityResult));

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

    /* Both solvers must preserve the graph. */
    assert(memcmp(
        graph, &original, sizeof(Graph)) == 0);

    printf("[PASS] %s\n", name);
}

int main(void)
{
    static Graph graph;

    buildNetwork(&graph);

    checkScenario(
        "Initial network",
        &graph, 40.0, 20.0, 20.0);

    /* Fail Hospital connection. */
    graph.edges[1].active = 0;

    checkScenario(
        "Hospital line failure",
        &graph, 20.0, 0.0, 20.0);

    /* Restore Hospital connection. */
    graph.edges[1].active = 1;

    checkScenario(
        "Hospital line restoration",
        &graph, 40.0, 20.0, 20.0);

    /* Fail Industry connection. */
    graph.edges[2].active = 0;

    checkScenario(
        "Industry line failure",
        &graph, 20.0, 20.0, 0.0);

    /* Fail Hospital connection too. */
    graph.edges[1].active = 0;

    checkScenario(
        "Both consumer lines failed",
        &graph, 0.0, 0.0, 0.0);

    /* Restore both connections. */
    graph.edges[1].active = 1;
    graph.edges[2].active = 1;

    checkScenario(
        "All connections restored",
        &graph, 40.0, 20.0, 20.0);

    printf("\nAll restoration consistency tests passed!\n");

    return 0;
}
