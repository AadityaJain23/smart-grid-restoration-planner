
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <limits.h>

#include "dp.h"

static int testsPassed = 0;

static void pass(const char *name)
{
    testsPassed++;
    printf("[PASS] %s\n", name);
}

static DPProblem makeProblem(int capacity)
{
    DPProblem p = {0};
    p.capacity = capacity;
    return p;
}

static void addConsumer(
    DPProblem *p,
    int id,
    int demand,
    int benefit,
    int priority
)
{
    assert(p != NULL);
    assert(p->consumerCount < DP_MAX_CONSUMERS);

    int i = p->consumerCount++;

    p->consumers[i] = (DPConsumer){
        id, demand, benefit, priority
    };
}

/* Test 1: Basic Knapsack */
static void testBasic(void)
{
    DPProblem p = makeProblem(5);
    DPResult r = {0};

    addConsumer(&p, 10, 2, 3, 1);
    addConsumer(&p, 20, 3, 4, 2);
    addConsumer(&p, 30, 4, 5, 3);

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 7);
    assert(r.allocatedPower == 5);
    assert(r.remainingPower == 0);
    assert(r.selectedCount == 2);
    assert(r.selected[0] == 1);
    assert(r.selected[1] == 0);

    pass("Basic Knapsack");
}

/* Test 2: Zero supply */
static void testZeroSupply(void)
{
    DPProblem p = makeProblem(0);
    DPResult r = {0};

    addConsumer(&p, 10, 5, 20, 1);

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 0);
    assert(r.allocatedPower == 0);
    assert(r.selectedCount == 0);

    pass("Zero supply");
}

/* Test 3: Empty consumer list */
static void testEmptyProblem(void)
{
    DPProblem p = makeProblem(50);
    DPResult r = {0};

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 0);
    assert(r.allocatedPower == 0);
    assert(r.remainingPower == 50);
    assert(r.selectedCount == 0);

    pass("Empty consumer list");
}

/* Test 4: Demand exceeds capacity */
static void testOversizedConsumer(void)
{
    DPProblem p = makeProblem(10);
    DPResult r = {0};

    addConsumer(&p, 10, 20, 100, 1);

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 0);
    assert(r.selectedCount == 0);

    pass("Oversized consumer");
}

/* Test 5: Supply exceeds total demand */
static void testExcessSupply(void)
{
    DPProblem p = makeProblem(100);
    DPResult r = {0};

    addConsumer(&p, 10, 20, 50, 1);
    addConsumer(&p, 20, 30, 40, 2);

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 90);
    assert(r.allocatedPower == 50);
    assert(r.remainingPower == 50);
    assert(r.selectedCount == 2);

    pass("Excess supply");
}

/* Test 6: Equal-benefit tie */
static void testEqualBenefit(void)
{
    DPProblem p = makeProblem(5);
    DPResult r = {0};

    addConsumer(&p, 10, 5, 10, 1);
    addConsumer(&p, 20, 5, 10, 2);

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 10);
    assert(r.selectedCount == 1);
    assert(r.selected[0] == 0);

    pass("Deterministic equal-benefit tie");
}

/* Test 7: Zero-demand beneficial consumer */
static void testZeroDemand(void)
{
    DPProblem p = makeProblem(0);
    DPResult r = {0};

    addConsumer(&p, 10, 0, 15, 1);

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 15);
    assert(r.allocatedPower == 0);
    assert(r.selectedCount == 1);
    assert(r.selected[0] == 0);

    pass("Zero-demand beneficial consumer");
}

/* Test 8: Duplicate node IDs */
static void testDuplicateIds(void)
{
    DPProblem p = makeProblem(20);
    DPResult r = {0};

    addConsumer(&p, 10, 5, 10, 1);
    addConsumer(&p, 10, 6, 12, 2);

    assert(solveKnapsack(&p, &r) == 0);

    pass("Duplicate node IDs rejected");
}

