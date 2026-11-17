
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "graph.h"
#include "mst.h"

#define EPSILON 1e-9

static int approximatelyEqual(double a, double b) {
    return fabs(a - b) < EPSILON;
}

static void loadTestNetwork(
    Graph *graph,
    AdjList *list,
    const char *filename
) {
    assert(loadNetworkFromFile(graph, filename) == 1);
    assert(buildAdjacencyList(graph, list) == 1);
    assert(validateAdjacencyList(graph, list) == 1);
}

/* Test 1: Original network is already a tree. */
static void testOriginalNetwork(void) {
    Graph graph;
    AdjList list;
    PrimResult result;

    loadTestNetwork(
        &graph, &list, "data/network.txt"
    );

    assert(primMinimumSpanningTree(
        &graph, &list, 0, &result) == 1);

    assert(result.isConnected == 1);
    assert(result.selectedEdges == 8);
    assert(approximatelyEqual(result.totalCost, 74.0));

    displayPrimMST(&graph, &result);

    printf("Test 1 passed: Original network MST.\n");
}

/* Test 2: Redundant network has a cheaper MST. */
static void testRedundantNetwork(void) {
    Graph graph;
    AdjList list;
    PrimResult result;

    loadTestNetwork(
        &graph, &list,
        "data/network_redundant.txt"
    );

    assert(primMinimumSpanningTree(
        &graph, &list, 0, &result) == 1);

    assert(result.isConnected == 1);
    assert(result.selectedEdges == 8);
    assert(approximatelyEqual(result.totalCost, 59.0));

    displayPrimMST(&graph, &result);

    printf("Test 2 passed: Redundant network MST.\n");
}

/* Test 3: Failure disconnects the original tree. */
static void testDisconnectedNetwork(void) {
    Graph graph;
    AdjList list;
    PrimResult result;

    loadTestNetwork(
        &graph, &list, "data/network.txt"
    );

    int found = 0;

    for (int i = 0; i < graph.edgeCount; i++) {
        if ((graph.edges[i].from == 1 &&
             graph.edges[i].to == 4) ||
            (graph.edges[i].from == 4 &&
             graph.edges[i].to == 1)) {

            graph.edges[i].active = 0;
            found = 1;
            break;
        }
    }

    assert(found == 1);

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(primMinimumSpanningTree(
        &graph, &list, 0, &result) == 1);

    assert(result.isConnected == 0);

    displayPrimMST(&graph, &result);

    printf("Test 3 passed: Disconnected network detected.\n");
}



/* Test 4: Kruskal on the original network. */
static void testKruskalOriginal(void) {
    Graph graph;
    AdjList list;
    KruskalResult result;

    loadTestNetwork(
        &graph, &list, "data/network.txt"
    );

    assert(kruskalMinimumSpanningTree(
        &graph, &list, &result) == 1);

    assert(result.isConnected == 1);
    assert(result.selectedEdges == 8);
    assert(approximatelyEqual(result.totalCost, 74.0));

    displayKruskalMST(&graph, &result);

    printf("Test 4 passed: Kruskal original network.\n");
}

/* Test 5: Kruskal on the redundant network. */
static void testKruskalRedundant(void) {
    Graph graph;
    AdjList list;
    KruskalResult result;

    loadTestNetwork(
        &graph, &list, "data/network_redundant.txt"
    );

    assert(kruskalMinimumSpanningTree(
        &graph, &list, &result) == 1);

    assert(result.isConnected == 1);
    assert(result.selectedEdges == 8);
    assert(approximatelyEqual(result.totalCost, 59.0));

    displayKruskalMST(&graph, &result);

    printf("Test 5 passed: Kruskal redundant network.\n");
}

/* Test 6: Kruskal detects disconnected networks. */
static void testKruskalDisconnected(void) {
    Graph graph;
    AdjList list;
    KruskalResult result;

    loadTestNetwork(
        &graph, &list, "data/network.txt"
    );

    /* Disconnect Hospital by failing line 1-4. */
    int found = 0;

    for (int i = 0; i < graph.edgeCount; i++) {
        if ((graph.edges[i].from == 1 &&
             graph.edges[i].to == 4) ||
            (graph.edges[i].from == 4 &&
             graph.edges[i].to == 1)) {

            graph.edges[i].active = 0;
            found = 1;
            break;
        }
    }

    assert(found == 1);
    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(kruskalMinimumSpanningTree(
        &graph, &list, &result) == 1);

    assert(result.isConnected == 0);
    assert(result.selectedEdges == 7);
    assert(approximatelyEqual(result.totalCost, 68.0));

    printf("Test 6 passed: Disconnected network.\n");
}

/* Test 7: Prim and Kruskal agree on MST cost. */
static void testPrimKruskalComparison(void) {
    Graph graph;
    AdjList list;
    PrimResult prim;
    KruskalResult kruskal;

    loadTestNetwork(
        &graph, &list, "data/network_redundant.txt"
    );

    assert(primMinimumSpanningTree(
        &graph, &list, 0, &prim) == 1);

    assert(kruskalMinimumSpanningTree(
        &graph, &list, &kruskal) == 1);

    assert(prim.isConnected == 1);
    assert(kruskal.isConnected == 1);

    assert(approximatelyEqual(
        prim.totalCost, kruskal.totalCost
    ));

    assert(prim.selectedEdges == kruskal.selectedEdges);

    printf("Test 7 passed: Prim and Kruskal agree.\n");
}



int main(void) {
    printf("\n========== MST TESTS ==========\n\n");

    /* Existing Prim tests */
    testOriginalNetwork();
    testRedundantNetwork();
    testDisconnectedNetwork();

    /* New Kruskal tests */
    testKruskalOriginal();
    testKruskalRedundant();
    testKruskalDisconnected();
    testPrimKruskalComparison();

    printf("\nAll 7 MST tests passed!\n");

    return 0;
}
