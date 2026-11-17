
#include <stdio.h>
#include <time.h>
#include <math.h>

#include "graph.h"
#include "mst.h"

#define TRIALS 10000
#define EPSILON 1e-9

/*
 * Prevent the benchmark results from being unused.
 * This does not guarantee perfectly accurate timings.
 */
static volatile double checksum = 0.0;

static int benchmarkNetwork(const char *filename) {
    Graph graph;
    AdjList list;
    PrimResult prim;
    KruskalResult kruskal;

    if (!loadNetworkFromFile(&graph, filename) ||
        !buildAdjacencyList(&graph, &list) ||
        !validateAdjacencyList(&graph, &list)) {
        printf("Error: Network setup failed.\n");
        return 0;
    }

    /* Use the first node as Prim's starting vertex. */
    int startId = graph.nodes[0].id;

    /* Verify correctness before benchmarking. */
    if (!primMinimumSpanningTree(
            &graph, &list, startId, &prim) ||
        !kruskalMinimumSpanningTree(
            &graph, &list, &kruskal)) {
        printf("Error: MST calculation failed.\n");
        return 0;
    }

    if (prim.isConnected != kruskal.isConnected ||
        prim.selectedEdges != kruskal.selectedEdges ||
        fabs(prim.totalCost - kruskal.totalCost)
            > EPSILON) {
        printf("Error: Prim and Kruskal disagree.\n");
        return 0;
    }

    printf("\nNetwork: %s\n", filename);
    printf("Nodes: %d, Edges: %d\n",
           graph.nodeCount, graph.edgeCount);
    printf("Prim MST Cost: %.2f\n", prim.totalCost);
    printf("Kruskal MST Cost: %.2f\n",
           kruskal.totalCost);

    /* Measure Prim. */
    clock_t start = clock();

    for (int i = 0; i < TRIALS; i++) {
        if (!primMinimumSpanningTree(
                &graph, &list, startId, &prim)) {
            return 0;
        }

        checksum += prim.totalCost;
    }

    clock_t end = clock();

    if (start == (clock_t)-1 ||
        end == (clock_t)-1) {
        printf("Error: CPU clock unavailable.\n");
        return 0;
    }

    double primTime =
        (double)(end - start) / CLOCKS_PER_SEC;

    /* Measure Kruskal on the same unchanged graph. */
    start = clock();

    for (int i = 0; i < TRIALS; i++) {
        if (!kruskalMinimumSpanningTree(
                &graph, &list, &kruskal)) {
            return 0;
        }

        checksum += kruskal.totalCost;
    }

    end = clock();

    if (start == (clock_t)-1 ||
        end == (clock_t)-1) {
        printf("Error: CPU clock unavailable.\n");
        return 0;
    }

    double kruskalTime =
        (double)(end - start) / CLOCKS_PER_SEC;

    printf("\nTrials per algorithm: %d\n", TRIALS);

    printf("Prim total CPU time: %.6f s\n",
           primTime);

    printf("Kruskal total CPU time: %.6f s\n",
           kruskalTime);

    printf("Prim average: %.6f ms\n",
           primTime * 1000.0 / TRIALS);

    printf("Kruskal average: %.6f ms\n",
           kruskalTime * 1000.0 / TRIALS);

    return 1;
}

int main(void) {
    printf("========== MST BENCHMARK ==========\n");

    if (!benchmarkNetwork("data/network.txt")) {
        return 1;
    }

    if (!benchmarkNetwork(
            "data/network_redundant.txt")) {
        return 1;
    }

    printf("\nBenchmark completed successfully.\n");
    printf("Checksum: %.2f\n", checksum);

    return 0;
}
