
#include <math.h>
#include <stdio.h>
#include <time.h>

#include "graph.h"
#include "priority_flow.h"

#define BENCH_BATCHES 5
#define BENCH_REPEATS 1000
#define BENCH_WARMUPS 10

static int closeMW(double a, double b)
{
    return fabs(a - b) <=
        1e-7 * (1.0 + fmax(fabs(a), fabs(b)));
}

/*
 * Verify the complete priority allocation.
 */
static int checkResult(
    const PriorityFlowResult *result,
    double expectedTotal,
    double expectedP1,
    double expectedP2,
    double expectedP3
)
{
    if (result == NULL) {
        return 0;
    }

    return
        closeMW(result->totalDelivered, expectedTotal) &&
        closeMW(result->deliveredByPriority[0], expectedP1) &&
        closeMW(result->deliveredByPriority[1], expectedP2) &&
        closeMW(result->deliveredByPriority[2], expectedP3) &&
        closeMW(
            result->totalDelivered + result->totalUnserved,
            result->totalDemand
        );
}

/*
 * Benchmark complete priority-flow calls.
 *
 * File loading is excluded from timing.
 */
static int benchmarkScenario(
    const char *filename,
    double expectedTotal,
    double expectedP1,
    double expectedP2,
    double expectedP3
)
{
    static Graph graph;
    static PriorityFlowResult result;

    initializeGraph(&graph);

    if (!loadNetworkFromFile(&graph, filename)) {
        fprintf(stderr,
            "[FAIL] Cannot load: %s\n",
            filename);
        return 0;
    }

    /*
     * Warm-up and correctness checks.
     */
    for (int i = 0; i < BENCH_WARMUPS; i++) {

        if (!calculatePriorityFlow(&graph, &result) ||
            !checkResult(
                &result,
                expectedTotal,
                expectedP1,
                expectedP2,
                expectedP3)) {

            fprintf(stderr,
                "[FAIL] Warm-up or priority mismatch: %s\n",
                filename);
            return 0;
        }
    }

    double totalAverage = 0.0;
    double minimum = INFINITY;
    double maximum = 0.0;

    printf("\n================================\n");
    printf("Priority Flow Benchmark\n");
    printf("Network: %s\n", filename);
    printf("Expected delivery: %.2f MW\n",
           expectedTotal);
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

            if (!calculatePriorityFlow(
                    &graph, &result)) {
                successful = 0;
                break;
            }

            if (!checkResult(
                    &result,
                    expectedTotal,
                    expectedP1,
                    expectedP2,
                    expectedP3)) {
                successful = 0;
                break;
            }
        }

        clock_t end = clock();

        if (end == (clock_t)-1 || !successful) {
            fprintf(stderr,
                "[FAIL] Priority benchmark execution\n");
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

    printf("[PASS] Priority Flow benchmark\n");

    return 1;
}

int main(void)
{
    if (!benchmarkScenario(
            "data/network.txt",
            100.0, 20.0, 10.0, 70.0)) {
        return 1;
    }

    if (!benchmarkScenario(
            "data/network_60mw.txt",
            60.0, 20.0, 10.0, 30.0)) {
        return 1;
    }

    printf("\nAll Priority Flow benchmarks passed!\n");

    return 0;
}
