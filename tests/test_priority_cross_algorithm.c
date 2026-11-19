
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
 * Build a four-node network.
 *
 * Generator ID 10
 * Substation ID 20
 * Hospital ID 30, P1
 * Industry ID 40, P3
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
    graph->nodes[2].demand = 20.0;
    graph->nodes[2].priority = 1;

    graph->nodes[3].id = 40;
    graph->nodes[3].type = CONSUMER;
    graph->nodes[3].demand = 20.0;
    graph->nodes[3].priority = 3;

    /* Generator to Substation */
    graph->edges[0].from = 10;
    graph->edges[0].to = 20;
    graph->edges[0].capacity = 40.0;
    graph->edges[0].active = 1;

    /* Substation to Hospital */
    graph->edges[1].from = 20;
    graph->edges[1].to = 30;
    graph->edges[1].capacity = 20.0;
    graph->edges[1].active = 1;

    /* Substation to Industry */
    graph->edges[2].from = 20;
    graph->edges[2].to = 40;
    graph->edges[2].capacity = 20.0;
    graph->edges[2].active = 1;
}

/*
 * Compare the two public solvers and
 * verify the expected priority allocation.
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

    /* Both algorithms maximize total flow. */
    assert(closeMW(
        maxResult.maxFlow,
        priorityResult.totalDelivered));

    assert(closeMW(
        priorityResult.totalDelivered,
        expectedTotal));

    /* Verify lexicographic allocation. */
    assert(closeMW(
        priorityResult.deliveredByPriority[0],
        expectedP1));

    assert(closeMW(
        priorityResult.deliveredByPriority[1],
        0.0));

    assert(closeMW(
        priorityResult.deliveredByPriority[2],
        expectedP3));

    /* Verify individual consumer delivery. */
    assert(closeMW(
        priorityResult.deliveredToNode[2],
        expectedP1));

    assert(closeMW(
        priorityResult.deliveredToNode[3],
        expectedP3));

    /* Demand accounting. */
    assert(closeMW(
        priorityResult.totalDelivered +
        priorityResult.totalUnserved,
        priorityResult.totalDemand));

    /* Neither solver may modify the graph. */
    assert(memcmp(
        graph, &original, sizeof(Graph)) == 0);

    printf("[PASS] %s\n", name);
}

int main(void)
{
    static Graph graph;

    /* Scenario 1: Full supply */
    buildNetwork(&graph);

    checkScenario(
        "Full supply",
        &graph, 40.0, 20.0, 20.0);

    /* Scenario 2: Limited supply */
    buildNetwork(&graph);
    graph.nodes[0].supply = 20.0;

    checkScenario(
        "Limited supply",
        &graph, 20.0, 20.0, 0.0);

    /* Scenario 3: Severe shortage */
    buildNetwork(&graph);
    graph.nodes[0].supply = 10.0;

    checkScenario(
        "Severe shortage",
        &graph, 10.0, 10.0, 0.0);

    /* Scenario 4: Hospital disconnected */
    buildNetwork(&graph);
    graph.edges[1].active = 0;

    checkScenario(
        "Hospital disconnected",
        &graph, 20.0, 0.0, 20.0);

    /* Scenario 5: Industry disconnected */
    buildNetwork(&graph);
    graph.edges[2].active = 0;

    checkScenario(
        "Industry disconnected",
        &graph, 20.0, 20.0, 0.0);

    /* Scenario 6: Both disconnected */
    buildNetwork(&graph);
    graph.edges[1].active = 0;
    graph.edges[2].active = 0;

    checkScenario(
        "Both consumers disconnected",
        &graph, 0.0, 0.0, 0.0);

    /* Scenario 7: Restored connections */
    buildNetwork(&graph);

    checkScenario(
        "Connections restored",
        &graph, 40.0, 20.0, 20.0);

    printf("\nAll cross-algorithm tests passed!\n");

    return 0;
}
