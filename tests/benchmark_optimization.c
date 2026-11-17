

#include <stdio.h>
#include <time.h>

#include "dp.h"

#define BENCHMARK_TRIALS 10000

static void createBenchmarkProblem(DPProblem *problem)
{
    *problem = (DPProblem){0};

    problem->capacity = 60;
    problem->consumerCount = 5;

    problem->consumers[0] =
        (DPConsumer){4, 20, 0, 1};

    problem->consumers[1] =
        (DPConsumer){7, 10, 0, 2};

    problem->consumers[2] =
        (DPConsumer){5, 25, 0, 3};

    problem->consumers[3] =
        (DPConsumer){6, 30, 0, 3};

    problem->consumers[4] =
        (DPConsumer){8, 15, 0, 3};
}

int main(void)
{
    DPProblem problem = {0};
    DPPriorityResult result2D = {0};
    DPPriorityScore result1D = {0};

    volatile double checksum = 0.0;

    createBenchmarkProblem(&problem);

    /* Warm up both implementations. */
    if (!solvePriorityKnapsack(&problem, &result2D) ||
        !solvePriorityKnapsack1D(&problem, &result1D)) {
        printf("Warm-up failed.\n");
        return 1;
    }

    clock_t start = clock();

    for (int i = 0; i < BENCHMARK_TRIALS; i++) {

        if (!solvePriorityKnapsack(&problem, &result2D)) {
            printf("2D DP failed.\n");
            return 1;
        }

        checksum += result2D.allocatedPower;
    }

    clock_t end = clock();

    double time2D =
        (double)(end - start) / CLOCKS_PER_SEC;

    start = clock();

    for (int i = 0; i < BENCHMARK_TRIALS; i++) {

        if (!solvePriorityKnapsack1D(&problem, &result1D)) {
            printf("1D DP failed.\n");
            return 1;
        }

        checksum += result1D.allocatedPower;
    }

    end = clock();

    double time1D =
        (double)(end - start) / CLOCKS_PER_SEC;

    printf("\n========================================\n");
    printf("PRIORITY DP BENCHMARK\n");
    printf("========================================\n");

    printf("Trials per algorithm: %d\n",
           BENCHMARK_TRIALS);

    printf("\n2D Priority DP:\n");
    printf("Total CPU time : %.6f seconds\n", time2D);
    printf("Average time   : %.6f ms\n",
           time2D * 1000.0 / BENCHMARK_TRIALS);

    printf("\n1D Priority DP:\n");
    printf("Total CPU time : %.6f seconds\n", time1D);
    printf("Average time   : %.6f ms\n",
           time1D * 1000.0 / BENCHMARK_TRIALS);

    printf("\nChecksum: %.2f\n", checksum);

    return 0;
}
