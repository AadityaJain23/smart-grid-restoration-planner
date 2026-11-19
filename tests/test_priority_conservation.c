
#include <assert.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 4;
    graph.edgeCount = 3;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 30.0;

    /* Substation */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    /* Hospital */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 20.0;
    graph.nodes[2].priority = 1;

    /* School */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 10.0;
    graph.nodes[3].priority = 2;

    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 30.0;
    graph.edges[0].active = 1;

    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 20.0;
    graph.edges[1].active = 1;

    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 10.0;
    graph.edges[2].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    /*
     * Zero flow is feasible and balanced.
     */
    assert(validateFlowConservation(
        &graph, &network) == 1);

    printf("[PASS] Zero-flow conservation\n");

    /*
     * Calculate a feasible maximum flow.
     */
    double maximumFlow = 0.0;
    int paths = 0;

    assert(runPriorityMaximumFlow(
        &network,
        &maximumFlow,
        &paths) == 1);

    assert(validateFlowConservation(
        &graph, &network) == 1);

    printf("[PASS] Maximum-flow conservation\n");

    /*
     * Corrupt a transmission arc pair while
     * preserving its capacity sum.
     *
     * This creates a valid-looking line flow
     * but violates node conservation.
     */
    PriorityNetwork invalid = network;

    invalid.arcs[0].capacity += 5.0;
    invalid.arcs[1].capacity -= 5.0;

    assert(validateTransmissionCapacity(
        &graph, &invalid) == 1);

    assert(validateFlowConservation(
        &graph, &invalid) == 0);

    printf("[PASS] Hidden flow imbalance detected\n");

    printf("\nAll flow conservation tests passed!\n");

    return 0;
}

