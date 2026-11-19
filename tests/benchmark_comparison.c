
#include <math.h>
#include <stdio.h>

static void compare(
    const char *name,
    double maxflowMs,
    double priorityMs
)
{
    if (!isfinite(maxflowMs) ||
        !isfinite(priorityMs) ||
        maxflowMs <= 0.0 ||
        priorityMs <= 0.0) {
        printf("[ERROR] Invalid timing for %s\n", name);
        return;
    }

    double difference = priorityMs - maxflowMs;
    double ratio = priorityMs / maxflowMs;
    double overhead = (ratio - 1.0) * 100.0;

    printf("\nNetwork: %s\n", name);
    printf("Maximum Flow : %.6f ms\n", maxflowMs);
    printf("Priority Flow: %.6f ms\n", priorityMs);
    printf("Difference   : %+.6f ms\n", difference);
    printf("Ratio        : %.3fx\n", ratio);
    printf("Overhead     : %+.2f%%\n", overhead);
}

int main(void)
{
    double mf100, pf100, mf60, pf60;

    printf("Enter mean CPU times in ms/call.\n");

    printf("100 MW Maximum Flow: ");
    if (scanf("%lf", &mf100) != 1) return 1;

    printf("100 MW Priority Flow: ");
    if (scanf("%lf", &pf100) != 1) return 1;

    printf("60 MW Maximum Flow: ");
    if (scanf("%lf", &mf60) != 1) return 1;

    printf("60 MW Priority Flow: ");
    if (scanf("%lf", &pf60) != 1) return 1;

    if (!isfinite(mf100) || !isfinite(pf100) ||
        !isfinite(mf60) || !isfinite(pf60) ||
        mf100 <= 0.0 || pf100 <= 0.0 ||
        mf60 <= 0.0 || pf60 <= 0.0) {
        printf("[FAIL] All times must be finite and positive.\n");
        return 1;
    }

    compare("Original 100 MW", mf100, pf100);
    compare("Limited 60 MW", mf60, pf60);

    printf("\n[PASS] Benchmark comparison completed\n");
    return 0;
}
