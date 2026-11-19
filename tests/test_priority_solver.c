
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    PriorityNetwork network = {0};

    network.vertexCount = 4;
    network.sourceIndex = 0;
    network.sinkIndex = 3;

    PriorityCost zero = zeroPriorityCost();

    /*
     * Source -> Generator
     * Total supply: 30 MW
     */
    assert(addPriorityArcPair(
        &network, 0, 1,
        30.0, 0.0, zero) == 1);

    /*
     * Generator -> Hospital
     * Hospital demand: 20 MW
     */
    assert(addPriorityArcPair(
        &network, 1, 2,
        20.0, 0.0, zero) == 1);

    assert(addPriorityArcPair(
        &network, 2, 3,
        20.0, 0.0,
        makeConsumerCost(1)) == 1);

    double totalFlow = 0.0;
    PriorityCost totalCost =
        zeroPriorityCost();

    int paths = 0;

    assert(runPriorityFlowPrototype(
        &network,
        &totalFlow,
        &totalCost,
        &paths) == 1);

    assert(fabs(totalFlow - 20.0) < FLOW_EPS);

    printf("[PASS] Total delivered power\n");

    assert(fabs(
        totalCost.component[0] + 20.0
    ) < FLOW_EPS);

    assert(totalCost.component[1] == 0.0);
    assert(totalCost.component[2] == 0.0);

    printf("[PASS] Priority 1 cost accumulation\n");

    assert(paths == 1);

    printf("[PASS] Augmenting path count\n");

    /*
     * Hospital sink capacity is saturated.
     */
    assert(network.arcs[4].capacity == 0.0);
    assert(network.arcs[5].capacity == 20.0);

    printf("[PASS] Residual capacity updates\n");

    printf("\nAll prototype solver tests passed!\n");

    return 0;
}