/* Test 9: Negative demand */
static void testNegativeDemand(void)
{
    DPProblem p = makeProblem(20);
    DPResult r = {0};

    addConsumer(&p, 10, -5, 10, 1);

    assert(solveKnapsack(&p, &r) == 0);

    pass("Negative demand rejected");
}

/* Test 10: Negative benefit */
static void testNegativeBenefit(void)
{
    DPProblem p = makeProblem(20);
    DPResult r = {0};

    addConsumer(&p, 10, 5, -10, 1);

    assert(solveKnapsack(&p, &r) == 0);

    pass("Negative benefit rejected");
}

/* Test 11: Capacity above limit */
static void testInvalidCapacity(void)
{
    DPProblem p = makeProblem(DP_MAX_CAPACITY + 1);
    DPResult r = {0};

    assert(solveKnapsack(&p, &r) == 0);

    pass("Capacity limit validation");
}

/* Test 12: Repeated calls */
static void testRepeatedCalls(void)
{
    DPProblem p = makeProblem(10);
    DPResult r = {0};

    addConsumer(&p, 10, 6, 30, 1);
    addConsumer(&p, 20, 5, 24, 2);
    addConsumer(&p, 30, 5, 24, 3);

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 48);

    p.capacity = 6;

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 30);
    assert(r.allocatedPower == 6);

    pass("Repeated solver calls");
}

/* Test 13: Input preservation */
static void testInputPreservation(void)
{
    DPProblem p = makeProblem(10);
    DPProblem original;
    DPResult r = {0};

    addConsumer(&p, 10, 4, 12, 1);
    addConsumer(&p, 20, 6, 15, 2);

    original = p;

    assert(solveKnapsack(&p, &r) == 1);

    assert(memcmp(&p, &original, sizeof(p)) == 0);

    pass("Input preservation");
}

/* Test 14: Integer overflow */
static void testIntegerOverflow(void)
{
    DPProblem p = makeProblem(2);
    DPResult r = {0};

    addConsumer(&p, 10, 1, INT_MAX, 1);
    addConsumer(&p, 20, 1, 1, 2);

    assert(solveKnapsack(&p, &r) == 0);

    pass("Integer overflow protection");
}

/* Test 15: Maximum capacity */
static void testMaximumCapacity(void)
{
    DPProblem p = makeProblem(DP_MAX_CAPACITY);
    DPResult r = {0};

    addConsumer(&p, 10, DP_MAX_CAPACITY, 100, 1);

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 100);
    assert(r.allocatedPower == DP_MAX_CAPACITY);

    pass("Maximum capacity boundary");
}

/* Test 16: Maximum consumer count */
static void testMaximumConsumers(void)
{
    DPProblem p = makeProblem(0);
    DPResult r = {0};

    for (int i = 0; i < DP_MAX_CONSUMERS; i++) {
        addConsumer(&p, 100 + i, 1, 1, 3);
    }

    assert(solveKnapsack(&p, &r) == 1);
    assert(r.maximumBenefit == 0);
    assert(r.selectedCount == 0);

    pass("Maximum consumer count");
}

/* Test 17: Invalid priority */
static void testInvalidPriority(void)
{
    DPProblem p = makeProblem(10);
    DPResult r = {0};

    addConsumer(&p, 10, 5, 10, 0);

    assert(solveKnapsack(&p, &r) == 0);

    pass("Invalid priority rejected");
}

int main(void)
{
    testBasic();
    testZeroSupply();
    testEmptyProblem();
    testOversizedConsumer();
    testExcessSupply();
    testEqualBenefit();
    testZeroDemand();
    testDuplicateIds();
    testNegativeDemand();
    testNegativeBenefit();
    testInvalidCapacity();
    testRepeatedCalls();
    testInputPreservation();
    testIntegerOverflow();
    testMaximumCapacity();
    testMaximumConsumers();
    testInvalidPriority();

    printf("\n========================================\n");
    printf("DP TEST SUMMARY\n");
    printf("========================================\n");
    printf("Tests passed: %d/17\n", testsPassed);

    return 0;
}
