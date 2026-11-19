
#include <assert.h>
#include <stdio.h>
#define PRIORITY_FLOW_LEGACY_TESTS
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
        20.0, 0.0,
        zero) == 1);

    assert(validatePriorityNetwork(&network) == 1);

    printf("[PASS] Valid residual network\n");

    PriorityNetwork invalid = network;

    invalid.arcs[0].from = 100;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Invalid vertex rejected\n");

    invalid = network;
    invalid.arcs[0].reverseIndex = 999;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Invalid reverse index rejected\n");

    invalid = network;
    invalid.arcs[0].capacity = -5.0;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Negative capacity rejected\n");

    invalid = network;
    invalid.arcs[1].cost.component[0] = 2.0;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Invalid reverse cost rejected\n");

    /*
     * Verify Bellman-Ford rejects a
     * malformed network safely.
     */
    invalid = network;
    invalid.arcs[0].to = 100;

    int parentArc[MAX_FLOW_VERTICES];
    PriorityCost dist[MAX_FLOW_VERTICES];

    assert(bellmanFordPriority(
        &invalid,
        parentArc,
        dist) == -1);

    printf("[PASS] Bellman-Ford rejects invalid network\n");

    printf("\nAll validation tests passed!\n");

    return 0;
}
