
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "divide_conquer.h"

static void testMaximumSubarray(void) {

    /* Test 1: Smart-grid demand changes */
    double demand[] = {
        -4, 7, 3, -2, 8, -10, 4
    };

    SubarrayResult result;

    assert(maximumSubarray(demand, 7, &result) == 1);

    assert(result.sum == 16.0);
    assert(result.start == 1);
    assert(result.end == 4);

    printf("Test 1: Demand surge passed.\n");

    /* Test 2: All negative values */
    double negative[] = {-8, -3, -6};

    assert(maximumSubarray(negative, 3, &result) == 1);

    assert(result.sum == -3.0);
    assert(result.start == 1);
    assert(result.end == 1);

    printf("Test 2: All negative values passed.\n");

    /* Test 3: All positive values */
    double positive[] = {2, 4, 6, 8};

    assert(maximumSubarray(positive, 4, &result) == 1);

    assert(result.sum == 20.0);
    assert(result.start == 0);
    assert(result.end == 3);

    printf("Test 3: All positive values passed.\n");

    /* Test 4: Single element */
    double single[] = {5};

    assert(maximumSubarray(single, 1, &result) == 1);

    assert(result.sum == 5.0);
    assert(result.start == 0);
    assert(result.end == 0);

    printf("Test 4: Single element passed.\n");

    /* Test 5: Empty input */
    assert(maximumSubarray(single, 0, &result) == 0);

    printf("Test 5: Empty input passed.\n");

    /* Test 6: Invalid non-finite input */
    double invalid[] = {2, NAN, 3};

    assert(maximumSubarray(invalid, 3, &result) == 0);

    printf("Test 6: Invalid input passed.\n");

    printf("\nAll Maximum Subarray tests passed.\n");
}


static void testFastExponentiation(void) {

    double result = 0.0;

    /* Test 1: Positive integer base */
    assert(fastPower(2.0, 10U, &result) == 1);
    assert(result == 1024.0);

    printf("Test 1: Positive power passed.\n");

    /* Test 2: Zero exponent */
    assert(fastPower(5.0, 0U, &result) == 1);
    assert(result == 1.0);

    printf("Test 2: Zero exponent passed.\n");

    /* Test 3: Odd exponent */
    assert(fastPower(3.0, 5U, &result) == 1);
    assert(result == 243.0);

    printf("Test 3: Odd exponent passed.\n");

    /* Test 4: Negative base */
    assert(fastPower(-2.0, 3U, &result) == 1);
    assert(result == -8.0);

    printf("Test 4: Negative base passed.\n");

    /* Test 5: Zero base */
    assert(fastPower(0.0, 5U, &result) == 1);
    assert(result == 0.0);

    printf("Test 5: Zero base passed.\n");

    /* Test 6: Invalid output pointer */
    assert(fastPower(2.0, 5U, NULL) == 0);

    printf("Test 6: Invalid pointer passed.\n");

    /* Test 7: Overflow */
    assert(fastPower(1e200, 4U, &result) == 0);

    printf("Test 7: Overflow detection passed.\n");

    /* Test 8: Demand projection */
    double demand = 0.0;

    assert(projectDemand(
        100.0, 0.05, 5U, &demand
    ) == 1);

    assert(fabs(demand - 127.62815625) < 1e-8);

    printf("Test 8: Demand projection passed.\n");

    /* Test 9: Invalid demand */
    assert(projectDemand(
        -100.0, 0.05, 5U, &demand
    ) == 0);

    printf("Test 9: Invalid demand passed.\n");

    printf("\nAll Fast Exponentiation tests passed.\n");
}


int main(void) {

    testMaximumSubarray();

    printf("\n");

    testFastExponentiation();

    return 0;
}
