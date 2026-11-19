
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    PriorityNetwork network = {0};

    network.vertexCount = 6;
    network.sourceIndex = 0;
    network.sinkIndex = 5;

    PriorityCost zero = zeroPriorityCost();

    /*
     * Negative cycle A:
     * 0 -> 1 -> 0
     * Cost: (-1, 0, 0)
     * Capacity: 6 MW
     */
    assert(addPriorityArcPair(
        &network, 0, 1,
        6.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 1, 0,
        6.0, 0.0,
        makeConsumerCost(1)) == 1);

    /*
     * Negative cycle B:
     * 2 -> 3 -> 2
     * Cost: (0, -1, 0)
     * Capacity: 4 MW
     */
    assert(addPriorityArcPair(
        &network, 2, 3,
        4.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 3, 2,
        4.0, 0.0,
        makeConsumerCost(2)) == 1);

    int count = 0;
    PriorityCost totalChange =
        zeroPriorityCost();

    assert(cancelNegativePriorityCycles(
        &network,
        10,
        &count,
        &totalChange) == 1);

    assert(count == 2);

    printf("[PASS] Multiple cycles canceled\n");

    assert(fabs(
        totalChange.component[0] + 6.0
    ) < FLOW_EPS);

    assert(fabs(
        totalChange.component[1] + 4.0
    ) < FLOW_EPS);

    assert(totalChange.component[2] == 0.0);

    printf("[PASS] Lexicographic cost accumulated\n");

    /*
     * Verify the two negative-cost
     * forward arcs are saturated.
     */
    assert(network.arcs[2].capacity == 0.0);
    assert(network.arcs[6].capacity == 0.0);

    printf("[PASS] Residual capacities updated\n");

    /*
     * Running the optimizer again should
     * find no negative cycles.
     */
    count = -1;

    assert(cancelNegativePriorityCycles(
        &network,
        10,
        &count,
        &totalChange) == 1);

    assert(count == 0);

    printf("[PASS] Optimized network is stable\n");

    printf("\nAll cycle-loop tests passed!\n");

    return 0;
}
