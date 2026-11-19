
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "graph.h"
#include "maxflow.h"

#define BENCH_BATCHES 5
#define BENCH_REPEATS 1000

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

/*
 * Benchmark the complete public
 * Maximum Flow calculation.
 *
 * File loading is excluded.
 */
static int benchmarkScenario(
    const char *filename,
    double expectedFlow
)
{
    static Graph graph;
    static MaxFlowResult result;

    initializeGraph(&graph);

    if (!loadNetworkFromFile(&graph, filename)) {
        fprintf(stderr,
            "[FAIL] Cannot load: %s\n", filename);
        return 0;
    }

    /*
     * Warm-up and correctness check.
     */
    for (int i = 0; i < 10; i++) {

        if (!calculateMaximumFlow(
                &graph, &result) ||
            !closeMW(result.maxFlow, expectedFlow)) {

            fprintf(stderr,
                "[FAIL] Warm-up or flow mismatch\n");
            return 0;
        }
    }

    double totalAverage = 0.0;
    double minimum = INFINITY;
    double maximum = 0.0;

    printf("\n================================\n");
    printf("Network: %s\n", filename);
    printf("Expected Maximum Flow: %.2f MW\n",
           expectedFlow);
    printf("Repeats per batch: %d\n",
           BENCH_REPEATS);
    printf("================================\n");

    for (int batch = 0;
         batch < BENCH_BATCHES;
         batch++) {

        int successful = 1;

        clock_t start = clock();

        if (start == (clock_t)-1) {
            fprintf(stderr,
                "[FAIL] CPU clock unavailable\n");
            return 0;
        }

        for (int run = 0;
             run < BENCH_REPEATS;
             run++) {

            if (!calculateMaximumFlow(
                    &graph, &result)) {
                successful = 0;
                break;
            }

            if (!closeMW(
                    result.maxFlow, expectedFlow)) {
                successful = 0;
                break;
            }
        }

        clock_t end = clock();

        if (end == (clock_t)-1 || !successful) {
            fprintf(stderr,
                "[FAIL] Benchmark execution\n");
            return 0;
        }

        double elapsedMs =
            1000.0 * (double)(end - start) /
            (double)CLOCKS_PER_SEC;

        double averageMs =
            elapsedMs / BENCH_REPEATS;

        if (averageMs < minimum) {
            minimum = averageMs;
        }

        if (averageMs > maximum) {
            maximum = averageMs;
        }

        totalAverage += averageMs;

        printf("Batch %d: %.6f ms/call\n",
               batch + 1, averageMs);
    }

    printf("\nResults:\n");
    printf("Mean CPU time: %.6f ms/call\n",
           totalAverage / BENCH_BATCHES);
    printf("Minimum: %.6f ms/call\n", minimum);
    printf("Maximum: %.6f ms/call\n", maximum);

    printf("[PASS] Maximum Flow benchmark\n");

    return 1;
}

int main(void)
{
    if (!benchmarkScenario(
            "data/network.txt", 100.0)) {
        return 1;
    }

    if (!benchmarkScenario(
            "data/network_60mw.txt", 60.0)) {
        return 1;
    }

    printf("\nAll Maximum Flow benchmarks passed!\n");

    return 0;
}
