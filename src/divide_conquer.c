
#include <stddef.h>
#include <float.h>
#include <math.h>

#include "divide_conquer.h"

/*
 * Find maximum subarray crossing the midpoint.
 */
static SubarrayResult maximumCrossingSubarray(
    const double values[],
    int low,
    int mid,
    int high
) {
    double leftBest = -DBL_MAX;
    double sum = 0.0;

    int bestLeft = mid;

    /* Maximum suffix of left half */
    for (int i = mid; i >= low; i--) {

        sum += values[i];

        if (sum > leftBest) {
            leftBest = sum;
            bestLeft = i;
        }
    }

    double rightBest = -DBL_MAX;
    sum = 0.0;

    int bestRight = mid + 1;

    /* Maximum prefix of right half */
    for (int j = mid + 1; j <= high; j++) {

        sum += values[j];

        if (sum > rightBest) {
            rightBest = sum;
            bestRight = j;
        }
    }

    SubarrayResult result;

    result.sum = leftBest + rightBest;
    result.start = bestLeft;
    result.end = bestRight;

    return result;
}

/*
 * Recursive divide-and-conquer solution.
 */
static SubarrayResult maximumSubarrayRecursive(
    const double values[],
    int low,
    int high
) {
    /* Base case: one element */
    if (low == high) {

        SubarrayResult result;

        result.sum = values[low];
        result.start = low;
        result.end = high;

        return result;
    }

    int mid = low + (high - low) / 2;

    SubarrayResult leftResult =
        maximumSubarrayRecursive(
            values, low, mid
        );

    SubarrayResult rightResult =
        maximumSubarrayRecursive(
            values, mid + 1, high
        );

    SubarrayResult crossResult =
        maximumCrossingSubarray(
            values, low, mid, high
        );

    /* Select the largest sum */
    if (leftResult.sum >= rightResult.sum &&
        leftResult.sum >= crossResult.sum) {

        return leftResult;
    }

    if (rightResult.sum >= leftResult.sum &&
        rightResult.sum >= crossResult.sum) {

        return rightResult;
    }

    return crossResult;
}

/*
 * Public interface.
 */
int maximumSubarray(
    const double values[],
    int count,
    SubarrayResult *result
) {
    if (values == NULL ||
        result == NULL ||
        count <= 0) {
        return 0;
    }

    /*
     * Reject non-finite input.
     * Also reject sums that may overflow double.
     */
    double absoluteTotal = 0.0;

    for (int i = 0; i < count; i++) {

        if (!isfinite(values[i])) {
            return 0;
        }

        double magnitude = fabs(values[i]);

        if (magnitude > DBL_MAX - absoluteTotal) {
            return 0;
        }

        absoluteTotal += magnitude;
    }

    *result = maximumSubarrayRecursive(
        values, 0, count - 1
    );

    return 1;
}



/*
 * Internal recursive exponentiation by squaring.
 *
 * Precondition:
 * base is finite and exponent is nonnegative.
 */
static double fastPowerRecursive(
    double base,
    unsigned int exponent
) {
    if (exponent == 0U) {
        return 1.0;
    }

    double half = fastPowerRecursive(
        base,
        exponent / 2U
    );

    double result = half * half;

    if (exponent % 2U != 0U) {
        result *= base;
    }

    return result;
}

/*
 * Public fast exponentiation function.
 */
int fastPower(
    double base,
    unsigned int exponent,
    double *result
) {
    if (result == NULL || !isfinite(base)) {
        return 0;
    }

    double value = fastPowerRecursive(
        base,
        exponent
    );

    if (!isfinite(value)) {
        return 0;
    }

    *result = value;

    return 1;
}



/*
 * Project demand using compound growth:
 *
 * D(t) = D0 * (1 + r)^t
 *
 * annualGrowthRate is a decimal fraction.
 * Example: 0.05 represents 5%.
 */
int projectDemand(
    double initialDemand,
    double annualGrowthRate,
    unsigned int years,
    double *projectedDemand
) {
    if (projectedDemand == NULL) {
        return 0;
    }

    if (!isfinite(initialDemand) ||
        !isfinite(annualGrowthRate)) {
        return 0;
    }

    if (initialDemand < 0.0 ||
        annualGrowthRate < -1.0) {
        return 0;
    }

    double factor;

    if (!fastPower(
        1.0 + annualGrowthRate,
        years,
        &factor
    )) {
        return 0;
    }

    double demand = initialDemand * factor;

    if (!isfinite(demand)) {
        return 0;
    }

    *projectedDemand = demand;

    return 1;
}
