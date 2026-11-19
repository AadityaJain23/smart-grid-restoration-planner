
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "../src/priority_flow.c"

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    static Graph graph;
    static PriorityNetwork network;
    static PriorityFlowResult result;

    memset(&graph, 0, sizeof(graph));

    graph.nodeCount = 4;
    graph.edgeCount = 3;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 20.0;

    /* Substation */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    /* Industry - Priority 3 */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 20.0;
    graph.nodes[2].priority = 3;

    /* Hospital - Priority 1 */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 1;

    /* Generator -> Substation */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 20.0;
    graph.edges[0].active = 1;

    /* Substation -> Industry */
    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 20.0;
    graph.edges[1].active = 1;

    /* Substation -> Hospital */
    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 20.0;
    graph.edges[2].active = 1;

    /*
     * Build the residual network.
     */
    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    printf("[PASS] Residual network constructed\n");

    /*
     * Phase 1: Maximum Flow.
     */
    double maximumFlow = 0.0;
    int augmentingPaths = 0;

    assert(runPriorityMaximumFlow(
        &network,
        &maximumFlow,
        &augmentingPaths) == 1);

    assert(closeMW(maximumFlow, 20.0));
    assert(augmentingPaths >= 1);

    printf("[PASS] Maximum Flow completed\n");
    printf("Augmenting paths: %d\n",
           augmentingPaths);

    /*
     * Phase 2: Priority optimization.
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

    printf("[PASS] Priority optimization completed\n");
    printf("Negative cycles canceled: %d\n",
           cyclesCanceled);

    /*
     * Verify no improving cycle remains.
     */
    int cycleArcs[MAX_FLOW_VERTICES];
    int cycleLength = 0;

    assert(findNegativePriorityCycle(
        &network,
        cycleArcs,
        &cycleLength) == 0);

    printf("[PASS] No improving cycle remains\n");

    /*
     * Verify final feasibility.
     */
    assert(validateFlowConservation(
        &graph, &network) == 1);

    printf("[PASS] Flow conservation verified\n");

    /*
     * Extract final allocation.
     */
    assert(extractPriorityFlowResult(
        &graph,
        &network,
        augmentingPaths,
        &result) == 1);

    assert(closeMW(
        result.deliveredByPriority[0], 20.0));

    assert(closeMW(
        result.deliveredByPriority[2], 0.0));

    assert(closeMW(
        result.totalDelivered, 20.0));

    printf("[PASS] Final priority allocation\n");

    printf("\n========== ITERATION SUMMARY ==========\n");
    printf("Vertices             : %d\n",
           network.vertexCount);
    printf("Residual arcs        : %d\n",
           network.arcCount);
    printf("Maximum Flow         : %.2f MW\n",
           maximumFlow);
    printf("Augmenting paths     : %d\n",
           augmentingPaths);
    printf("Cycles canceled      : %d\n",
           cyclesCanceled);
    printf("=======================================\n");

    printf("\nAll iteration-count tests passed!\n");

    return 0;
}
