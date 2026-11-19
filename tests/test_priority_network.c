
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/*
 * Temporary white-box test.
 */
#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};
    PriorityNetwork network = {0};

    graph.nodeCount = 3;
    graph.edgeCount = 2;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 60.0;

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

    /* Active line */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 30.0;
    graph.edges[0].active = 1;

    /* Failed line */
    graph.edges[1].from = 10;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 15.0;
    graph.edges[1].active = 0;

    Graph original = graph;

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    assert(network.vertexCount == 5);
    assert(network.sourceIndex == 3);
    assert(network.sinkIndex == 4);

    printf("[PASS] Network vertex construction\n");

    /*
     * 1 active transmission line = 2 arcs
     * 1 generator connection = 2 arcs
     * 2 consumer connections = 4 arcs
     */
    assert(network.arcCount == 8);

    printf("[PASS] Failed transmission line excluded\n");

    assert(network.arcs[0].capacity == 30.0);
    assert(network.arcs[1].capacity == 30.0);

    printf("[PASS] Shared transmission capacity\n");

    /*
     * Consumer-to-sink arcs.
     * Arc 4 = Hospital forward arc.
     * Arc 6 = School forward arc.
     */
    assert(network.arcs[4].from == 1);
    assert(network.arcs[4].to == 4);
    assert(network.arcs[4].capacity == 20.0);

    assert(network.arcs[6].from == 2);
    assert(network.arcs[6].to == 4);
    assert(network.arcs[6].capacity == 10.0);

    assert(network.arcs[4].cost.component[0] == -1.0);
    assert(network.arcs[6].cost.component[1] == -1.0);

    printf("[PASS] Consumer priority costs\n");

    assert(memcmp(
        &graph, &original, sizeof(Graph)) == 0);

    printf("[PASS] Original graph preserved\n");

    printf("\nAll priority network tests passed!\n");

    return 0;
}
