
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "priority_flow.h"

#define REPEAT_COUNT 100

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

/*
 * Verify repeated Priority Flow results
 * on a fixed network.
 */
static void testDeterminism(
    const char *filename,
    double expectedTotal,
    double expectedP1,
    double expectedP2,
    double expectedP3
)
{
    static Graph graph;
    static Graph original;

    static PriorityFlowResult reference;
    static PriorityFlowResult current;

    initializeGraph(&graph);

    assert(loadNetworkFromFile(
        &graph, filename) == 1);

    original = graph;

    /*
     * First execution becomes the reference.
     */
    assert(calculatePriorityFlow(
        &graph, &reference) == 1);

    assert(closeMW(
        reference.totalDelivered,
        expectedTotal));

    assert(closeMW(
        reference.deliveredByPriority[0],
        expectedP1));

    assert(closeMW(
        reference.deliveredByPriority[1],
        expectedP2));

    assert(closeMW(
        reference.deliveredByPriority[2],
        expectedP3));

    /*
     * Repeat the exact same calculation.
     */
    for (int run = 0; run < REPEAT_COUNT; run++) {

        memset(&current, 0, sizeof(current));

        assert(calculatePriorityFlow(
            &graph, &current) == 1);

        /*
         * Total delivery must match.
         */
        assert(closeMW(
            current.totalDelivered,
            reference.totalDelivered));

        /*
         * Supply and demand totals.
         */
        assert(closeMW(
            current.totalSupply,
            reference.totalSupply));

        assert(closeMW(
            current.totalDemand,
            reference.totalDemand));

        assert(closeMW(
            current.totalUnserved,
            reference.totalUnserved));

        /*
         * Priority totals must match.
         */
        for (int p = 0;
             p < PRIORITY_LEVELS;
             p++) {

            assert(closeMW(
                current.deliveredByPriority[p],
                reference.deliveredByPriority[p]));
        }

        /*
         * Individual consumer allocations.
         */
        for (int i = 0; i < graph.nodeCount; i++) {

            assert(closeMW(
                current.deliveredToNode[i],
                reference.deliveredToNode[i]));
        }

        /*
         * Augmenting-path count.
         */
        assert(current.augmentingPaths ==
               reference.augmentingPaths);

        /*
         * Original graph must remain unchanged.
         */
        assert(memcmp(
            &graph,
            &original,
            sizeof(Graph)) == 0);
    }

    printf("[PASS] %s: %d repeated runs\n",
           filename, REPEAT_COUNT);

    printf("Total Delivered: %.2f MW\n",
           reference.totalDelivered);

    printf("P1: %.2f MW\n",
           reference.deliveredByPriority[0]);

    printf("P2: %.2f MW\n",
           reference.deliveredByPriority[1]);

    printf("P3: %.2f MW\n",
           reference.deliveredByPriority[2]);

    printf("Augmenting Paths: %d\n",
           reference.augmentingPaths);
}

int main(void)
{
    printf("\n=== Original Network ===\n");

    testDeterminism(
        "data/network.txt",
        100.0, 20.0, 10.0, 70.0);

    printf("\n=== Limited Network ===\n");

    testDeterminism(
        "data/network_60mw.txt",
        60.0, 20.0, 10.0, 30.0);

    printf("\nAll determinism tests passed!\n");

    return 0;
}
