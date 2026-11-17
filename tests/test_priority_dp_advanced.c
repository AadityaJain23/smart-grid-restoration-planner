
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "dp.h"

static int passed = 0;

static void addConsumer(
    DPProblem *p,
    int id,
    int demand,
    int priority
)
{
    assert(p->consumerCount < DP_MAX_CONSUMERS);

    int i = p->consumerCount++;

    p->consumers[i] = (DPConsumer){
        id, demand, 0, priority
    };
}

static void checkBoth(
    const DPProblem *problem,
    int p1,
    int p2,
    int p3,
    int allocated
)
{
    DPPriorityResult result2D = {0};
    DPPriorityScore result1D = {0};

    assert(solvePriorityKnapsack(problem, &result2D) == 1);

    assert(solvePriorityKnapsack1D(problem, &result1D) == 1);

    /* Check the expected optimum. */
    assert(result2D.score.priority1Count == p1);
    assert(result2D.score.priority2Count == p2);
    assert(result2D.score.priority3Count == p3);
    assert(result2D.score.allocatedPower == allocated);

    /* Check the 1D and 2D scores agree. */
    assert(result1D.priority1Count ==
           result2D.score.priority1Count);

    assert(result1D.priority2Count ==
           result2D.score.priority2Count);

    assert(result1D.priority3Count ==
           result2D.score.priority3Count);

    assert(result1D.allocatedPower ==
           result2D.score.allocatedPower);

    /* Check the reconstructed allocation. */
    assert(result2D.allocatedPower == allocated);
    assert(result2D.remainingPower ==
           problem->capacity - allocated);

    int total = 0;
    int counts[4] = {0};

    for (int i = 0; i < result2D.selectedCount; i++) {
        int index = result2D.selected[i];

        assert(index >= 0);
        assert(index < problem->consumerCount);

        const DPConsumer *c = &problem->consumers[index];

        total += c->demand;
        counts[c->priority]++;
    }

    assert(total == allocated);
    assert(counts[1] == p1);
    assert(counts[2] == p2);
    assert(counts[3] == p3);
    assert(allocated <= problem->capacity);
}

static void testOriginalGrid(void)
{
    DPProblem p = {0};
    p.capacity = 60;

    addConsumer(&p, 4, 20, 1);
    addConsumer(&p, 7, 10, 2);
    addConsumer(&p, 5, 25, 3);
    addConsumer(&p, 6, 30, 3);
    addConsumer(&p, 8, 15, 3);

    checkBoth(&p, 1, 1, 1, 60);

    printf("[PASS] Original grid - 60 MW\n");
    passed++;
}

static void testCriticalShortage(void)
{
    DPProblem p = {0};
    p.capacity = 20;

    addConsumer(&p, 10, 20, 1);
    addConsumer(&p, 20, 10, 2);
    addConsumer(&p, 30, 10, 3);

    checkBoth(&p, 1, 0, 0, 20);

    printf("[PASS] Critical-load priority\n");
    passed++;
}

static void testSamePriority(void)
{
    DPProblem p = {0};
    p.capacity = 15;

    addConsumer(&p, 10, 10, 3);
    addConsumer(&p, 20, 7, 3);
    addConsumer(&p, 30, 8, 3);

    checkBoth(&p, 0, 0, 2, 15);

    printf("[PASS] Same-priority optimization\n");
    passed++;
}

static void testZeroSupply(void)
{
    DPProblem p = {0};
    p.capacity = 0;

    addConsumer(&p, 10, 5, 1);

    checkBoth(&p, 0, 0, 0, 0);

    printf("[PASS] Zero supply\n");
    passed++;
}

static void testZeroDemand(void)
{
    DPProblem p = {0};
    p.capacity = 0;

    addConsumer(&p, 10, 0, 1);
    addConsumer(&p, 20, 5, 2);

    checkBoth(&p, 1, 0, 0, 0);

    printf("[PASS] Zero-demand consumer\n");
    passed++;
}

static void testEmptyProblem(void)
{
    DPProblem p = {0};
    p.capacity = 50;

    checkBoth(&p, 0, 0, 0, 0);

    printf("[PASS] Empty consumer list\n");
    passed++;
}

static void testInputPreservation(void)
{
    DPProblem p = {0};
    DPProblem original;

    p.capacity = 30;

    addConsumer(&p, 10, 20, 1);
    addConsumer(&p, 20, 10, 2);

    original = p;

    checkBoth(&p, 1, 1, 0, 30);

    assert(memcmp(&p, &original, sizeof(p)) == 0);

    printf("[PASS] Input preservation\n");
    passed++;
}

static void testRepeatedCalls(void)
{
    DPProblem p = {0};
    p.capacity = 30;

    addConsumer(&p, 10, 20, 1);
    addConsumer(&p, 20, 10, 2);

    checkBoth(&p, 1, 1, 0, 30);

    p.capacity = 10;

    checkBoth(&p, 0, 1, 0, 10);

    printf("[PASS] Repeated solver calls\n");
    passed++;
}

static void testInvalidInput(void)
{
    DPProblem p = {0};
    DPPriorityScore score = {0};
    DPPriorityResult result = {0};

    p.capacity = DP_MAX_CAPACITY + 1;

    assert(solvePriorityKnapsack(&p, &result) == 0);
    assert(solvePriorityKnapsack1D(&p, &score) == 0);

    printf("[PASS] Invalid capacity rejected\n");
    passed++;
}

static void testMaximumCapacity(void)
{
    DPProblem p = {0};
    p.capacity = DP_MAX_CAPACITY;

    addConsumer(&p, 10, DP_MAX_CAPACITY, 1);

    checkBoth(&p, 1, 0, 0, DP_MAX_CAPACITY);

    printf("[PASS] Maximum capacity boundary\n");
    passed++;
}

int main(void)
{
    testOriginalGrid();
    testCriticalShortage();
    testSamePriority();
    testZeroSupply();
    testZeroDemand();
    testEmptyProblem();
    testInputPreservation();
    testRepeatedCalls();
    testInvalidInput();
    testMaximumCapacity();

    printf("\n========================================\n");
    printf("ADVANCED PRIORITY DP TEST SUMMARY\n");
    printf("========================================\n");
    printf("Tests passed: %d/10\n", passed);

    return 0;
}
