
#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "priority_flow.h"

int main(void)
{
    static Graph graph;
    static PriorityFlowResult result;

    memset(&graph, 0, sizeof(graph));
    memset(&result, 0, sizeof(result));

    graph.nodeCount = 3;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 30.0;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].priority = 1;
    graph.nodes[1].demand = 20.0;

    strcpy(graph.nodes[1].name, "Hospital");

    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].priority = 2;
    graph.nodes[2].demand = 10.0;

    strcpy(graph.nodes[2].name, "School");

    result.totalSupply = 30.0;
    result.totalDemand = 30.0;
    result.totalDelivered = 30.0;
    result.totalUnserved = 0.0;

    result.deliveredByPriority[0] = 20.0;
    result.deliveredByPriority[1] = 10.0;

    result.deliveredToNode[1] = 20.0;
    result.deliveredToNode[2] = 10.0;

    result.augmentingPaths = 2;

    /*
     * Verify that reporting does not
     * modify the result structure.
     */
    PriorityFlowResult before = result;

    displayPriorityFlowReport(&graph, &result);

    assert(memcmp(
        &before,
        &result,
        sizeof(result)) == 0);

    printf("\n[PASS] Priority Flow report completed\n");
    printf("[PASS] Result preserved after reporting\n");

    /*
     * Null pointers should be handled
     * without crashing.
     */
    displayPriorityFlowReport(NULL, &result);
    displayPriorityFlowReport(&graph, NULL);

    printf("[PASS] Null report inputs handled\n");

    printf("\nAll Priority Flow report tests passed!\n");

    return 0;
}
