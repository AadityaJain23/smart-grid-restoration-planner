
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
 * Valid baseline:
 *
 * Generator 10: 20 MW
 * Hospital 20: 20 MW, Priority 1
 *
 * Two parallel lines:
 * 10 -> 20, capacity 12 MW
 * 10 -> 20, capacity 8 MW
 */
static void buildParallelGraph(Graph *graph)
{
    memset(graph, 0, sizeof(*graph));

    graph->nodeCount = 2;
    graph->edgeCount = 2;

    graph->nodes[0].id = 10;
    graph->nodes[0].type = GENERATOR;
    graph->nodes[0].supply = 20.0;

    graph->nodes[1].id = 20;
    graph->nodes[1].type = CONSUMER;
    graph->nodes[1].demand = 20.0;
    graph->nodes[1].priority = 1;

    graph->edges[0].from = 10;
    graph->edges[0].to = 20;
    graph->edges[0].capacity = 12.0;
    graph->edges[0].active = 1;

    graph->edges[1].from = 10;
    graph->edges[1].to = 20;
    graph->edges[1].capacity = 8.0;
    graph->edges[1].active = 1;
}

static void expectRejected(
    const Graph *graph
)
{
    PriorityFlowResult result = {0};

    result.totalDelivered = 123.0;
    result.totalUnserved = 456.0;

    PriorityFlowResult before = result;

    assert(calculatePriorityFlow(
        graph, &result) == 0);

    assert(memcmp(
        &result, &before, sizeof(result)) == 0);
}

int main(void)
{
    static Graph graph;
    PriorityFlowResult result = {0};

    /*
     * Test 1: Valid parallel lines.
     */
    buildParallelGraph(&graph);

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    assert(closeMW(result.totalDelivered, 20.0));
    assert(closeMW(result.deliveredToNode[1], 20.0));

    printf("[PASS] Parallel lines deliver 20 MW\n");

    /*
     * Test 2: Reverse one line's
     * endpoint orientation.
     */
    buildParallelGraph(&graph);

    graph.edges[1].from = 20;
    graph.edges[1].to = 10;

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    assert(closeMW(result.totalDelivered, 20.0));

    printf("[PASS] Reversed parallel line accepted\n");

    /*
     * Test 3: Disable 12 MW line.
     */
    buildParallelGraph(&graph);

    graph.edges[0].active = 0;

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    assert(closeMW(result.totalDelivered, 8.0));
    assert(closeMW(result.totalUnserved, 12.0));

    printf("[PASS] Failed parallel line handled\n");

    /*
     * Test 4: Duplicate logical node IDs.
     */
    buildParallelGraph(&graph);

    graph.nodes[1].id = 10;

    expectRejected(&graph);

    printf("[PASS] Duplicate node IDs rejected\n");

    /*
     * Test 5: Self-loop.
     */
    buildParallelGraph(&graph);

    graph.edges[0].from = 10;
    graph.edges[0].to = 10;

    expectRejected(&graph);

    printf("[PASS] Self-loop rejected\n");

    /*
     * Test 6: Unknown endpoint.
     */
    buildParallelGraph(&graph);

    graph.edges[1].to = 999;

    expectRejected(&graph);

    printf("[PASS] Unknown endpoint rejected\n");

    printf("\nAll graph-structure tests passed!\n");

    return 0;
}
