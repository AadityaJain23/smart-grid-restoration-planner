
#include <assert.h>
#include <math.h>
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

    /*
     * Source -> Generator
     */
    assert(addPriorityArcPair(
        &network, 0, 1,
        60.0, 0.0, zero) == 1);

    /*
     * Generator -> Hospital
     */
    assert(addPriorityArcPair(
        &network, 1, 2,
        30.0, 0.0, zero) == 1);

    /*
     * Hospital -> Sink
     */
    assert(addPriorityArcPair(
        &network, 2, 3,
        20.0, 0.0, p1) == 1);

    int parentArc[MAX_FLOW_VERTICES];
    PriorityCost dist[MAX_FLOW_VERTICES];

    assert(bellmanFordPriority(
        &network, parentArc, dist) == 1);

    printf("[PASS] Augmenting path found\n");

    double flow = 0.0;
    PriorityCost deltaCost =
        zeroPriorityCost();

    assert(augmentPriorityPath(
        &network,
        parentArc,
        &flow,
        &deltaCost) == 1);

    assert(fabs(flow - 20.0) < FLOW_EPS);

    printf("[PASS] Bottleneck calculation\n");

    /*
     * Forward residual capacities.
     */
    assert(network.arcs[0].capacity == 40.0);
    assert(network.arcs[2].capacity == 10.0);
    assert(network.arcs[4].capacity == 0.0);

    printf("[PASS] Forward capacity updates\n");

    /*
     * Reverse residual capacities.
     */
    assert(network.arcs[1].capacity == 20.0);
    assert(network.arcs[3].capacity == 20.0);
    assert(network.arcs[5].capacity == 20.0);

    printf("[PASS] Reverse capacity updates\n");

    /*
     * Priority 1 cost change.
     */
    assert(deltaCost.component[0] == -20.0);
    assert(deltaCost.component[1] == 0.0);
    assert(deltaCost.component[2] == 0.0);

    printf("[PASS] Lexicographic cost update\n");

    /*
     * The sink edge is now saturated.
     */
    assert(bellmanFordPriority(
        &network, parentArc, dist) == 0);

    printf("[PASS] Saturated sink detected\n");

    /*
     * Invalid parent chain must fail
     * without changing the network.
     */
    PriorityNetwork before = network;

    int invalidParent[MAX_FLOW_VERTICES];

    for (int i = 0; i < MAX_FLOW_VERTICES; i++) {
        invalidParent[i] = -1;
    }

    assert(augmentPriorityPath(
        &network,
        invalidParent,
        &flow,
        &deltaCost) == 0);

    /*
     * Verify relevant capacities unchanged.
     */
    for (int i = 0; i < network.arcCount; i++) {
        assert(network.arcs[i].capacity ==
               before.arcs[i].capacity);
    }

    printf("[PASS] Invalid path preserves network\n");

    printf("\nAll augmentation tests passed!\n");

    return 0;
}
