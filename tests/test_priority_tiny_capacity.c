
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 2;
    graph.edgeCount = 1;

    double tinyCapacity = FLOW_EPS / 2.0;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = tinyCapacity;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = tinyCapacity;
    graph.nodes[1].priority = 1;

    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = tinyCapacity;
    graph.edges[0].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    printf("[PASS] Tiny-capacity network constructed\n");

    double maxFlow = -1.0;
    int paths = -1;

    assert(runPriorityMaximumFlow(
        &network, &maxFlow, &paths) == 1);

    /*
     * Current FLOW_EPS policy treats
     * this capacity as numerically zero.
     */
    assert(maxFlow == 0.0);
    assert(paths == 0);

    printf("[PASS] Tiny capacity ignored by BFS\n");

    /*
     * Verify the residual network remains
     * structurally valid.
     */
    assert(validatePriorityNetwork(
        &network) == 1);

    printf("[PASS] Tiny residual network remains valid\n");

    printf("\nAll tiny-capacity tests passed!\n");

    return 0;
}
