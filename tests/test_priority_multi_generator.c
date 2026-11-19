
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

static int closeEnough(double a, double b)
{
    return fabs(a - b) <
           FLOW_EPS * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 5;
    graph.edgeCount = 5;

    /* Generator A */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 40.0;

    /* Generator B */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = GENERATOR;
    graph.nodes[1].supply = 30.0;

    /* Main Bus */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = SUBSTATION;

    /* Hospital */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 1;

    /* Industry */
    graph.nodes[4].id = 50;
    graph.nodes[4].type = CONSUMER;
    graph.nodes[4].demand = 50.0;
    graph.nodes[4].priority = 3;

    /* Two parallel lines from Generator A */
    graph.edges[0].from = 10;
    graph.edges[0].to = 30;
    graph.edges[0].capacity = 25.0;
    graph.edges[0].active = 1;

    graph.edges[1].from = 10;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 20.0;
    graph.edges[1].active = 1;

    /* Generator B -> Main Bus */
    graph.edges[2].from = 20;
    graph.edges[2].to = 30;
    graph.edges[2].capacity = 30.0;
    graph.edges[2].active = 1;

    /* Main Bus -> Hospital */
    graph.edges[3].from = 30;
    graph.edges[3].to = 40;
    graph.edges[3].capacity = 20.0;
    graph.edges[3].active = 1;

    /* Main Bus -> Industry */
    graph.edges[4].from = 30;
    graph.edges[4].to = 50;
    graph.edges[4].capacity = 50.0;
    graph.edges[4].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    printf("[PASS] Multi-generator network built\n");

    /*
     * Each active physical line contributes
     * two residual arcs.
     *
     * 5 lines = 10 arcs
     * 2 generators = 4 arcs
     * 2 consumers = 4 arcs
     *
     * Total = 18 arcs
     */
    assert(network.arcCount == 18);

    printf("[PASS] Parallel lines represented separately\n");

    double maximumFlow = 0.0;
    int paths = 0;

    assert(runPriorityMaximumFlow(
        &network,
        &maximumFlow,
        &paths) == 1);

    assert(closeEnough(maximumFlow, 70.0));

    printf("[PASS] Maximum Flow = 70 MW\n");

    assert(validateGeneratorSupply(
        &graph, &network) == 1);

    printf("[PASS] Generator supply limits\n");

    assert(validateConsumerDemand(
        &graph, &network) == 1);

    printf("[PASS] Consumer demand limits\n");

    assert(validateTransmissionCapacity(
        &graph, &network) == 1);

    printf("[PASS] Parallel transmission capacities\n");

    assert(validateFlowConservation(
        &graph, &network) == 1);

    printf("[PASS] Full flow conservation\n");

    PriorityFlowResult result = {0};

    assert(extractPriorityFlowResult(
        &graph, &network, paths, &result) == 1);

    assert(closeEnough(
        result.deliveredByPriority[0], 20.0));

    assert(closeEnough(
        result.deliveredByPriority[2], 50.0));

    assert(closeEnough(result.totalUnserved, 0.0));

    printf("[PASS] Priority allocations correct\n");

    printf("\nAll multi-generator tests passed!\n");

    return 0;
}
