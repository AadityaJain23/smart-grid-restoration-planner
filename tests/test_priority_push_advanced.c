
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    PriorityNetwork network = {0};

    network.vertexCount = 3;
    network.sourceIndex = 0;
    network.sinkIndex = 2;

    PriorityCost zero = zeroPriorityCost();

    assert(addPriorityArcPair(
        &network,
        0, 1,
        30.0, 30.0,
        zero) == 1);

    assert(validatePriorityNetwork(&network) == 1);

    printf("[PASS] Initial network valid\n");

    /*
     * Send 10 MW forward.
     */
    assert(pushPriorityFlow(
        &network, 0, 10.0) == 1);

    assert(network.arcs[0].capacity == 20.0);
    assert(network.arcs[1].capacity == 40.0);

    printf("[PASS] Forward update\n");

    /*
     * Send 5 MW in reverse.
     */
    assert(pushPriorityFlow(
        &network, 1, 5.0) == 1);

    assert(network.arcs[0].capacity == 25.0);
    assert(network.arcs[1].capacity == 35.0);

    printf("[PASS] Reverse update\n");

    /*
     * Shared-capacity invariant.
     */
    assert(fabs(
        network.arcs[0].capacity +
        network.arcs[1].capacity - 60.0
    ) < FLOW_EPS);

    printf("[PASS] Shared capacity conserved\n");

    /*
     * Reject flow exceeding residual capacity.
     */
    double beforeForward =
        network.arcs[0].capacity;

    double beforeReverse =
        network.arcs[1].capacity;

    assert(pushPriorityFlow(
        &network, 0, 40.0) == 0);

    assert(network.arcs[0].capacity ==
           beforeForward);

    assert(network.arcs[1].capacity ==
           beforeReverse);

    printf("[PASS] Excess flow rejected safely\n");

    /*
     * Reject invalid reverse index.
     */
    PriorityNetwork invalid = network;

    invalid.arcs[0].reverseIndex = 999;

    assert(pushPriorityFlow(
        &invalid, 0, 5.0) == 0);

    printf("[PASS] Invalid reverse index rejected\n");

    /*
     * Reject numerically insignificant flow.
     */
    assert(pushPriorityFlow(
        &network, 0, FLOW_EPS / 2.0) == 0);

    printf("[PASS] Tiny flow rejected\n");

    printf("\nAll advanced residual tests passed!\n");

    return 0;
}
