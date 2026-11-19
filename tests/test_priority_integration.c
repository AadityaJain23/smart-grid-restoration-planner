
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

    printf("[PASS] Priority network constructed\n");

    int parentArc[MAX_FLOW_VERTICES];
    PriorityCost dist[MAX_FLOW_VERTICES];

    double totalFlow = 0.0;
    int paths = 0;

    /*
     * This simple network has no competing
     * routes or negative residual cycles.
     */
    while (1) {

        int status = bellmanFordPriority(
            &network, parentArc, dist);

        assert(status >= 0);

        if (status == 0) {
            break;
        }

        assert(comparePriorityCost(
            dist[network.sinkIndex],
            zeroPriorityCost()) < 0);

        double delta = 0.0;
        PriorityCost deltaCost =
            zeroPriorityCost();

        assert(augmentPriorityPath(
            &network,
            parentArc,
            &delta,
            &deltaCost) == 1);

        assert(delta > FLOW_EPS);

        totalFlow += delta;
        paths++;

        assert(paths <= 3);
    }

    assert(fabs(totalFlow - 30.0) < FLOW_EPS);

    printf("[PASS] Total delivery = 30 MW\n");

    /*
     * Find the consumer-to-sink forward arcs.
     * Do not assume fixed arc indices.
     */
    double delivered[MAX_NODES] = {0};

    for (int e = 0; e < network.arcCount; e++) {

        const PriorityArc *arc =
            &network.arcs[e];

        if (arc->to != network.sinkIndex) {
            continue;
        }

        int i = arc->from;

        if (i < 0 || i >= graph.nodeCount ||
            graph.nodes[i].type != CONSUMER) {
            continue;
        }

        double demand = graph.nodes[i].demand;

        delivered[i] = demand - arc->capacity;
    }

    assert(fabs(delivered[1] - 20.0) < FLOW_EPS);
    assert(fabs(delivered[2] - 10.0) < FLOW_EPS);

    printf("[PASS] Hospital receives 20 MW\n");
    printf("[PASS] School receives 10 MW\n");

    int cycleArcs[MAX_FLOW_VERTICES];
    int cycleLength = 0;

    assert(findNegativePriorityCycle(
        &network,
        cycleArcs,
        &cycleLength) == 0);

    printf("[PASS] No improving residual cycle\n");

    printf("\nAll integration tests passed!\n");

    return 0;
}
