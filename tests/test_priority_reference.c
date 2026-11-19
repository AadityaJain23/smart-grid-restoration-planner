
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "priority_flow.h"

/*
 * Independent reference solver for a
 * small star-shaped network.
 *
 * All capacities and demands are integers.
 * This is NOT the production algorithm.
 */

typedef struct {
    int p1;
    int p2;
    int p3;
} ReferenceAllocation;

static int lexBetter(
    ReferenceAllocation a,
    ReferenceAllocation b
)
{
    if (a.p1 != b.p1) {
        return a.p1 > b.p1;
    }

    if (a.p2 != b.p2) {
        return a.p2 > b.p2;
    }

    return a.p3 > b.p3;
}

/*
 * Enumerate every possible consumer
 * allocation within demand limits.
 */
static ReferenceAllocation exhaustiveReference(
    int supply,
    int demand1,
    int demand2,
    int demand3
)
{
    ReferenceAllocation best = {0, 0, 0};

    for (int p1 = 0; p1 <= demand1; p1++) {

        for (int p2 = 0; p2 <= demand2; p2++) {

            for (int p3 = 0; p3 <= demand3; p3++) {

                if (p1 + p2 + p3 > supply) {
                    continue;
                }

                ReferenceAllocation candidate = {
                    p1, p2, p3
                };

                if (lexBetter(candidate, best)) {
                    best = candidate;
                }
            }
        }
    }

    return best;
}

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

int main(void)
{
    /*
     * Independent exhaustive calculation.
     */
    ReferenceAllocation reference =
        exhaustiveReference(8, 5, 4, 6);

    assert(reference.p1 == 5);
    assert(reference.p2 == 3);
    assert(reference.p3 == 0);

    printf("[PASS] Independent reference optimum\n");

    /*
     * Build equivalent graph.
     */
    static Graph graph;
    static PriorityFlowResult result;

    memset(&graph, 0, sizeof(graph));

    graph.nodeCount = 5;
    graph.edgeCount = 4;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 8.0;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 5.0;
    graph.nodes[2].priority = 1;

    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 4.0;
    graph.nodes[3].priority = 2;

    graph.nodes[4].id = 50;
    graph.nodes[4].type = CONSUMER;
    graph.nodes[4].demand = 6.0;
    graph.nodes[4].priority = 3;

    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 8.0;
    graph.edges[0].active = 1;

    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 5.0;
    graph.edges[1].active = 1;

    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 4.0;
    graph.edges[2].active = 1;

    graph.edges[3].from = 20;
    graph.edges[3].to = 50;
    graph.edges[3].capacity = 6.0;
    graph.edges[3].active = 1;

    assert(calculatePriorityFlow(
        &graph, &result) == 1);

    printf("[PASS] Production solver completed\n");

    /*
     * Compare production result with
     * independent reference optimum.
     */
    assert(closeMW(
        result.deliveredByPriority[0],
        reference.p1));

    assert(closeMW(
        result.deliveredByPriority[1],
        reference.p2));

    assert(closeMW(
        result.deliveredByPriority[2],
        reference.p3));

    printf("[PASS] Production matches reference\n");

    assert(closeMW(result.totalDelivered, 8.0));
    assert(closeMW(result.totalUnserved, 7.0));

    printf("[PASS] Total delivery accounting\n");

    printf("\nReference optimum: (%d, %d, %d)\n",
           reference.p1,
           reference.p2,
           reference.p3);

    printf("Production result: (%.2f, %.2f, %.2f)\n",
           result.deliveredByPriority[0],
           result.deliveredByPriority[1],
           result.deliveredByPriority[2]);

    printf("\nAll reference comparison tests passed!\n");

    return 0;
}
