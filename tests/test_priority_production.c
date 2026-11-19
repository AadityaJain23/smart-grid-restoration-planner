
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

int main(void)
{
    static Graph graph;
    static PriorityFlowResult result;

    memset(&graph, 0, sizeof(graph));
    memset(&result, 0, sizeof(result));

    graph.nodeCount = 3;
    graph.edgeCount = 2;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 30.0;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = 20.0;
    graph.nodes[1].priority = 1;

    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 10.0;
    graph.nodes[2].priority = 2;

    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 20.0;
    graph.edges[0].active = 1;

    graph.edges[1].from = 10;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 10.0;
    graph.edges[1].active = 1;

    /*
     * Preserve the original graph.
     */
    static Graph original;
    original = graph;

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    printf("[PASS] Production API completed\n");

    assert(closeMW(result.totalDelivered, 30.0));
    assert(closeMW(result.totalUnserved, 0.0));

    printf("[PASS] Total delivery and demand accounting\n");

    assert(closeMW(
        result.deliveredByPriority[0], 20.0));

    assert(closeMW(
        result.deliveredByPriority[1], 10.0));

    printf("[PASS] Priority allocations\n");

    /*
     * The solver must not modify the graph.
     */
    assert(memcmp(
        &graph, &original, sizeof(Graph)) == 0);

    printf("[PASS] Original graph preserved\n");

    /*
     * Invalid priority must be rejected.
     */
    graph.nodes[1].priority = 5;

    PriorityFlowResult unchanged = result;

    assert(calculatePriorityFlow(
        &graph, &result) == 0);

    assert(memcmp(
        &result,
        &unchanged,
        sizeof(PriorityFlowResult)) == 0);

    printf("[PASS] Invalid input rejected safely\n");

    printf("\nAll production API tests passed!\n");

    return 0;
}
