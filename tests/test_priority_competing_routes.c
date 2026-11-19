
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        FLOW_EPS * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 4;
    graph.edgeCount = 3;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 20.0;

    /* Substation */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    /*
     * Industry is deliberately stored
     * before Hospital in the node array.
     */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 20.0;
    graph.nodes[2].priority = 3;

    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 1;

    /* Generator -> Substation */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 20.0;
    graph.edges[0].active = 1;

    /* Substation -> Industry */
    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 20.0;
    graph.edges[1].active = 1;

    /* Substation -> Hospital */
    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 20.0;
    graph.edges[2].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    printf("[PASS] Competing-route network built\n");

    /*
     * Phase 1: Maximum Flow.
     */
    double maxFlow = 0.0;
    int paths = 0;

    assert(runPriorityMaximumFlow(
        &network, &maxFlow, &paths) == 1);

    assert(closeMW(maxFlow, 20.0));

    PriorityFlowResult before = {0};

    assert(extractPriorityFlowResult(
        &graph, &network, paths, &before) == 1);

    printf("Before optimization:\n");
    printf("Hospital: %.2f MW\n",
           before.deliveredToNode[3]);
    printf("Industry: %.2f MW\n",
           before.deliveredToNode[2]);

    /*
     * This scenario is designed so BFS
     * initially chooses Industry.
     */
    assert(closeMW(
        before.deliveredToNode[2], 20.0));

    assert(closeMW(
        before.deliveredToNode[3], 0.0));

    printf("[PASS] Initial lower-priority allocation\n");

    /*
     * Phase 2: Negative-cycle optimization.
     */
    int cyclesCanceled = 0;
    PriorityCost costChange =
        zeroPriorityCost();

    assert(cancelNegativePriorityCycles(
        &network,
        100,
        &cyclesCanceled,
        &costChange) == 1);

    assert(cyclesCanceled >= 1);

    printf("[PASS] Negative cycle canceled\n");

    PriorityFlowResult after = {0};

    assert(extractPriorityFlowResult(
        &graph, &network, paths, &after) == 1);

    assert(closeMW(
        after.deliveredToNode[3], 20.0));

    assert(closeMW(
        after.deliveredToNode[2], 0.0));

    printf("[PASS] Hospital receives 20 MW\n");
    printf("[PASS] Industry allocation rerouted\n");

    /*
     * Total throughput must remain 20 MW.
     */
    assert(closeMW(
        after.totalDelivered, maxFlow));

    printf("[PASS] Maximum throughput preserved\n");

    /*
     * Final feasibility validation.
     */
    assert(validateFlowConservation(
        &graph, &network) == 1);

    printf("[PASS] Flow conservation preserved\n");

    printf("\nAll competing-route tests passed!\n");

    return 0;
}
