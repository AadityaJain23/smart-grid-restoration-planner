
#include <assert.h>
#include <stdio.h>
#include <time.h>

/*
 * Convert CPU clock ticks to milliseconds.
 */
static double elapsedMilliseconds(
    clock_t start,
    clock_t end
)
{
    return 1000.0 *
        (double)(end - start) /
        (double)CLOCKS_PER_SEC;
}

/*
 * A small deterministic workload.
 *
 * volatile prevents the compiler from
 * simply removing the computation.
 */
static double runWorkload(void)
{
    volatile double accumulator = 0.0;

    for (int i = 0; i < 1000000; i++) {
        accumulator +=
            (double)(i % 97) / 97.0;
    }

    return accumulator;
}

int main(void)
{
    /*
     * Verify clock() is available.
     */
    clock_t start = clock();

    if (start == (clock_t)-1) {
        printf("[FAIL] CPU clock unavailable\n");
        return 1;
    }

    double result = runWorkload();

    clock_t end = clock();

    if (end == (clock_t)-1) {
        printf("[FAIL] CPU clock unavailable\n");
        return 1;
    }

    double elapsed =
        elapsedMilliseconds(start, end);

    assert(elapsed >= 0.0);
    assert(result > 0.0);

    printf("[PASS] CPU timing completed\n");
    printf("Workload result: %.2f\n", result);
    printf("Elapsed CPU time: %.6f ms\n", elapsed);

    /*
     * Repeat the workload five times.
     */
    double total = 0.0;

    for (int trial = 0; trial < 5; trial++) {

        start = clock();

        if (start == (clock_t)-1) {
            return 1;
        }

        result = runWorkload();

        end = clock();

        if (end == (clock_t)-1) {
            return 1;
        }

        double duration =
            elapsedMilliseconds(start, end);

        if (duration < 0.0) {
            return 1;
        }

        total += duration;

        printf("Trial %d: %.6f ms\n",
               trial + 1, duration);
    }

    printf("Average CPU time: %.6f ms\n",
           total / 5.0);

    printf("\nAll benchmark timer tests passed!\n");

    return 0;
}
