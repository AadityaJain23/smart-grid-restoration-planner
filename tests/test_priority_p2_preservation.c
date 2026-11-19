
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
    graph.nodes[0].supply = 50.0;

    /* Substation */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    /*
     * Deliberately place Industry first.
     * BFS may initially favor this node.
     */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 30.0;
    graph.nodes[2].priority = 3;

    /* School - Priority 2 */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 2;

    /* Hospital - Priority 1 */
    graph.nodes[4].id = 50;
    graph.nodes[4].type = CONSUMER;
    graph.nodes[4].demand = 20.0;
    graph.nodes[4].priority = 1;

    /* Generator -> Substation */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 50.0;
    graph.edges[0].active = 1;

    /* Substation -> Industry */
    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 30.0;
    graph.edges[1].active = 1;

    /* Substation -> School */
    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 20.0;
    graph.edges[2].active = 1;

    /* Substation -> Hospital */
    graph.edges[3].from = 20;
    graph.edges[3].to = 50;
    graph.edges[3].capacity = 20.0;
    graph.edges[3].active = 1;

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    printf("[PASS] Three-priority network optimized\n");

    printf("\nFinal Allocations:\n");
    printf("Hospital : %.2f MW\n",
           result.deliveredToNode[4]);
    printf("School   : %.2f MW\n",
           result.deliveredToNode[3]);
    printf("Industry : %.2f MW\n",
           result.deliveredToNode[2]);

    /*
     * Priority 1 must remain maximized.
     */
    assert(closeMW(
        result.deliveredByPriority[0], 20.0));

    printf("[PASS] Priority 1 preserved\n");

    /*
     * Priority 2 must receive its
     * maximum possible 20 MW.
     */
    assert(closeMW(
        result.deliveredByPriority[1], 20.0));

    printf("[PASS] Priority 2 maximized\n");

    /*
     * Priority 3 receives remaining supply.
     */
    assert(closeMW(
        result.deliveredByPriority[2], 10.0));

    printf("[PASS] Priority 3 receives remaining power\n");

    /*
     * Total throughput remains 50 MW.
     */
    assert(closeMW(
        result.totalDelivered, 50.0));

    assert(closeMW(
        result.totalUnserved, 20.0));

    printf("[PASS] Total delivery and demand accounting\n");

    printf("\nAll Priority 2 preservation tests passed!\n");

    return 0;
}

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
    graph.nodes[0].supply = 50.0;

    /* Substation */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    /*
     * Deliberately place Industry first.
     * BFS may initially favor this node.
     */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 30.0;
    graph.nodes[2].priority = 3;

    /* School - Priority 2 */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 2;

    /* Hospital - Priority 1 */
    graph.nodes[4].id = 50;
    graph.nodes[4].type = CONSUMER;
    graph.nodes[4].demand = 20.0;
    graph.nodes[4].priority = 1;

    /* Generator -> Substation */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 50.0;
    graph.edges[0].active = 1;

    /* Substation -> Industry */
    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 30.0;
    graph.edges[1].active = 1;

    /* Substation -> School */
    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 20.0;
    graph.edges[2].active = 1;

    /* Substation -> Hospital */
    graph.edges[3].from = 20;
    graph.edges[3].to = 50;
    graph.edges[3].capacity = 20.0;
    graph.edges[3].active = 1;

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    printf("[PASS] Three-priority network optimized\n");

    printf("\nFinal Allocations:\n");
    printf("Hospital : %.2f MW\n",
           result.deliveredToNode[4]);
    printf("School   : %.2f MW\n",
           result.deliveredToNode[3]);
    printf("Industry : %.2f MW\n",
           result.deliveredToNode[2]);

    /*
     * Priority 1 must remain maximized.
     */
    assert(closeMW(
        result.deliveredByPriority[0], 20.0));

    printf("[PASS] Priority 1 preserved\n");

    /*
     * Priority 2 must receive its
     * maximum possible 20 MW.
     */
    assert(closeMW(
        result.deliveredByPriority[1], 20.0));

    printf("[PASS] Priority 2 maximized\n");

    /*
     * Priority 3 receives remaining supply.
     */
    assert(closeMW(
        result.deliveredByPriority[2], 10.0));

    printf("[PASS] Priority 3 receives remaining power\n");

    /*
     * Total throughput remains 50 MW.
     */
    assert(closeMW(
        result.totalDelivered, 50.0));

    assert(closeMW(
        result.totalUnserved, 20.0));

    printf("[PASS] Total delivery and demand accounting\n");

    printf("\nAll Priority 2 preservation tests passed!\n");

    return 0;
}
