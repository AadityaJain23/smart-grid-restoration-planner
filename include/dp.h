
#ifndef DP_H
#define DP_H

#include "graph.h"

/*
 * Week 4 - Dynamic Programming
 * Classical 0/1 Knapsack for full-service
 * consumer selection.
 *
 * Demand and supply use integer MW units.
 */

#define DP_MAX_CONSUMERS MAX_NODES
#define DP_MAX_CAPACITY 1000

typedef struct {
    int nodeId;
    int demand;
    int benefit;
    int priority;
} DPConsumer;

typedef struct {
    DPConsumer consumers[DP_MAX_CONSUMERS];
    int consumerCount;
    int capacity;
} DPProblem;

typedef struct {
    int selected[DP_MAX_CONSUMERS];
    int selectedCount;

    int maximumBenefit;
    int allocatedPower;
    int remainingPower;
} DPResult;

/*
 * Solves the classical 0/1 Knapsack problem.
 *
 * Returns:
 *  1 on success
 *  0 on invalid input or failure
 *
 * selected[] contains indices into
 * problem->consumers[], not graph node IDs.
 */
int solveKnapsack(
    const DPProblem *problem,
    DPResult *result
);

/*
 * Displays the selected consumers
 * and the optimization summary.
 */
void displayKnapsackResult(
    const DPProblem *problem,
    const DPResult *result
);


/*
 * Priority-aware DP score.
 *
 * Comparison order:
 * 1. Priority-1 served count
 * 2. Priority-2 served count
 * 3. Priority-3 served count
 * 4. Total allocated MW
 */
typedef struct {
    int priority1Count;
    int priority2Count;
    int priority3Count;
    int allocatedPower;
} DPPriorityScore;

typedef struct {
    int selected[DP_MAX_CONSUMERS];
    int selectedCount;

    DPPriorityScore score;

    int allocatedPower;
    int remainingPower;
} DPPriorityResult;

/*
 * Solves priority-aware full-service selection.
 *
 * Returns 1 on success, 0 on failure.
 *
 * selected[] contains indices into
 * problem->consumers[].
 */
int solvePriorityKnapsack(
    const DPProblem *problem,
    DPPriorityResult *result
);

void displayPriorityKnapsackResult(
    const DPProblem *problem,
    const DPPriorityResult *result
);

/*
 * Memory-optimized Priority Knapsack (1D DP)
 *
 * Returns:
 * 1 = Success
 * 0 = Failure
 */
int solvePriorityKnapsack1D(
    const DPProblem *problem,
    DPPriorityScore *score
);

#endif
