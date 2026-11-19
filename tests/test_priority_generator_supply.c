
#include <assert.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 2;
    graph.edgeCount = 0;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 60.0;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = GENERATOR;
    graph.nodes[1].supply = 40.0;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    assert(validateGeneratorSupply(
        &graph, &network) == 1);

    printf("[PASS] Initial generator supplies valid\n");

    /*
     * Simulate 45 MW from Generator A.
     */
    assert(pushPriorityFlow(
        &network, 0, 45.0) == 1);

    /*
     * Simulate 30 MW from Generator B.
     */
    assert(pushPriorityFlow(
        &network, 2, 30.0) == 1);

    assert(validateGeneratorSupply(
        &graph, &network) == 1);

    printf("[PASS] Multiple generator allocations valid\n");

    /*
     * Corrupt Generator A's residual
     * capacity to imply 70 MW output.
     */
    PriorityNetwork invalid = network;

    invalid.arcs[0].capacity = -10.0;

    assert(validateGeneratorSupply(
        &graph, &invalid) == 0);

    printf("[PASS] Generator over-supply rejected\n");

    /*
     * Remove Generator B's source edge
     * from the active arc range.
     */
    invalid = network;
    invalid.arcCount = 2;

    assert(validateGeneratorSupply(
        &graph, &invalid) == 0);

    printf("[PASS] Missing generator connection rejected\n");

    printf("\nAll generator supply tests passed!\n");

    return 0;
}

