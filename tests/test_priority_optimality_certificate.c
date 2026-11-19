
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        FLOW_EPS * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 4;
    graph.edgeCount = 3;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 20.0;

    /* Substation */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    /*
     * Industry is inserted before Hospital.
     */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 20.0;
    graph.nodes[2].priority = 3;

    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 1;

    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 20.0;
    graph.edges[0].active = 1;

    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 20.0;
    graph.edges[1].active = 1;

    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 20.0;
    graph.edges[2].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    double maximumFlow = 0.0;
    int paths = 0;

    assert(runPriorityMaximumFlow(
        &network, &maximumFlow, &paths) == 1);

    assert(closeMW(maximumFlow, 20.0));

    printf("[PASS] Maximum flow constructed\n");

    /*
     * The initial BFS allocation should
     * serve Industry first.
     */
    PriorityFlowResult before = {0};

    assert(extractPriorityFlowResult(
        &graph, &network, paths, &before) == 1);

    assert(closeMW(
        before.deliveredToNode[2], 20.0));

    printf("[PASS] Initial suboptimal allocation\n");

    /*
     * Verify that an improving negative
     * residual cycle exists.
     */
    int cycleArcs[MAX_FLOW_VERTICES];
    int cycleLength = 0;

    assert(findNegativePriorityCycle(
        &network,
        cycleArcs,
        &cycleLength) == 1);

    assert(cycleLength > 0);

    printf("[PASS] Improving residual cycle detected\n");

    /*
     * Optimize the allocation.
     */
    int cyclesCanceled = 0;

    PriorityCost costChange =
        zeroPriorityCost();

    assert(cancelNegativePriorityCycles(
        &network,
        100,
        &cyclesCanceled,
        &costChange) == 1);

    assert(cyclesCanceled >= 1);

    printf("[PASS] Negative-cycle optimization\n");

    /*
     * Verify no improving cycle remains.
     */
    assert(findNegativePriorityCycle(
        &network,
        cycleArcs,
        &cycleLength) == 0);

    printf("[PASS] No negative residual cycle remains\n");

    /*
     * Verify final priority allocation.
     */
    PriorityFlowResult after = {0};

    assert(extractPriorityFlowResult(
        &graph, &network, paths, &after) == 1);

    assert(closeMW(
        after.deliveredToNode[3], 20.0));

    assert(closeMW(
        after.deliveredToNode[2], 0.0));

    assert(closeMW(
        after.totalDelivered, 20.0));

    printf("[PASS] Lexicographic allocation verified\n");

    /*
     * Verify feasibility.
     */
    assert(validateFlowConservation(
        &graph, &network) == 1);

    printf("[PASS] Flow conservation verified\n");

    /*
     * Repeating optimization should
     * not change an optimal network.
     */
    cyclesCanceled = -1;

    assert(cancelNegativePriorityCycles(
        &network,
        100,
        &cyclesCanceled,
        &costChange) == 1);

    assert(cyclesCanceled == 0);

    printf("[PASS] Optimization is stable\n");

    printf("\nAll optimality certificate tests passed!\n");

    return 0;
}
