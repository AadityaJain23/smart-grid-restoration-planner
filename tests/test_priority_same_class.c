
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

    graph.nodeCount = 6;
    graph.edgeCount = 5;

    /* Generator A */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 30.0;

    /* Generator B */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = GENERATOR;
    graph.nodes[1].supply = 20.0;

    /* Shared Hospital Substation */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = SUBSTATION;

    /* Hospital A */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 1;

    /* Hospital B */
    graph.nodes[4].id = 50;
    graph.nodes[4].type = CONSUMER;
    graph.nodes[4].demand = 20.0;
    graph.nodes[4].priority = 1;

    /* School */
    graph.nodes[5].id = 60;
    graph.nodes[5].type = CONSUMER;
    graph.nodes[5].demand = 20.0;
    graph.nodes[5].priority = 2;

    /*
     * Generator A -> Shared Hospital Substation
     * This is the 20 MW P1 bottleneck.
     */
    graph.edges[0].from = 10;
    graph.edges[0].to = 30;
    graph.edges[0].capacity = 20.0;
    graph.edges[0].active = 1;

    /* Shared Substation -> Hospital A */
    graph.edges[1].from = 30;
    graph.edges[1].to = 40;
    graph.edges[1].capacity = 20.0;
    graph.edges[1].active = 1;

    /* Shared Substation -> Hospital B */
    graph.edges[2].from = 30;
    graph.edges[2].to = 50;
    graph.edges[2].capacity = 20.0;
    graph.edges[2].active = 1;

    /*
     * Generator A -> School
     */
    graph.edges[3].from = 10;
    graph.edges[3].to = 60;
    graph.edges[3].capacity = 10.0;
    graph.edges[3].active = 1;

    /*
     * Generator B -> School
     */
    graph.edges[4].from = 20;
    graph.edges[4].to = 60;
    graph.edges[4].capacity = 20.0;
    graph.edges[4].active = 1;

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    printf("[PASS] Same-priority network optimized\n");

    double hospitalA = result.deliveredToNode[3];
    double hospitalB = result.deliveredToNode[4];
    double school = result.deliveredToNode[5];

    printf("\nFinal Allocations:\n");
    printf("Hospital A: %.2f MW\n", hospitalA);
    printf("Hospital B: %.2f MW\n", hospitalB);
    printf("School    : %.2f MW\n", school);

    /*
     * Shared Hospital bottleneck:
     * P1 maximum = 20 MW.
     */
    assert(closeMW(
        hospitalA + hospitalB, 20.0));

    assert(closeMW(
        result.deliveredByPriority[0], 20.0));

    printf("[PASS] Maximum P1 total preserved\n");

    /*
     * School should receive its
     * full 20 MW demand.
     */
    assert(closeMW(school, 20.0));

    assert(closeMW(
        result.deliveredByPriority[1], 20.0));

    printf("[PASS] Priority 2 maximized\n");

    assert(closeMW(
        result.totalDelivered, 40.0));

    assert(closeMW(
        result.totalUnserved, 20.0));

    printf("[PASS] Total delivery and demand accounting\n");

    /*
     * Individual Hospital deliveries
     * must respect demand constraints.
     */
    assert(hospitalA >= -1e-7);
    assert(hospitalA <= 20.0 + 1e-7);

    assert(hospitalB >= -1e-7);
    assert(hospitalB <= 20.0 + 1e-7);

    printf("[PASS] Same-priority demand constraints\n");

    printf("\nAll same-priority tests passed!\n");

    return 0;
}
