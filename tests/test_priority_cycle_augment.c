
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

    /*
     * Create a negative cycle:
     *
     * 0 -> 1 -> 2 -> 0
     *
     * Capacities: 10, 8, 6 MW
     */
    assert(addPriorityArcPair(
        &network, 0, 1,
        10.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 1, 2,
        8.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 2, 0,
        6.0, 0.0,
        makeConsumerCost(1)) == 1);

    int cycleArcs[MAX_FLOW_VERTICES];
    int cycleLength = 0;

    assert(findNegativePriorityCycle(
        &network,
        cycleArcs,
        &cycleLength) == 1);

    assert(cycleLength == 3);

    printf("[PASS] Negative cycle found\n");

    double deltaFlow = 0.0;
    PriorityCost deltaCost =
        zeroPriorityCost();

    assert(augmentNegativePriorityCycle(
        &network,
        cycleArcs,
        cycleLength,
        &deltaFlow,
        &deltaCost) == 1);

    assert(fabs(deltaFlow - 6.0) < FLOW_EPS);

    printf("[PASS] Cycle bottleneck calculation\n");

    /*
     * Forward residual capacities.
     */
    assert(network.arcs[0].capacity == 4.0);
    assert(network.arcs[2].capacity == 2.0);
    assert(network.arcs[4].capacity == 0.0);

    printf("[PASS] Forward residual updates\n");

    /*
     * Reverse residual capacities.
     */
    assert(network.arcs[1].capacity == 6.0);
    assert(network.arcs[3].capacity == 6.0);
    assert(network.arcs[5].capacity == 6.0);

    printf("[PASS] Reverse residual updates\n");

    /*
     * Lexicographic cost improvement.
     */
    assert(deltaCost.component[0] == -6.0);
    assert(deltaCost.component[1] == 0.0);
    assert(deltaCost.component[2] == 0.0);

    printf("[PASS] Lexicographic cost improvement\n");

    /*
     * The saturated edge should prevent
     * the same negative cycle from remaining.
     */
    int status = findNegativePriorityCycle(
        &network,
        cycleArcs,
        &cycleLength);

    assert(status == 0);

    printf("[PASS] Negative cycle eliminated\n");

    printf("\nAll cycle augmentation tests passed!\n");

    return 0;
}
