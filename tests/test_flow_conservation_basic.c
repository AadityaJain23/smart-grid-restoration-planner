
#include <assert.h>
#include <math.h>
#include <stdio.h>

int main(void)
{
    /*
     * Simple substation example:
     *
     * 40 MW enters.
     * 20 MW goes to Hospital.
     * 15 MW goes to Residential.
     * 5 MW goes to another substation.
     */

    double inflow = 40.0;

    double hospital = 20.0;
    double residential = 15.0;
    double substationB = 5.0;

    double outflow =
        hospital + residential + substationB;

    double balance = inflow - outflow;

    assert(fabs(balance) < 1e-9);

    printf("[PASS] Balanced substation\n");

    /*
     * Test an invalid allocation.
     */
    double invalidOutflow =
        hospital + residential + 10.0;

    double invalidBalance =
        inflow - invalidOutflow;

    assert(fabs(invalidBalance) > 1e-9);

    printf("[PASS] Flow imbalance detected\n");

    /*
     * Test signed transmission flow.
     *
     * Original capacity = 30 MW.
     * Forward residual = 20 MW.
     */
    double capacity = 30.0;
    double forwardResidual = 20.0;

    double signedFlow =
        capacity - forwardResidual;

    assert(fabs(signedFlow - 10.0) < 1e-9);

    printf("[PASS] Signed forward flow\n");

    /*
     * Reverse flow:
     * Forward residual = 40 MW.
     */
    forwardResidual = 40.0;

    signedFlow =
        capacity - forwardResidual;

    assert(fabs(signedFlow + 10.0) < 1e-9);

    printf("[PASS] Signed reverse flow\n");

    printf("\nAll basic flow conservation tests passed!\n");

    return 0;
}
