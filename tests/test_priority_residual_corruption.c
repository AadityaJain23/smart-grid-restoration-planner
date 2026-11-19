
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../src/priority_flow.c"

/*
 * Build a small valid residual network.
 */
static void buildValidNetwork(
    PriorityNetwork *network
)
{
    memset(network, 0, sizeof(*network));

    network->vertexCount = 3;
    network->sourceIndex = 0;
    network->sinkIndex = 2;

    PriorityCost zero = zeroPriorityCost();

    assert(addPriorityArcPair(
        network,
        0, 1,
        30.0, 30.0,
        zero) == 1);

    assert(addPriorityArcPair(
        network,
        1, 2,
        20.0, 0.0,
        zero) == 1);

    assert(validatePriorityNetwork(network) == 1);
}

int main(void)
{
    static PriorityNetwork network;
    static PriorityNetwork invalid;

    buildValidNetwork(&network);

    /*
     * Test 1: Invalid source vertex.
     */
    invalid = network;
    invalid.arcs[0].from = 999;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Invalid source vertex rejected\n");

    /*
     * Test 2: Invalid destination vertex.
     */
    invalid = network;
    invalid.arcs[0].to = 999;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Invalid destination vertex rejected\n");

    /*
     * Test 3: Invalid reverse index.
     */
    invalid = network;
    invalid.arcs[0].reverseIndex = 999;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Invalid reverse index rejected\n");

    /*
     * Test 4: Broken reverse relationship.
     */
    invalid = network;
    invalid.arcs[1].reverseIndex = 2;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Broken reverse relationship rejected\n");

    /*
     * Test 5: Negative capacity.
     */
    invalid = network;
    invalid.arcs[0].capacity = -5.0;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Negative residual capacity rejected\n");

    /*
     * Test 6: NaN capacity.
     */
    invalid = network;
    invalid.arcs[0].capacity = NAN;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] NaN residual capacity rejected\n");

    /*
     * Test 7: Incorrect reverse cost.
     */
    invalid = network;
    invalid.arcs[1].cost.component[0] = 3.0;

    assert(validatePriorityNetwork(&invalid) == 0);

    printf("[PASS] Incorrect reverse cost rejected\n");

    /*
     * Test 8: Failed update must not
     * modify the original network.
     */
    invalid = network;

    PriorityNetwork before = invalid;

    assert(pushPriorityFlow(
        &invalid, 0, 100.0) == 0);

    assert(memcmp(
        &invalid,
        &before,
        sizeof(PriorityNetwork)) == 0);

    printf("[PASS] Failed update preserves network\n");

    /*
     * Test 9: BFS rejects malformed network.
     */
    invalid = network;
    invalid.arcs[0].reverseIndex = -1;

    int parentArc[MAX_FLOW_VERTICES];

    assert(bfsPriorityAugmentingPath(
        &invalid, parentArc) == -1);

    printf("[PASS] BFS rejects malformed network\n");

    /*
     * Test 10: Cycle detection rejects
     * malformed network.
     */
    invalid = network;
    invalid.arcs[0].from = 999;

    int cycleArcs[MAX_FLOW_VERTICES];
    int cycleLength = 0;

    assert(findNegativePriorityCycle(
        &invalid,
        cycleArcs,
        &cycleLength) == -1);

    printf("[PASS] Cycle detection rejects malformed network\n");

    printf("\nAll residual corruption tests passed!\n");

    return 0;
}
