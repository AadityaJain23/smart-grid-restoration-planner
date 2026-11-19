
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

    assert(addPriorityArcPair(
        &network, 0, 1,
        20.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 1, 2,
        10.0, 0.0, zero) == 1);

    int parentArc[MAX_FLOW_VERTICES];

    assert(bfsPriorityAugmentingPath(
        &network, parentArc) == 1);

    assert(parentArc[1] == 0);
    assert(parentArc[2] == 2);

    printf("[PASS] BFS augmenting path found\n");

    /*
     * Disable the sink connection.
     */
    network.arcs[2].capacity = 0.0;

    assert(bfsPriorityAugmentingPath(
        &network, parentArc) == 0);

    printf("[PASS] Unreachable sink detected\n");

    /*
     * Invalid reverse relationship.
     */
    PriorityNetwork invalid = network;

    invalid.arcs[0].reverseIndex = 999;

    assert(bfsPriorityAugmentingPath(
        &invalid, parentArc) == -1);

    printf("[PASS] Invalid network rejected\n");

    printf("\nAll BFS tests passed!\n");

    return 0;
}

