
#include <assert.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 3;
    graph.edgeCount = 2;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = SUBSTATION;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    graph.nodes[2].id = 30;
    graph.nodes[2].type = SUBSTATION;

    /*
     * Active transmission line:
     * 10 <-> 20, capacity 30 MW.
     */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 30.0;
    graph.edges[0].active = 1;

    /*
     * Failed transmission line:
     * 20 <-> 30, capacity 15 MW.
     */
    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 15.0;
    graph.edges[1].active = 0;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    assert(network.arcCount == 2);

    assert(validateTransmissionCapacity(
        &graph, &network) == 1);

    printf("[PASS] Initial transmission capacities\n");

    /*
     * Send 20 MW forward.
     */
    assert(pushPriorityFlow(
        &network, 0, 20.0) == 1);

    assert(validateTransmissionCapacity(
        &graph, &network) == 1);

    printf("[PASS] Forward transmission flow\n");

    /*
     * Reverse the 20 MW and then send
     * 15 MW in the opposite direction.
     */
    assert(pushPriorityFlow(
        &network, 1, 35.0) == 1);

    assert(validateTransmissionCapacity(
        &graph, &network) == 1);

    printf("[PASS] Reverse transmission flow\n");

    /*
     * Deliberately corrupt the residual
     * pair to violate the physical limit.
     */
    PriorityNetwork invalid = network;

    invalid.arcs[0].capacity = 70.0;
    invalid.arcs[1].capacity = 0.0;

    assert(validateTransmissionCapacity(
        &graph, &invalid) == 0);

    printf("[PASS] Capacity violation rejected\n");

    /*
     * The failed line must not have
     * a corresponding residual pair.
     */
    assert(network.arcCount == 2);

    printf("[PASS] Failed line excluded\n");

    printf("\nAll transmission capacity tests passed!\n");

    return 0;
}
