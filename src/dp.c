#include "dp.h"

#include <stdio.h>
#include <string.h>
#include <limits.h>

static int validateDPProblem(const DPProblem *problem);

static DPPriorityScore
priorityTable[DP_MAX_CONSUMERS + 1]
             [DP_MAX_CAPACITY + 1];

static int isBetterPriorityScore(
    DPPriorityScore a,
    DPPriorityScore b
)
{
    if (a.priority1Count != b.priority1Count) {
        return a.priority1Count > b.priority1Count;
    }

    if (a.priority2Count != b.priority2Count) {
        return a.priority2Count > b.priority2Count;
    }

    if (a.priority3Count != b.priority3Count) {
        return a.priority3Count > b.priority3Count;
    }

    return a.allocatedPower > b.allocatedPower;
}


static DPPriorityScore addConsumerToScore(
    DPPriorityScore previous,
    const DPConsumer *consumer
)
{
    DPPriorityScore result = previous;

    if (consumer->priority == 1) {
        result.priority1Count++;
    } else if (consumer->priority == 2) {
        result.priority2Count++;
    } else {
        result.priority3Count++;
    }

    result.allocatedPower += consumer->demand;

    return result;
}


int solvePriorityKnapsack(
    const DPProblem *problem,
    DPPriorityResult *result
)
{
    if (result == NULL || !validateDPProblem(problem)) {
        return 0;
    }

    memset(result, 0, sizeof(*result));

    int n = problem->consumerCount;
    int capacity = problem->capacity;

    /* Initialize DP states. */
    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            priorityTable[i][w] = (DPPriorityScore){0};
        }
    }

    /* Build priority-aware DP table. */
    for (int i = 1; i <= n; i++) {

        const DPConsumer *consumer =
            &problem->consumers[i - 1];

        int demand = consumer->demand;

        for (int w = 0; w <= capacity; w++) {

            DPPriorityScore exclude =
                priorityTable[i - 1][w];

            DPPriorityScore best = exclude;

            if (demand <= w) {

                DPPriorityScore include =
                    addConsumerToScore(
                        priorityTable[i - 1][w - demand],
                        consumer
                    );

                if (isBetterPriorityScore(include, exclude)) {
                    best = include;
                }
            }

            priorityTable[i][w] = best;
        }
    }

    result->score = priorityTable[n][capacity];

    /* Reconstruct selected consumers. */
    int remaining = capacity;

    for (int i = n; i >= 1; i--) {

        const DPConsumer *consumer =
            &problem->consumers[i - 1];

        int demand = consumer->demand;

        if (demand > remaining) {
            continue;
        }

        DPPriorityScore exclude =
            priorityTable[i - 1][remaining];

        DPPriorityScore include =
            addConsumerToScore(
                priorityTable[i - 1][remaining - demand],
                consumer
            );

        if (isBetterPriorityScore(include, exclude)) {

            result->selected[result->selectedCount++] = i - 1;

            result->allocatedPower += demand;
            remaining -= demand;
        }
    }

    result->remainingPower =
        capacity - result->allocatedPower;

    return 1;
}


void displayPriorityKnapsackResult(
    const DPProblem *problem,
    const DPPriorityResult *result
)
{
    if (problem == NULL || result == NULL) {
        printf("Invalid priority DP result.\n");
        return;
    }

    printf("\n========================================\n");
    printf("    PRIORITY-AWARE POWER ALLOCATION\n");
    printf("========================================\n");

    printf("Available Supply : %d MW\n",
           problem->capacity);

    printf("Allocated Power  : %d MW\n",
           result->allocatedPower);

    printf("Remaining Power  : %d MW\n",
           result->remainingPower);

    printf("Selected Loads   : %d\n",
           result->selectedCount);

    printf("\nFully Served Consumers by Priority:\n");

    printf("Priority 1 : %d\n",
           result->score.priority1Count);

    printf("Priority 2 : %d\n",
           result->score.priority2Count);

    printf("Priority 3 : %d\n",
           result->score.priority3Count);

    printf("\nSelected Consumers:\n");

    if (result->selectedCount == 0) {
        printf("None\n");
        return;
    }

    printf("%-10s %-12s %-10s\n",
           "Node ID", "Demand", "Priority");

    printf("----------------------------------\n");

    for (int i = result->selectedCount - 1; i >= 0; i--) {

        int index = result->selected[i];

        if (index < 0 || index >= problem->consumerCount) {
            printf("Invalid selection index.\n");
            return;
        }

        const DPConsumer *consumer =
            &problem->consumers[index];

        printf("%-10d %-12d %-10d\n",
               consumer->nodeId,
               consumer->demand,
               consumer->priority);
    }
}


static int dpTable[DP_MAX_CONSUMERS + 1]
                  [DP_MAX_CAPACITY + 1];

/* Memory-optimized priority-aware DP table */
static DPPriorityScore
priorityTable1D[DP_MAX_CAPACITY + 1];



