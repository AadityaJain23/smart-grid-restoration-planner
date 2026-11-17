
#include <stdio.h>
#include <assert.h>

#include "dp.h"

static void testOriginalGrid60MW(void)
{
    DPProblem problem = {0};
    DPPriorityResult result = {0};

    problem.consumerCount = 5;
    problem.capacity = 60;

    problem.consumers[0] = (DPConsumer){
        4, 20, 0, 1
    };

    problem.consumers[1] = (DPConsumer){
        7, 10, 0, 2
    };

    problem.consumers[2] = (DPConsumer){
        5, 25, 0, 3
    };

    problem.consumers[3] = (DPConsumer){
        6, 30, 0, 3
    };

    problem.consumers[4] = (DPConsumer){
        8, 15, 0, 3
    };

    assert(solvePriorityKnapsack(&problem, &result) == 1);

    assert(result.score.priority1Count == 1);
    assert(result.score.priority2Count == 1);
    assert(result.score.priority3Count == 1);

    assert(result.allocatedPower == 60);
    assert(result.remainingPower == 0);
    assert(result.selectedCount == 3);

    displayPriorityKnapsackResult(&problem, &result);

    printf("\n[PASS] Original Grid - 60 MW\n");
}

int main(void)
{
    testOriginalGrid60MW();

    printf("\nAll priority DP tests passed!\n");

    return 0;
}
