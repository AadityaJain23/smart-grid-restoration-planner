
#include <stdio.h>
#include <assert.h>

#include "dsu.h"

/* Test 1: Every element starts separately. */
static void testInitialization(void) {
    DSU dsu;

    assert(dsuInitialize(&dsu, 5) == 1);

    for (int i = 0; i < 5; i++) {
        assert(dsuFind(&dsu, i) == i);
        assert(dsu.rank[i] == 0);
    }

    printf("Test 1 passed: Initialization.\n");
}

/* Test 2: Union merges separate components. */
static void testUnion(void) {
    DSU dsu;

    assert(dsuInitialize(&dsu, 5) == 1);

    assert(dsuUnion(&dsu, 0, 1) == 1);
    assert(dsuUnion(&dsu, 1, 2) == 1);

    assert(dsuConnected(&dsu, 0, 2) == 1);
    assert(dsuConnected(&dsu, 0, 3) == 0);

    printf("Test 2 passed: Union.\n");
}

/* Test 3: Repeated union detects an existing connection. */
static void testCycleDetection(void) {
    DSU dsu;

    assert(dsuInitialize(&dsu, 4) == 1);

    assert(dsuUnion(&dsu, 0, 1) == 1);
    assert(dsuUnion(&dsu, 1, 2) == 1);

    /* 0 and 2 are already in the same component. */
    assert(dsuUnion(&dsu, 0, 2) == 0);

    printf("Test 3 passed: Cycle detection.\n");
}

/* Test 4: Path compression shortens parent chains. */
static void testPathCompression(void) {
    DSU dsu;

    assert(dsuInitialize(&dsu, 5) == 1);

    assert(dsuUnion(&dsu, 0, 1) == 1);
    assert(dsuUnion(&dsu, 2, 3) == 1);
    assert(dsuUnion(&dsu, 0, 2) == 1);

    /*
     * Root 0 has rank 2.
     * Node 3 initially points to 2.
     */
    assert(dsu.parent[3] == 2);

    assert(dsuFind(&dsu, 3) == 0);
    assert(dsu.parent[3] == 0);

    printf("Test 4 passed: Path compression.\n");
}

/* Test 5: Union by rank chooses the taller root. */
static void testUnionByRank(void) {
    DSU dsu;

    assert(dsuInitialize(&dsu, 5) == 1);

    assert(dsuUnion(&dsu, 0, 1) == 1);
    assert(dsuUnion(&dsu, 0, 2) == 1);

    assert(dsu.rank[0] == 1);

    assert(dsuUnion(&dsu, 3, 0) == 1);

    /* Root 3 must attach beneath root 0. */
    assert(dsuFind(&dsu, 3) == 0);
    assert(dsu.rank[0] == 1);

    printf("Test 5 passed: Union by rank.\n");
}

/* Test 6: Invalid input is rejected. */
static void testInvalidInput(void) {
    DSU dsu;

    assert(dsuInitialize(NULL, 5) == 0);
    assert(dsuInitialize(&dsu, 0) == 0);
    assert(dsuInitialize(&dsu, MAX_NODES + 1) == 0);

    assert(dsuInitialize(&dsu, 5) == 1);

    assert(dsuFind(&dsu, -1) == -1);
    assert(dsuFind(&dsu, 5) == -1);
    assert(dsuUnion(&dsu, 0, 99) == -1);
    assert(dsuConnected(&dsu, -1, 2) == -1);

    printf("Test 6 passed: Invalid input.\n");
}

/* Test 7: DSU handles maximum supported size. */
static void testMaximumSize(void) {
    DSU dsu;

    assert(dsuInitialize(&dsu, MAX_NODES) == 1);

    for (int i = 1; i < MAX_NODES; i++) {
        assert(dsuUnion(&dsu, 0, i) == 1);
    }

    for (int i = 0; i < MAX_NODES; i++) {
        assert(dsuConnected(&dsu, 0, i) == 1);
    }

    printf("Test 7 passed: Maximum size.\n");
}

int main(void) {
    printf("\n========== DSU TESTS ==========\n\n");

    testInitialization();
    testUnion();
    testCycleDetection();
    testPathCompression();
    testUnionByRank();
    testInvalidInput();
    testMaximumSize();

    printf("\nAll 7 DSU tests passed!\n");

    return 0;
}
