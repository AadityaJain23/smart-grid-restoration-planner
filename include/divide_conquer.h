#ifndef DIVIDE_CONQUER_H
#define DIVIDE_CONQUER_H

typedef struct {
    double sum;
    int start;
    int end;
} SubarrayResult;

/*
 * Find maximum nonempty contiguous subarray.
 * Returns 1 on success, 0 on invalid input.
 */
int maximumSubarray(
    const double values[],
    int count,
    SubarrayResult *result
);


/*
 * Lecture 10: Fast Exponentiation
 *
 * Calculates base^exponent for exponent >= 0.
 * Returns 1 on success, 0 on invalid input
 * or non-finite result.
 */
int fastPower(
    double base,
    unsigned int exponent,
    double *result
);

/*
 * Simplified compound demand-growth projection.
 * Returns 1 on success, 0 on invalid input.
 */
int projectDemand(
    double initialDemand,
    double annualGrowthRate,
    unsigned int years,
    double *projectedDemand
);

#endif