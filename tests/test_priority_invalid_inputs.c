
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "priority_flow.h"

/*
 * Build a small valid network.
 */
static void buildValidGraph(Graph *graph)
{
    memset(graph, 0, sizeof(*graph));

    graph->nodeCount = 2;
    graph->edgeCount = 1;

    graph->nodes[0].id = 10;
    graph->nodes[0].type = GENERATOR;
    graph->nodes[0].supply = 20.0;

    graph->nodes[1].id = 20;
    graph->nodes[1].type = CONSUMER;
    graph->nodes[1].demand = 20.0;
    graph->nodes[1].priority = 1;

    graph->edges[0].from = 10;
    graph->edges[0].to = 20;
    graph->edges[0].capacity = 20.0;
    graph->edges[0].active = 1;
}

/*
 * Verify invalid input is rejected and
 * the caller's result is preserved.
 */
static void expectRejected(
    const Graph *graph,
    const char *label
)
{
    PriorityFlowResult result = {0};

    result.totalDelivered = 123.0;
    result.totalUnserved = 456.0;

    PriorityFlowResult before = result;

    assert(calculatePriorityFlow(
        graph, &result) == 0);

    assert(memcmp(
        &result,
        &before,
        sizeof(result)) == 0);

    printf("[PASS] %s\n", label);
}

int main(void)
{
    static Graph graph;

    buildValidGraph(&graph);

    PriorityFlowResult valid = {0};

    assert(calculatePriorityFlow(
        &graph, &valid) == 1);

    printf("[PASS] Valid baseline network\n");

    /*
     * Negative generator supply.
     */
    buildValidGraph(&graph);
    graph.nodes[0].supply = -5.0;

    expectRejected(
        &graph, "Negative supply rejected");

    /*
     * Negative consumer demand.
     */
    buildValidGraph(&graph);
    graph.nodes[1].demand = -10.0;

    expectRejected(
        &graph, "Negative demand rejected");

    /*
     * Negative transmission capacity.
     */
    buildValidGraph(&graph);
    graph.edges[0].capacity = -1.0;

    expectRejected(
        &graph, "Negative capacity rejected");

    /*
     * Invalid consumer priority.
     */
    buildValidGraph(&graph);
    graph.nodes[1].priority = 5;

    expectRejected(
        &graph, "Invalid priority rejected");

    /*
     * Invalid transmission endpoint.
     */
    buildValidGraph(&graph);
    graph.edges[0].to = 999;

    expectRejected(
        &graph, "Invalid endpoint rejected");

    /*
     * Nonfinite generator supply.
     */
    buildValidGraph(&graph);
    graph.nodes[0].supply = NAN;

    expectRejected(
        &graph, "NaN supply rejected");

    /*
     * Infinite transmission capacity.
     */
    buildValidGraph(&graph);
    graph.edges[0].capacity = INFINITY;

    expectRejected(
        &graph, "Infinite capacity rejected");

    /*
     * Null graph pointer.
     */
    expectRejected(
        NULL, "Null graph rejected");

    /*
     * Null result pointer.
     */
    buildValidGraph(&graph);

    assert(calculatePriorityFlow(
        &graph, NULL) == 0);

    printf("[PASS] Null result rejected\n");

    printf("\nAll invalid-input tests passed!\n");

    return 0;
}
