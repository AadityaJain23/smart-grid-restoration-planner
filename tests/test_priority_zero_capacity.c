
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "priority_flow.h"

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

/*
 * Construct the baseline graph.
 */
static void buildNetwork(Graph *graph)
{
    memset(graph, 0, sizeof(*graph));

    graph->nodeCount = 4;
    graph->edgeCount = 3;

    /* Generator */
    graph->nodes[0].id = 10;
    graph->nodes[0].type = GENERATOR;
    graph->nodes[0].supply = 40.0;

    /* Substation */
    graph->nodes[1].id = 20;
    graph->nodes[1].type = SUBSTATION;

    /* Hospital */
    graph->nodes[2].id = 30;
    graph->nodes[2].type = CONSUMER;
    graph->nodes[2].demand = 20.0;
    graph->nodes[2].priority = 1;

    /* Industry */
    graph->nodes[3].id = 40;
    graph->nodes[3].type = CONSUMER;
    graph->nodes[3].demand = 20.0;
    graph->nodes[3].priority = 3;

    /* Generator -> Substation */
    graph->edges[0].from = 10;
    graph->edges[0].to = 20;
    graph->edges[0].capacity = 40.0;
    graph->edges[0].active = 1;

    /* Substation -> Hospital */
    graph->edges[1].from = 20;
    graph->edges[1].to = 30;
    graph->edges[1].capacity = 20.0;
    graph->edges[1].active = 1;

    /* Substation -> Industry */
    graph->edges[2].from = 20;
    graph->edges[2].to = 40;
    graph->edges[2].capacity = 0.0;
    graph->edges[2].active = 1;
}

/*
 * Verify the public solver result.
 */
static void checkAllocation(
    const Graph *graph,
    double expectedHospital,
    double expectedIndustry,
    double expectedTotal
)
{
    PriorityFlowResult result = {0};

    assert(calculatePriorityFlow(
        graph, &result) == 1);

    assert(closeMW(
        result.deliveredToNode[2],
        expectedHospital));

    assert(closeMW(
        result.deliveredToNode[3],
        expectedIndustry));

    assert(closeMW(
        result.totalDelivered,
        expectedTotal));

    assert(closeMW(
        result.totalUnserved,
        40.0 - expectedTotal));
}

int main(void)
{
    static Graph graph;

    /*
     * Scenario 1:
     * Active line with zero capacity.
     */
    buildNetwork(&graph);

    checkAllocation(
        &graph, 20.0, 0.0, 20.0);

    printf("[PASS] Zero-capacity line carries no flow\n");

    /*
     * Scenario 2:
     * Industry line is inactive.
     */
    buildNetwork(&graph);
    graph.edges[2].active = 0;

    checkAllocation(
        &graph, 20.0, 0.0, 20.0);

    printf("[PASS] Inactive line excluded\n");

    /*
     * Scenario 3:
     * Restore Industry line capacity.
     */
    buildNetwork(&graph);
    graph.edges[2].capacity = 20.0;

    checkAllocation(
        &graph, 20.0, 20.0, 40.0);

    printf("[PASS] Restored line delivers power\n");

    /*
     * Scenario 4:
     * Generator connection has zero capacity.
     */
    buildNetwork(&graph);
    graph.edges[0].capacity = 0.0;

    checkAllocation(
        &graph, 0.0, 0.0, 0.0);

    printf("[PASS] Zero-capacity generator route\n");

    printf("\nAll zero-capacity tests passed!\n");

    return 0;
}
