
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "graph.h"
#include "maxflow.h"
#include "priority_flow.h"

#define BENCH_BATCHES 5
#define BENCH_REPEATS 100

static Graph graph;
static MaxFlowResult maxResult;
static PriorityFlowResult priorityResult;

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

static int addBenchmarkEdge(
    Graph *g,
    int from,
    int to,
    double capacity
)
{
    if (g->edgeCount >= MAX_EDGES) {
        return 0;
    }

    Edge *edge = &g->edges[g->edgeCount++];

    edge->from = from;
    edge->to = to;
    edge->capacity = capacity;
    edge->active = 1;

    return 1;
}

/*
 * Build a deterministic smart-grid network.
 *
 * Node IDs are deliberately different
 * from their array indices.
 */
static int buildBenchmarkGraph(
    Graph *g,
    int generators,
    int substations,
    int consumers,
    int dense
)
{
    memset(g, 0, sizeof(*g));

    int total = generators +
                substations +
                consumers;

    if (total > MAX_NODES ||
        generators < 1 ||
        substations < 1 ||
        consumers < 1) {
        return 0;
    }

    g->nodeCount = total;

    /*
     * Generators.
     */
    for (int i = 0; i < generators; i++) {
        g->nodes[i].id = 100 + i;
        g->nodes[i].type = GENERATOR;
        g->nodes[i].supply = 30.0;
    }

    /*
     * Substations.
     */
    for (int i = 0; i < substations; i++) {
        int index = generators + i;

        g->nodes[index].id = 200 + i;
        g->nodes[index].type = SUBSTATION;
    }

    /*
     * Consumers.
     */
    for (int i = 0; i < consumers; i++) {
        int index = generators +
                    substations + i;

        g->nodes[index].id = 300 + i;
        g->nodes[index].type = CONSUMER;
        g->nodes[index].demand = 10.0;
        g->nodes[index].priority =
            (i % 3) + 1;
    }

    /*
     * Connect generators to substations.
     */
    for (int i = 0; i < generators; i++) {

        int substationId =
            200 + (i % substations);

        if (!addBenchmarkEdge(
                g,
                100 + i,
                substationId,
                30.0)) {
            return 0;
        }
    }

    /*
     * Connected substation backbone.
     */
    for (int i = 0; i < substations - 1; i++) {

        if (!addBenchmarkEdge(
                g,
                200 + i,
                200 + i + 1,
                100.0)) {
            return 0;
        }
    }

    /*
     * Consumer connections.
     */
    for (int i = 0; i < consumers; i++) {

        int substationId =
            200 + (i % substations);

        if (!addBenchmarkEdge(
                g,
                substationId,
                300 + i,
                10.0)) {
            return 0;
        }
    }

    /*
     * Dense configuration:
     * Add nonconsecutive substation links.
     * Existing backbone edges are excluded.
     */
    if (dense) {

        for (int i = 0; i < substations; i++) {

            for (int j = i + 2;
                 j < substations;
                 j++) {

                if (!addBenchmarkEdge(
                        g,
                        200 + i,
                        200 + j,
                        100.0)) {
                    return 0;
                }
            }
        }
    }

    return 1;
}

/*
 * Benchmark one solver.
 *
 * type = 0: Week 4 Maximum Flow
 * type = 1: Week 5 Priority Flow
 */
static double measureSolver(
    const Graph *g,
    int type,
    double expectedFlow
)
{
    double totalAverage = 0.0;

    for (int batch = 0;
         batch < BENCH_BATCHES;
         batch++) {

        clock_t start = clock();

        if (start == (clock_t)-1) {
            return -1.0;
        }

        for (int run = 0;
             run < BENCH_REPEATS;
             run++) {

            if (type == 0) {

                if (!calculateMaximumFlow(
                        g, &maxResult)) {
                    return -1.0;
                }

                if (!closeMW(
                        maxResult.maxFlow,
                        expectedFlow)) {
                    return -1.0;
                }
            }
            else {

                if (!calculatePriorityFlow(
                        g, &priorityResult)) {
                    return -1.0;
                }

                if (!closeMW(
                        priorityResult.totalDelivered,
                        expectedFlow)) {
                    return -1.0;
                }
            }
        }

        clock_t end = clock();

        if (end == (clock_t)-1) {
            return -1.0;
        }

        double elapsedMs =
            1000.0 * (double)(end - start) /
            (double)CLOCKS_PER_SEC;

        totalAverage +=
            elapsedMs / BENCH_REPEATS;
    }

    return totalAverage / BENCH_BATCHES;
}

static int benchmarkConfiguration(
    int generators,
    int substations,
    int consumers,
    int dense
)
{
    if (!buildBenchmarkGraph(
            &graph,
            generators,
            substations,
            consumers,
            dense)) {
        return 0;
    }

    /*
     * Establish correct total throughput.
     */
    if (!calculateMaximumFlow(
            &graph, &maxResult)) {
        return 0;
    }

    double expectedFlow = maxResult.maxFlow;

    if (!calculatePriorityFlow(
            &graph, &priorityResult)) {
        return 0;
    }

    if (!closeMW(
            priorityResult.totalDelivered,
            expectedFlow)) {
        return 0;
    }

    /*
     * Warm-up both algorithms.
     */
    for (int i = 0; i < 10; i++) {

        if (!calculateMaximumFlow(
                &graph, &maxResult) ||
            !calculatePriorityFlow(
                &graph, &priorityResult)) {
            return 0;
        }
    }

    double maxflowMs =
        measureSolver(&graph, 0, expectedFlow);

    double priorityMs =
        measureSolver(&graph, 1, expectedFlow);

    if (maxflowMs <= 0.0 ||
        priorityMs <= 0.0) {
        return 0;
    }

    double ratio = priorityMs / maxflowMs;

    printf("%-7d %-7d %-8s %-12.6f "
           "%-12.6f %.3f\n",
           graph.nodeCount,
           graph.edgeCount,
           dense ? "Dense" : "Sparse",
           maxflowMs,
           priorityMs,
           ratio);

    return 1;
}

int main(void)
{
    printf("========================================\n");
    printf(" SMART GRID SCALABILITY BENCHMARK\n");
    printf("========================================\n");

    printf("%-7s %-7s %-8s %-12s "
           "%-12s %s\n",
           "Nodes",
           "Edges",
           "Type",
           "MaxFlow(ms)",
           "Priority(ms)",
           "Ratio");

    if (!benchmarkConfiguration(
            2, 3, 5, 0) ||
        !benchmarkConfiguration(
            2, 3, 5, 1) ||
        !benchmarkConfiguration(
            3, 5, 12, 0) ||
        !benchmarkConfiguration(
            3, 5, 12, 1) ||
        !benchmarkConfiguration(
            4, 8, 18, 0) ||
        !benchmarkConfiguration(
            4, 8, 18, 1)) {

        printf("\n[FAIL] Scalability benchmark\n");
        return 1;
    }

    printf("\n[PASS] All scaling benchmarks completed\n");

    return 0;
}
