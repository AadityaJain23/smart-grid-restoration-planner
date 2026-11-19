
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

static int nearlyEqualMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 6;
    graph.edgeCount = 5;

    /* Generator A */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 25.75;

    /* Generator B */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = GENERATOR;
    graph.nodes[1].supply = 14.50;

    /* Central Substation */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = SUBSTATION;

    /* Hospital */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 12.25;
    graph.nodes[3].priority = 1;

    /* School */
    graph.nodes[4].id = 50;
    graph.nodes[4].type = CONSUMER;
    graph.nodes[4].demand = 8.50;
    graph.nodes[4].priority = 2;

    /* Residential */
    graph.nodes[5].id = 60;
    graph.nodes[5].type = CONSUMER;
    graph.nodes[5].demand = 19.50;
    graph.nodes[5].priority = 3;

    /*
     * Generator connections to substation.
     */
    graph.edges[0].from = 10;
    graph.edges[0].to = 30;
    graph.edges[0].capacity = 25.75;
    graph.edges[0].active = 1;

    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 14.50;
    graph.edges[1].active = 1;

    /*
     * Substation to consumers.
     */
    graph.edges[2].from = 30;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 12.25;
    graph.edges[2].active = 1;

    graph.edges[3].from = 30;
    graph.edges[3].to = 50;
    graph.edges[3].capacity = 8.50;
    graph.edges[3].active = 1;

    graph.edges[4].from = 30;
    graph.edges[4].to = 60;
    graph.edges[4].capacity = 19.50;
    graph.edges[4].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    printf("[PASS] Fractional network constructed\n");

    double maxFlow = 0.0;
    int paths = 0;

    assert(runPriorityMaximumFlow(
        &network, &maxFlow, &paths) == 1);

    assert(nearlyEqualMW(maxFlow, 40.25));

    printf("[PASS] Fractional Maximum Flow\n");

    assert(validateGeneratorSupply(
        &graph, &network) == 1);

    assert(validateConsumerDemand(
        &graph, &network) == 1);

    assert(validateTransmissionCapacity(
        &graph, &network) == 1);

    assert(validateFlowConservation(
        &graph, &network) == 1);

    printf("[PASS] Fractional flow conservation\n");

    PriorityFlowResult result = {0};

    assert(extractPriorityFlowResult(
        &graph, &network, paths, &result) == 1);

    assert(nearlyEqualMW(
        result.deliveredByPriority[0], 12.25));

    assert(nearlyEqualMW(
        result.deliveredByPriority[1], 8.50));

    assert(nearlyEqualMW(
        result.deliveredByPriority[2], 19.50));

    printf("[PASS] Fractional priority totals\n");

    assert(nearlyEqualMW(
        result.totalUnserved, 0.0));

    printf("[PASS] Fractional demand fully served\n");

    printf("\nAll fractional-flow tests passed!\n");

    return 0;
}
