
#include <assert.h>
#include <math.h>
#include <stdio.h>

/*
 * White-box test for private helpers.
 * Do not compile priority_flow.c separately.
 */
#include "../src/priority_flow.c"

int main(void)
{
    PriorityNetwork network = {0};

    network.vertexCount = 4;
    network.sourceIndex = 2;
    network.sinkIndex = 3;

    PriorityCost zero = zeroPriorityCost();

    /*
     * Add one undirected transmission line
     * with shared capacity 30 MW.
     */
    assert(addPriorityArcPair(
        &network, 0, 1,
        30.0, 30.0, zero) == 1);

    assert(network.arcCount == 2);

    assert(network.arcs[0].reverseIndex == 1);
    assert(network.arcs[1].reverseIndex == 0);

    assert(network.arcs[0].capacity == 30.0);
    assert(network.arcs[1].capacity == 30.0);

    printf("[PASS] Shared-capacity arc creation\n");

    /*
     * Send 10 MW from node 0 to node 1.
     */
    assert(pushPriorityFlow(
        &network, 0, 10.0) == 1);

    assert(network.arcs[0].capacity == 20.0);
    assert(network.arcs[1].capacity == 40.0);

    printf("[PASS] Forward residual update\n");

    /*
     * Reverse 10 MW of the previous flow.
     */
    assert(pushPriorityFlow(
        &network, 1, 10.0) == 1);

    assert(network.arcs[0].capacity == 30.0);
    assert(network.arcs[1].capacity == 30.0);

    printf("[PASS] Reverse residual update\n");

    /*
     * Reverse the physical direction:
     * 15 MW from node 1 to node 0.
     */
    assert(pushPriorityFlow(
        &network, 1, 15.0) == 1);

    assert(network.arcs[0].capacity == 45.0);
    assert(network.arcs[1].capacity == 15.0);

    assert(fabs(
        network.arcs[0].capacity +
        network.arcs[1].capacity - 60.0
    ) < FLOW_EPS);

    printf("[PASS] Shared capacity conservation\n");

    /*
     * Invalid flow must be rejected.
     */
    assert(pushPriorityFlow(
        &network, 1, 20.0) == 0);

    printf("[PASS] Capacity overflow prevention\n");

    /*
     * Verify reverse lexicographic costs.
     */
    PriorityCost p1 = makeConsumerCost(1);

    assert(addPriorityArcPair(
        &network, 1, 3,
        20.0, 0.0, p1) == 1);

    assert(network.arcs[2].cost.component[0] == -1.0);
    assert(network.arcs[3].cost.component[0] == 1.0);

    printf("[PASS] Reverse priority costs\n");

    printf("\nAll residual arc tests passed!\n");

    return 0;
}
