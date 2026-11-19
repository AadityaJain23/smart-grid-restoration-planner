
#include <assert.h>
#include <stdio.h>
#define PRIORITY_FLOW_LEGACY_TESTS
#include "../src/priority_flow.c"

int main(void)
{
    PriorityNetwork network = {0};

    network.vertexCount = 4;
    network.sourceIndex = 0;
    network.sinkIndex = 3;

    PriorityCost zero = zeroPriorityCost();
    PriorityCost p1 = makeConsumerCost(1);
    PriorityCost p2 = makeConsumerCost(2);

    /*
     * Two paths:
     *
     * Source -> Hospital -> Sink
     * Source -> School   -> Sink
     *
     * Hospital must be preferred.
     */
    assert(addPriorityArcPair(
        &network, 0, 1,
        20.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 1, 3,
        20.0, 0.0, p1) == 1);

    assert(addPriorityArcPair(
        &network, 0, 2,
        10.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 2, 3,
        10.0, 0.0, p2) == 1);

    int parentArc[MAX_FLOW_VERTICES];
    PriorityCost dist[MAX_FLOW_VERTICES];

    int status = bellmanFordPriority(
        &network, parentArc, dist);

    assert(status == 1);

    printf("[PASS] Sink reachable\n");

    assert(dist[3].component[0] == -1.0);
    assert(dist[3].component[1] == 0.0);

    printf("[PASS] Priority 1 path preferred\n");

    assert(parentArc[3] == 2);

    printf("[PASS] Parent arc reconstruction\n");

    /*
     * Disable the Hospital sink arc.
     */
    network.arcs[2].capacity = 0.0;

    status = bellmanFordPriority(
        &network, parentArc, dist);

    assert(status == 1);

    assert(dist[3].component[0] == 0.0);
    assert(dist[3].component[1] == -1.0);

    printf("[PASS] Priority 2 fallback\n");

    /*
     * Disable School sink arc as well.
     */
    network.arcs[6].capacity = 0.0;

    status = bellmanFordPriority(
        &network, parentArc, dist);

    assert(status == 0);

    printf("[PASS] Unreachable sink detected\n");

    printf("\nAll Bellman-Ford tests passed!\n");

    return 0;
}
