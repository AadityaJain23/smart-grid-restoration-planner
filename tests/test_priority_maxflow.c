
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 3;
    graph.edgeCount = 2;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 30.0;

    /* Hospital */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = 20.0;
    graph.nodes[1].priority = 1;

    /* School */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 10.0;
    graph.nodes[2].priority = 2;

    /* Generator -> Hospital */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 20.0;
    graph.edges[0].active = 1;

    /* Generator -> School */
    graph.edges[1].from = 10;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 10.0;
    graph.edges[1].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    printf("[PASS] Priority network built\n");

    double maxFlow = 0.0;
    int paths = 0;

    assert(runPriorityMaximumFlow(
        &network,
        &maxFlow,
        &paths) == 1);

    assert(fabs(maxFlow - 30.0) < FLOW_EPS);

    printf("[PASS] Maximum Flow = 30 MW\n");

    assert(paths == 2);

    printf("[PASS] Two augmenting paths\n");

    /*
     * No more augmenting paths should exist.
     */
    int parentArc[MAX_FLOW_VERTICES];

    assert(bfsPriorityAugmentingPath(
        &network, parentArc) == 0);

    printf("[PASS] No residual augmenting path\n");

    /*
     * Extract consumer allocations.
     */
    PriorityFlowResult result = {0};

    assert(extractPriorityFlowResult(
        &graph,
        &network,
        paths,
        &result) == 1);

    assert(fabs(result.totalDelivered - 30.0)
           < FLOW_EPS);

    assert(fabs(result.deliveredToNode[1] - 20.0)
           < FLOW_EPS);

    assert(fabs(result.deliveredToNode[2] - 10.0)
           < FLOW_EPS);

    printf("[PASS] Consumer allocations correct\n");

    printf("\nAll Priority Maximum Flow tests passed!\n");

    return 0;
}
