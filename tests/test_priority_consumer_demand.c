
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 3;
    graph.edgeCount = 0;

    /* Hospital - Priority 1 */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = CONSUMER;
    graph.nodes[0].demand = 20.0;
    graph.nodes[0].priority = 1;

    /* School - Priority 2 */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = 10.0;
    graph.nodes[1].priority = 2;

    /* Residential - Priority 3 */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 25.0;
    graph.nodes[2].priority = 3;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    assert(validateConsumerDemand(
        &graph, &network) == 1);

    printf("[PASS] Initial consumer demands valid\n");

    /*
     * No physical generator is present.
     * These pushes test sink-edge bookkeeping,
     * not a complete feasible power flow.
     *
     * Consumer forward arc indices:
     * Hospital = 0
     * School = 2
     * Residential = 4
     */

    /* Hospital fully served: 20 MW */
    assert(pushPriorityFlow(
        &network, 0, 20.0) == 1);

    /* School partially served: 5 MW */
    assert(pushPriorityFlow(
        &network, 2, 5.0) == 1);

    /* Residential remains unserved */

    assert(validateConsumerDemand(
        &graph, &network) == 1);

    printf("[PASS] Full and partial deliveries valid\n");
    printf("[PASS] Unserved consumer valid\n");

    /*
     * Corrupt Hospital's forward residual
     * to imply an over-allocation.
     */
    PriorityNetwork invalid = network;

    invalid.arcs[0].capacity = -1.0;

    assert(validateConsumerDemand(
        &graph, &invalid) == 0);

    printf("[PASS] Over-delivery rejected\n");

    /*
     * Corrupt School's reverse residual.
     */
    invalid = network;
    invalid.arcs[3].capacity = 7.0;

    assert(validateConsumerDemand(
        &graph, &invalid) == 0);

    printf("[PASS] Reverse residual mismatch rejected\n");

    /*
     * Remove the Residential sink pair
     * from the active arc range.
     */
    invalid = network;
    invalid.arcCount = 4;

    assert(validateConsumerDemand(
        &graph, &invalid) == 0);

    printf("[PASS] Missing consumer connection rejected\n");

    printf("\nAll consumer demand tests passed!\n");

    return 0;
}
