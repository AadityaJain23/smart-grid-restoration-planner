
#include <assert.h>
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
     * Create cycle:
     * 0 -> 1 -> 2 -> 0
     *
     * Total cost = (-1, 0, 0).
     */
    assert(addPriorityArcPair(
        &network, 0, 1,
        10.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 1, 2,
        10.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 2, 0,
        10.0, 0.0,
        makeConsumerCost(1)) == 1);

    int cycleArcs[MAX_FLOW_VERTICES];
    int cycleLength = 0;

    int status = findNegativePriorityCycle(
        &network,
        cycleArcs,
        &cycleLength
    );

    assert(status == 1);
    assert(cycleLength == 3);

    printf("[PASS] Negative cycle detected\n");

    PriorityCost totalCost =
        zeroPriorityCost();

    for (int i = 0; i < cycleLength; i++) {
        totalCost = addPriorityCost(
            totalCost,
            network.arcs[cycleArcs[i]].cost
        );
    }

    assert(totalCost.component[0] == -1.0);

    printf("[PASS] Negative cycle reconstructed\n");

    /*
     * Disable the negative-cost arc.
     */
    network.arcs[4].capacity = 0.0;

    status = findNegativePriorityCycle(
        &network,
        cycleArcs,
        &cycleLength
    );

    assert(status == 0);

    printf("[PASS] No negative cycle detected\n");

    printf("\nAll cycle detection tests passed!\n");

    return 0;
}
