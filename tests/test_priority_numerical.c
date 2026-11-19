
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

static int closeDouble(double a, double b)
{
    return fabs(a - b) <=
        1e-9 * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    PriorityNetwork network = {0};

    network.vertexCount = 3;
    network.sourceIndex = 0;
    network.sinkIndex = 2;

    PriorityCost zero = zeroPriorityCost();

    /*
     * Undirected line with capacity 0.3 MW.
     */
    assert(addPriorityArcPair(
        &network,
        0, 1,
        0.3, 0.3,
        zero) == 1);

    assert(validatePriorityNetwork(
        &network) == 1);

    printf("[PASS] Fractional residual network\n");

    /*
     * Send 0.1 MW forward.
     */
    assert(pushPriorityFlow(
        &network, 0, 0.1) == 1);

    assert(closeDouble(
        network.arcs[0].capacity, 0.2));

    assert(closeDouble(
        network.arcs[1].capacity, 0.4));

    printf("[PASS] Fractional forward update\n");

    /*
     * Send 0.1 MW in reverse.
     */
    assert(pushPriorityFlow(
        &network, 1, 0.1) == 1);

    assert(closeDouble(
        network.arcs[0].capacity, 0.3));

    assert(closeDouble(
        network.arcs[1].capacity, 0.3));

    printf("[PASS] Fractional reverse update\n");

    /*
     * Repeated forward/reverse updates.
     */
    for (int i = 0; i < 1000; i++) {

        assert(pushPriorityFlow(
            &network, 0, 0.1) == 1);

        assert(pushPriorityFlow(
            &network, 1, 0.1) == 1);
    }

    assert(closeDouble(
        network.arcs[0].capacity, 0.3));

    assert(closeDouble(
        network.arcs[1].capacity, 0.3));

    printf("[PASS] Repeated updates stable\n");

    /*
     * Verify shared-capacity invariant.
     */
    assert(closeDouble(
        network.arcs[0].capacity +
        network.arcs[1].capacity,
        0.6));

    printf("[PASS] Shared capacity conserved\n");

    /*
     * Tiny updates should be rejected.
     */
    assert(pushPriorityFlow(
        &network,
        0,
        FLOW_EPS / 2.0) == 0);

    printf("[PASS] Tiny update rejected\n");

    /*
     * Nonfinite updates should be rejected.
     */
    assert(pushPriorityFlow(
        &network, 0, NAN) == 0);

    assert(pushPriorityFlow(
        &network, 0, INFINITY) == 0);

    printf("[PASS] Nonfinite updates rejected\n");

    printf("\nAll numerical stability tests passed!\n");

    return 0;
}
