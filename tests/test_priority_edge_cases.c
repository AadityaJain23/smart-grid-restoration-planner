
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 3;
    graph.edgeCount = 0;

    /*
     * Generator A is disconnected.
     */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 50.0;

    /*
     * Zero-demand consumer.
     */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = 0.0;
    graph.nodes[1].priority = 1;

    /*
     * Disconnected consumer.
     */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 20.0;
    graph.nodes[2].priority = 2;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    printf("[PASS] Disconnected network constructed\n");

    double maxFlow = -1.0;
    int paths = -1;

    assert(runPriorityMaximumFlow(
        &network, &maxFlow, &paths) == 1);

    assert(fabs(maxFlow) < FLOW_EPS);
    assert(paths == 0);

    printf("[PASS] Disconnected generator handled\n");

    assert(validateFlowConservation(
        &graph, &network) == 1);

    printf("[PASS] Zero-flow conservation\n");

    PriorityFlowResult result = {0};

    assert(extractPriorityFlowResult(
        &graph, &network, paths, &result) == 1);

    assert(fabs(result.deliveredToNode[1]) < FLOW_EPS);
    assert(fabs(result.deliveredToNode[2]) < FLOW_EPS);

    assert(fabs(result.totalUnserved - 20.0)
           < FLOW_EPS);

    printf("[PASS] Zero-demand consumer handled\n");
    printf("[PASS] Unserved demand calculated\n");

    /*
     * Invalid negative demand.
     */
    Graph invalid = graph;
    invalid.nodes[2].demand = -5.0;

    assert(buildPriorityNetwork(
        &invalid, &network) == 0);

    printf("[PASS] Negative demand rejected\n");

    /*
     * Invalid nonfinite generator supply.
     */
    invalid = graph;
    invalid.nodes[0].supply = NAN;

    assert(buildPriorityNetwork(
        &invalid, &network) == 0);

    printf("[PASS] Nonfinite supply rejected\n");

    printf("\nAll edge-case tests passed!\n");

    return 0;
}
