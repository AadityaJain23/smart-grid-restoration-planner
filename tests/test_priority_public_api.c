
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "priority_flow.h"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 3;
    graph.edgeCount = 2;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 30.0;

    /* Hospital - Priority 1 */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = 20.0;
    graph.nodes[1].priority = 1;

    /* School - Priority 2 */
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

    PriorityFlowResult result = {0};

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    printf("[PASS] Public priority flow API\n");

    assert(fabs(result.totalDelivered - 30.0)
           < FLOW_EPS);

    printf("[PASS] Total delivery = 30 MW\n");

    assert(fabs(result.deliveredToNode[1] - 20.0)
           < FLOW_EPS);

    printf("[PASS] Hospital receives 20 MW\n");

    assert(fabs(result.deliveredToNode[2] - 10.0)
           < FLOW_EPS);

    printf("[PASS] School receives 10 MW\n");

    assert(fabs(result.deliveredByPriority[0] - 20.0)
           < FLOW_EPS);

    assert(fabs(result.deliveredByPriority[1] - 10.0)
           < FLOW_EPS);

    assert(fabs(result.totalUnserved)
           < FLOW_EPS);

    printf("[PASS] Priority totals and unserved demand\n");

    printf("\nAll public API tests passed!\n");

    return 0;
}