static int validateDPProblem(const DPProblem *problem)
{
    if (problem == NULL) {
        return 0;
    }

    if (problem->consumerCount < 0 ||
        problem->consumerCount > DP_MAX_CONSUMERS) {
        return 0;
    }

    if (problem->capacity < 0 ||
        problem->capacity > DP_MAX_CAPACITY) {
        return 0;
    }

    for (int i = 0; i < problem->consumerCount; i++) {

        if (problem->consumers[i].demand < 0) {
            return 0;
        }

        if (problem->consumers[i].benefit < 0) {
            return 0;
        }

        if (problem->consumers[i].priority < 1 ||
            problem->consumers[i].priority > 3) {
            return 0;
        }

        for (int j = 0; j < i; j++) {
            if (problem->consumers[i].nodeId ==
                problem->consumers[j].nodeId) {
                return 0;
            }
        }
    }

    return 1;
}


int solveKnapsack(
    const DPProblem *problem,
    DPResult *result
)
{
    if (result == NULL || !validateDPProblem(problem)) {
        return 0;
    }

    memset(result, 0, sizeof(*result));

    int n = problem->consumerCount;
    int capacity = problem->capacity;

    /* Initialize the DP table. */
    for (int i = 0; i <= n; i++) {
        for (int w = 0; w <= capacity; w++) {
            dpTable[i][w] = 0;
        }
    }

    /* Build the DP table. */
    for (int i = 1; i <= n; i++) {

        int demand = problem->consumers[i - 1].demand;
        int benefit = problem->consumers[i - 1].benefit;

        for (int w = 0; w <= capacity; w++) {

            int exclude = dpTable[i - 1][w];

            if (demand <= w) {

                int previous =
                    dpTable[i - 1][w - demand];

                /* Prevent signed integer overflow. */
                if (benefit > INT_MAX - previous) {
                    memset(result, 0, sizeof(*result));
                    return 0;
                }

                int include = benefit + previous;

                dpTable[i][w] =
                    (include > exclude) ? include : exclude;

            } else {

                dpTable[i][w] = exclude;
            }
        }
    }

    result->maximumBenefit = dpTable[n][capacity];

    /* Reconstruct selected consumers. */
    int remaining = capacity;

    for (int i = n; i >= 1; i--) {

        int demand = problem->consumers[i - 1].demand;

        if (demand <= remaining &&
            dpTable[i][remaining] >
            dpTable[i - 1][remaining]) {

            result->selected[result->selectedCount] = i - 1;
            result->selectedCount++;

            result->allocatedPower += demand;
            remaining -= demand;
        }
    }

    result->remainingPower =
        capacity - result->allocatedPower;

    return 1;
}


void displayKnapsackResult(
    const DPProblem *problem,
    const DPResult *result
)
{
    if (problem == NULL || result == NULL) {
        printf("Invalid DP result.\n");
        return;
    }

    printf("\n========================================\n");
    printf("     DYNAMIC PROGRAMMING - KNAPSACK\n");
    printf("========================================\n");

    printf("Available Supply : %d MW\n",
           problem->capacity);

    printf("Maximum Benefit  : %d\n",
           result->maximumBenefit);

    printf("Allocated Power  : %d MW\n",
           result->allocatedPower);

    printf("Remaining Power  : %d MW\n",
           result->remainingPower);

    printf("Selected Loads   : %d\n",
           result->selectedCount);

    printf("\nSelected Consumers:\n");

    if (result->selectedCount == 0) {
        printf("None\n");
        return;
    }

    printf("%-10s %-12s %-12s %-10s\n",
           "Node ID", "Demand", "Benefit", "Priority");

    printf("---------------------------------------------\n");

    for (int i = result->selectedCount - 1; i >= 0; i--) {

        int index = result->selected[i];

        if (index < 0 || index >= problem->consumerCount) {
            printf("Invalid selection index.\n");
            return;
        }

        const DPConsumer *consumer =
            &problem->consumers[index];

        printf("%-10d %-12d %-12d %-10d\n",
               consumer->nodeId,
               consumer->demand,
               consumer->benefit,
               consumer->priority);
    }
}



int solvePriorityKnapsack1D(
    const DPProblem *problem,
    DPPriorityScore *score
)
{
    if (score == NULL || !validateDPProblem(problem)) {
        return 0;
    }

    *score = (DPPriorityScore){0};

    int n = problem->consumerCount;
    int capacity = problem->capacity;

    /* Initialize the 1D DP table. */
    for (int w = 0; w <= capacity; w++) {
        priorityTable1D[w] = (DPPriorityScore){0};
    }

    /* Process each consumer exactly once. */
    for (int i = 0; i < n; i++) {

        const DPConsumer *consumer =
            &problem->consumers[i];

        int demand = consumer->demand;

        /*
         * Descending capacity order is essential
         * for 0/1 Knapsack.
         */
        for (int w = capacity; w >= demand; w--) {

            DPPriorityScore exclude =
                priorityTable1D[w];

            DPPriorityScore include =
                addConsumerToScore(
                    priorityTable1D[w - demand],
                    consumer
                );

            if (isBetterPriorityScore(include, exclude)) {
                priorityTable1D[w] = include;
            }
        }
    }

    *score = priorityTable1D[capacity];

    return 1;
}
