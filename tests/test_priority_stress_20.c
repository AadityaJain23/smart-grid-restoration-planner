
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "priority_flow.h"
#include "maxflow.h"

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    static Graph graph;
    static Graph original;
    static PriorityFlowResult result;
    static MaxFlowResult baseline;

    memset(&graph, 0, sizeof(graph));

    graph.nodeCount = 20;

    /*
     * Three generators.
     */
    for (int i = 0; i < 3; i++) {
        graph.nodes[i].id = 10 + i;
        graph.nodes[i].type = GENERATOR;
    }

    graph.nodes[0].supply = 50.0;
    graph.nodes[1].supply = 40.0;
    graph.nodes[2].supply = 30.0;

    /*
     * Five substations.
     */
    for (int i = 3; i < 8; i++) {
        graph.nodes[i].id = 20 + (i - 3);
        graph.nodes[i].type = SUBSTATION;
    }

    /*
     * Twelve consumers.
     */
    for (int i = 8; i < 20; i++) {
        graph.nodes[i].id = 30 + (i - 8);
        graph.nodes[i].type = CONSUMER;

        if (i < 11) {
            graph.nodes[i].priority = 1;
            graph.nodes[i].demand = 15.0;
        }
        else if (i < 15) {
            graph.nodes[i].priority = 2;
            graph.nodes[i].demand = 10.0;
        }
        else {
            graph.nodes[i].priority = 3;
            graph.nodes[i].demand = 15.0;
        }
    }

    /*
     * Helper macro for test edges.
     * All physical lines are active.
     */
#define ADD_EDGE(FROM, TO, CAP) do {       \
    int e = graph.edgeCount++;             \
    graph.edges[e].from = (FROM);          \
    graph.edges[e].to = (TO);              \
    graph.edges[e].capacity = (CAP);       \
    graph.edges[e].active = 1;             \
} while (0)

    /*
     * Generators to substations.
     */
    ADD_EDGE(10, 20, 50.0);
    ADD_EDGE(11, 21, 40.0);
    ADD_EDGE(12, 22, 30.0);

    /*
     * Redundant substation backbone.
     */
    ADD_EDGE(20, 21, 60.0);
    ADD_EDGE(21, 22, 60.0);
    ADD_EDGE(22, 23, 60.0);
    ADD_EDGE(23, 24, 60.0);
    ADD_EDGE(20, 24, 60.0);
    ADD_EDGE(20, 22, 60.0);
    ADD_EDGE(21, 23, 60.0);
    ADD_EDGE(22, 24, 60.0);

    /*
     * Consumer connections.
     * Each consumer connects to one
     * substation with sufficient capacity.
     */
    for (int i = 8; i < 20; i++) {

        int substationId =
            20 + ((i - 8) % 5);

        ADD_EDGE(
            substationId,
            graph.nodes[i].id,
            graph.nodes[i].demand
        );
    }

#undef ADD_EDGE

    /*
     * Total physical lines:
     * 3 generator connections
     * 8 backbone connections
     * 12 consumer connections
     * = 23 edges
     */
    assert(graph.edgeCount == 23);

    original = graph;

    /*
     * Run Priority-Aware Flow Solver.
     */
    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    printf("[PASS] 20-node priority solver completed\n");

    assert(closeMW(
        result.totalSupply, 120.0));

    assert(closeMW(
        result.totalDemand, 160.0));

    printf("[PASS] Supply and demand totals\n");

    assert(closeMW(
        result.totalDelivered, 120.0));

    assert(closeMW(
        result.totalUnserved, 40.0));

    printf("[PASS] Maximum delivery = 120 MW\n");

    assert(closeMW(
        result.deliveredByPriority[0], 45.0));

    assert(closeMW(
        result.deliveredByPriority[1], 40.0));

    assert(closeMW(
        result.deliveredByPriority[2], 35.0));

    printf("[PASS] Lexicographic priority totals\n");

    /*
     * Compare with Week 4 Maximum Flow.
     */
    assert(calculateMaximumFlow(
        &graph, &baseline) == 1);

    assert(closeMW(
        result.totalDelivered,
        baseline.maxFlow));

    printf("[PASS] Week 4 Maximum Flow comparison\n");

    /*
     * Verify original graph unchanged.
     */
    assert(memcmp(
        &graph, &original,
        sizeof(Graph)) == 0);

    printf("[PASS] Original graph preserved\n");

    printf("\nFinal Priority Allocation:\n");
    printf("P1: %.2f MW\n",
           result.deliveredByPriority[0]);
    printf("P2: %.2f MW\n",
           result.deliveredByPriority[1]);
    printf("P3: %.2f MW\n",
           result.deliveredByPriority[2]);

    printf("\nAll 20-node stress tests passed!\n");

    return 0;
}
