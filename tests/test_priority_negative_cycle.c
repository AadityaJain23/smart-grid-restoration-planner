
#include <assert.h>
#include <stdio.h>
#define PRIORITY_FLOW_LEGACY_TESTS
#define PRIORITY_FLOW_PROTOTYPE_TEST
#include "../src/priority_flow.c"

int main(void)
{
    PriorityNetwork network = {0};

    network.vertexCount = 4;
    network.sourceIndex = 0;
    network.sinkIndex = 3;

    PriorityCost zero = zeroPriorityCost();

    /*
     * Source -> A
     */
    assert(addPriorityArcPair(
        &network,
        0, 1,
        10.0, 0.0,
        zero) == 1);

    /*
     * A -> B
     */
    assert(addPriorityArcPair(
        &network,
        1, 2,
        10.0, 0.0,
        zero) == 1);

    /*
     * B -> A
     *
     * Negative lexicographic cost.
     */
    assert(addPriorityArcPair(
        &network,
        2, 1,
        10.0, 0.0,
        makeConsumerCost(1)) == 1);

    /*
     * B -> Sink
     */
    assert(addPriorityArcPair(
        &network,
        2, 3,
        10.0, 0.0,
        zero) == 1);

    int parentArc[MAX_FLOW_VERTICES];
    PriorityCost dist[MAX_FLOW_VERTICES];

    int status = bellmanFordPriority(
        &network,
        parentArc,
        dist);

    assert(status == -1);

    printf("[PASS] Negative cycle detected\n");

    /*
     * The prototype should reject the
     * unsupported negative-cycle case.
     */
    double totalFlow = 0.0;
    PriorityCost totalCost =
        zeroPriorityCost();

    int paths = 0;

    assert(runPriorityFlowPrototype(
        &network,
        &totalFlow,
        &totalCost,
        &paths) == 0);

    printf("[PASS] Prototype rejects negative cycle\n");

    printf("\nNegative-cycle safety tests passed!\n");

    return 0;
}
