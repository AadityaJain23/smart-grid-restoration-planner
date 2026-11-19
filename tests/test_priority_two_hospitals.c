
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "priority_flow.h"

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    static Graph graph;
    static PriorityFlowResult result;

    memset(&graph, 0, sizeof(graph));

    graph.nodeCount = 5;
    graph.edgeCount = 4;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 30.0;

    /* Substation */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    /* Hospital A */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 20.0;
    graph.nodes[2].priority = 1;

    /* Hospital B */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 1;

    /* Industry */
    graph.nodes[4].id = 50;
    graph.nodes[4].type = CONSUMER;
    graph.nodes[4].demand = 30.0;
    graph.nodes[4].priority = 3;

    /* Generator -> Substation */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 30.0;
    graph.edges[0].active = 1;

    /* Substation -> Hospital A */
    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 20.0;
    graph.edges[1].active = 1;

    /* Substation -> Hospital B */
    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 20.0;
    graph.edges[2].active = 1;

    /* Substation -> Industry */
    graph.edges[3].from = 20;
    graph.edges[3].to = 50;
    graph.edges[3].capacity = 30.0;
    graph.edges[3].active = 1;

    /*
     * Run the complete public optimizer.
     */
    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    printf("[PASS] Two-hospital network optimized\n");

    double hospitalA = result.deliveredToNode[2];
    double hospitalB = result.deliveredToNode[3];
    double industry = result.deliveredToNode[4];

    printf("\nFinal Allocations:\n");
    printf("Hospital A: %.2f MW\n", hospitalA);
    printf("Hospital B: %.2f MW\n", hospitalB);
    printf("Industry  : %.2f MW\n", industry);

    /*
     * Total Priority 1 delivery must
     * equal the available 30 MW.
     */
    assert(closeMW(
        result.deliveredByPriority[0], 30.0));

    printf("[PASS] Maximum Priority 1 delivery\n");

    /*
     * Lower-priority Industry must not
     * receive electricity while Priority 1
     * demand remains unsatisfied.
     */
    assert(closeMW(industry, 0.0));

    printf("[PASS] Lower-priority delivery excluded\n");

    /*
     * Each Hospital must remain within
     * its individual demand.
     */
    assert(hospitalA >= -1e-7);
    assert(hospitalA <= 20.0 + 1e-7);

    assert(hospitalB >= -1e-7);
    assert(hospitalB <= 20.0 + 1e-7);

    assert(closeMW(
        hospitalA + hospitalB, 30.0));

    printf("[PASS] Individual demand constraints\n");

    /*
     * Total delivery must remain 30 MW.
     */
    assert(closeMW(
        result.totalDelivered, 30.0));

    assert(closeMW(
        result.totalUnserved, 40.0));

    printf("[PASS] Total delivery and unserved demand\n");

    printf("\nAll two-hospital tests passed!\n");

    return 0;
}
