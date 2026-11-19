
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
 * Build a network with:
 * - Generator: 40 MW
 * - Hospital P1: 20 MW
 * - Industry P3: 20 MW
 * - One isolated substation
 */
static void buildNetwork(Graph *graph)
{
    memset(graph, 0, sizeof(*graph));

    graph->nodeCount = 5;
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

    graph->nodes[4].id = 50;
    graph->nodes[4].type = SUBSTATION;

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

    /* Substation -> Industry (failed) */
    graph->edges[2].from = 20;
    graph->edges[2].to = 40;
    graph->edges[2].capacity = 20.0;
    graph->edges[2].active = 0;
}

static void checkResult(
    const Graph *graph,
    double expectedTotal,
    double expectedP1,
    double expectedP3,
    double expectedUnserved
)
{
    PriorityFlowResult result = {0};

    assert(calculatePriorityFlow(
        graph, &result) == 1);

    assert(closeMW(
        result.totalDelivered, expectedTotal));

    assert(closeMW(
        result.deliveredByPriority[0], expectedP1));

    assert(closeMW(
        result.deliveredByPriority[2], expectedP3));

    assert(closeMW(
        result.totalUnserved, expectedUnserved));
}

int main(void)
{
    static Graph graph;

    /*
     * Scenario 1:
     * Industry disconnected.
     */
    buildNetwork(&graph);

    checkResult(
        &graph, 20.0, 20.0, 0.0, 20.0);

    printf("[PASS] Disconnected consumer\n");

    /*
     * Scenario 2:
     * Both consumer lines inactive.
     */
    buildNetwork(&graph);
    graph.edges[1].active = 0;

    checkResult(
        &graph, 0.0, 0.0, 0.0, 40.0);

    printf("[PASS] All consumers disconnected\n");

    /*
     * Scenario 3:
     * Isolated substation must not
     * affect the connected network.
     */
    buildNetwork(&graph);

    checkResult(
        &graph, 20.0, 20.0, 0.0, 20.0);

    printf("[PASS] Isolated substation handled\n");

    /*
     * Scenario 4:
     * Restore Industry connection.
     */
    buildNetwork(&graph);
    graph.edges[2].active = 1;

    checkResult(
        &graph, 40.0, 20.0, 20.0, 0.0);

    printf("[PASS] Restored transmission line\n");

    printf("\nAll disconnected-network tests passed!\n");

    return 0;
}
