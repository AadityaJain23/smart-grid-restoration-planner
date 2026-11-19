
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
 * Construct the baseline network.
 *
 * Generator -> Substation -> Consumers
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
    graph->edges[2].capacity = 20.0;
    graph->edges[2].active = 1;
}

/*
 * Compare both public solvers and verify
 * the expected priority allocation.
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

    /* Maximum throughput must agree. */
    assert(closeMW(
        maxResult.maxFlow,
        priorityResult.totalDelivered));

    assert(closeMW(
        priorityResult.totalDelivered,
        expectedTotal));

    /* Verify priority allocation. */
    assert(closeMW(
        priorityResult.deliveredByPriority[0],
        expectedP1));

    assert(closeMW(
        priorityResult.deliveredByPriority[1],
        0.0));

    assert(closeMW(
        priorityResult.deliveredByPriority[2],
        expectedP3));

    /* Individual consumer allocations. */
    assert(closeMW(
        priorityResult.deliveredToNode[2],
        expectedP1));

    assert(closeMW(
        priorityResult.deliveredToNode[3],
        expectedP3));

    /* Supply and demand constraints. */
    assert(priorityResult.totalDelivered >= 0.0);

    assert(priorityResult.totalDelivered <=
        priorityResult.totalSupply + 1e-7);

    assert(closeMW(
        priorityResult.totalDelivered +
        priorityResult.totalUnserved,
        priorityResult.totalDemand));

    /* Original graph must remain unchanged. */
    assert(memcmp(
        graph, &original, sizeof(Graph)) == 0);

    printf("[PASS] %s\n", name);
}

int main(void)
{
    static Graph graph;

    /* Test 1: Zero generator supply */
    buildNetwork(&graph);
    graph.nodes[0].supply = 0.0;

    checkScenario(
        "Zero generator supply",
        &graph, 0.0, 0.0, 0.0);

    /* Test 2: Exact supply */
    buildNetwork(&graph);

    checkScenario(
        "Exact supply",
        &graph, 40.0, 20.0, 20.0);

    /* Test 3: Limited supply */
    buildNetwork(&graph);
    graph.nodes[0].supply = 25.0;

    checkScenario(
        "Limited supply",
        &graph, 25.0, 20.0, 5.0);

    /* Test 4: Transmission bottleneck */
    buildNetwork(&graph);
    graph.edges[0].capacity = 15.0;

    checkScenario(
        "Transmission bottleneck",
        &graph, 15.0, 15.0, 0.0);

    /* Test 5: Exact bottleneck */
    buildNetwork(&graph);
    graph.edges[0].capacity = 20.0;

    checkScenario(
        "Exact bottleneck",
        &graph, 20.0, 20.0, 0.0);

    /* Test 6: Excess generation */
    buildNetwork(&graph);
    graph.nodes[0].supply = 100.0;

    checkScenario(
        "Excess generation",
        &graph, 40.0, 20.0, 20.0);

    /* Test 7: Tight bottleneck */
    buildNetwork(&graph);
    graph.edges[0].capacity = 5.0;

    checkScenario(
        "Tight bottleneck",
        &graph, 5.0, 5.0, 0.0);

    /* Test 8: Capacity restored */
    buildNetwork(&graph);

    checkScenario(
        "Capacity restored",
        &graph, 40.0, 20.0, 20.0);

    printf("\nAll capacity boundary tests passed!\n");

    return 0;
}
