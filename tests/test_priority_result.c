
#include <assert.h>
#include <math.h>
#include <stdio.h>
#define PRIORITY_FLOW_LEGACY_TESTS
#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 3;
    graph.edgeCount = 2;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 30.0;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = 20.0;
    graph.nodes[1].priority = 1;

    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 10.0;
    graph.nodes[2].priority = 2;

    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 20.0;
    graph.edges[0].active = 1;

    graph.edges[1].from = 10;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 10.0;
    graph.edges[1].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    int parent[MAX_FLOW_VERTICES];
    PriorityCost dist[MAX_FLOW_VERTICES];

    int paths = 0;

    while (1) {

        int status = bellmanFordPriority(
            &network, parent, dist);

        assert(status >= 0);

        if (status == 0) {
            break;
        }

        double delta = 0.0;
        PriorityCost cost = zeroPriorityCost();

        assert(augmentPriorityPath(
            &network, parent, &delta, &cost) == 1);

        paths++;
        assert(paths <= 3);
    }

    PriorityFlowResult result = {0};

    assert(extractPriorityFlowResult(
        &graph, &network, paths, &result) == 1);

    assert(fabs(result.totalSupply - 30.0) < FLOW_EPS);
    assert(fabs(result.totalDemand - 30.0) < FLOW_EPS);
    assert(fabs(result.totalDelivered - 30.0) < FLOW_EPS);
    assert(fabs(result.totalUnserved) < FLOW_EPS);

    printf("[PASS] Total supply and demand\n");
    printf("[PASS] Total delivered and unserved\n");

    assert(fabs(result.deliveredToNode[1] - 20.0)
           < FLOW_EPS);

    assert(fabs(result.deliveredToNode[2] - 10.0)
           < FLOW_EPS);

    printf("[PASS] Consumer allocations\n");

    assert(fabs(result.deliveredByPriority[0] - 20.0)
           < FLOW_EPS);

    assert(fabs(result.deliveredByPriority[1] - 10.0)
           < FLOW_EPS);

    assert(fabs(result.deliveredByPriority[2])
           < FLOW_EPS);

    printf("[PASS] Priority totals\n");

    assert(result.augmentingPaths == paths);

    printf("[PASS] Augmenting path count\n");

    printf("\nAll priority result tests passed!\n");

    return 0;
}
