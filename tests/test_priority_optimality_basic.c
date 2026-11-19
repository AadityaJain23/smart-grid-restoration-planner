
#include <assert.h>
#include <stdio.h>

/*
 * Compare two delivered-power vectors.
 *
 * Returns:
 *  1 if A is lexicographically better
 * -1 if B is lexicographically better
 *  0 if equal
 */
static int compareAllocation(
    const double a[3],
    const double b[3]
)
{
    for (int i = 0; i < 3; i++) {

        if (a[i] > b[i]) {
            return 1;
        }

        if (a[i] < b[i]) {
            return -1;
        }
    }

    return 0;
}

int main(void)
{
    double A[3] = {0.0, 0.0, 30.0};
    double B[3] = {15.0, 10.0, 5.0};
    double C[3] = {20.0, 10.0, 0.0};

    assert(compareAllocation(B, A) == 1);

    printf("[PASS] Priority 1 improvement preferred\n");

    assert(compareAllocation(C, B) == 1);

    printf("[PASS] Higher Priority 1 delivery preferred\n");

    double D[3] = {20.0, 5.0, 5.0};

    assert(compareAllocation(C, D) == 1);

    printf("[PASS] Priority 2 breaks Priority 1 tie\n");

    double E[3] = {20.0, 10.0, 5.0};

    assert(compareAllocation(E, C) == 1);

    printf("[PASS] Priority 3 breaks higher-priority tie\n");

    assert(compareAllocation(C, C) == 0);

    printf("[PASS] Equal allocations detected\n");

    printf("\nAll lexicographic optimality tests passed!\n");

    return 0;
}
