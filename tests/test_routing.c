
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "graph.h"
#include "routing.h"

#define EPSILON 1e-9

/* Compare two finite floating-point values. */
static int approximatelyEqual(double a, double b) {
    return fabs(a - b) < EPSILON;
}

/* Load the original grid and build its adjacency list. */
static void setupNetwork(Graph *graph, AdjList *list) {
    assert(loadNetworkFromFile(graph, "data/network.txt") == 1);
    assert(buildAdjacencyList(graph, list) == 1);
    assert(validateAdjacencyList(graph, list) == 1);
}

/* Find an edge by its endpoint IDs. */
static int findEdgeIndex(
    const Graph *graph, int fromId, int toId
) {
    for (int i = 0; i < graph->edgeCount; i++) {
        const Edge *edge = &graph->edges[i];

        if ((edge->from == fromId && edge->to == toId) ||
            (edge->from == toId && edge->to == fromId)) {
            return i;
        }
    }

    return -1;
}

/* Test 1: All distances in the original network. */
static void testOriginalNetwork(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    assert(dijkstraShortestPaths(&graph, &list, 0, &result) == 1);

    const double expected[9] = {
        0.0, 10.0, 14.0, 12.0, 16.0,
        18.0, 24.0, 18.0, 20.0
    };

    assert(graph.nodeCount == 9);

    for (int i = 0; i < graph.nodeCount; i++) {
        assert(approximatelyEqual(result.distance[i], expected[i]));
    }

    printf("Test 1 passed: Original shortest distances.\n");
}

/* Test 2: Verify Hospital route and its parents. */
static void testHospitalRoute(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    assert(dijkstraShortestPaths(&graph, &list, 0, &result) == 1);

    assert(approximatelyEqual(result.distance[4], 16.0));
    assert(result.parent[4] == 1);
    assert(result.parent[1] == 0);
    assert(result.sourceIndex == 0);

    assert(displayShortestPath(&graph, &result, 4) == 1);

    printf("Test 2 passed: Hospital route.\n");
}

/* Test 3: Simulate failure of line 1-4. */
static void testLineFailure(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    int edgeIndex = findEdgeIndex(&graph, 1, 4);
    assert(edgeIndex >= 0);

    graph.edges[edgeIndex].active = 0;

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(dijkstraShortestPaths(&graph, &list, 0, &result) == 1);

    assert(isinf(result.distance[4]));
    assert(result.parent[4] == -1);
    assert(displayShortestPath(&graph, &result, 4) == 0);

    printf("Test 3 passed: Hospital unreachable after failure.\n");
}

/* Test 4: Restore the failed transmission line. */
static void testLineRestoration(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    int edgeIndex = findEdgeIndex(&graph, 1, 4);
    assert(edgeIndex >= 0);

    graph.edges[edgeIndex].active = 0;
    assert(buildAdjacencyList(&graph, &list) == 1);

    graph.edges[edgeIndex].active = 1;
    assert(buildAdjacencyList(&graph, &list) == 1);

    assert(dijkstraShortestPaths(&graph, &list, 0, &result) == 1);
    assert(approximatelyEqual(result.distance[4], 16.0));

    printf("Test 4 passed: Route restored.\n");
}

/* Test 5: Invalid source ID. */
static void testInvalidSource(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    assert(dijkstraShortestPaths(&graph, &list, 99, &result) == 0);

    printf("Test 5 passed: Invalid source rejected.\n");
}

/* Test 6: Node IDs are not array indices. */
static void testNoncontiguousIDs(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    initializeGraph(&graph);

    Node a = {10, "Generator", GENERATOR, 0, 100, 1};
    Node b = {25, "Substation", SUBSTATION, 0, 0, 2};
    Node c = {40, "Hospital", CONSUMER, 20, 0, 1};

    assert(addNode(&graph, a) == 1);
    assert(addNode(&graph, b) == 1);
    assert(addNode(&graph, c) == 1);

    Edge e1 = {10, 25, 5, 40, 4, 1};
    Edge e2 = {25, 40, 5, 30, 6, 1};

    assert(addEdge(&graph, e1) == 1);
    assert(addEdge(&graph, e2) == 1);

    assert(buildAdjacencyList(&graph, &list) == 1);

    assert(dijkstraShortestPaths(&graph, &list, 10, &result) == 1);

    assert(result.sourceIndex == 0);
    assert(approximatelyEqual(result.distance[2], 10.0));
    assert(result.parent[2] == 1);

    printf("Test 6 passed: Noncontiguous IDs.\n");
}

/* Test 7: A zero-cost transmission line. */
static void testZeroCostEdge(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    int edgeIndex = findEdgeIndex(&graph, 1, 4);
    assert(edgeIndex >= 0);

    graph.edges[edgeIndex].cost = 0.0;

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(dijkstraShortestPaths(&graph, &list, 0, &result) == 1);

    assert(approximatelyEqual(result.distance[4], 10.0));

    printf("Test 7 passed: Zero-cost edge.\n");
}

/* Test 8: Negative edge cost must be rejected. */
static void testNegativeCost(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    graph.edges[0].cost = -5.0;

    assert(dijkstraShortestPaths(&graph, &list, 0, &result) == 0);

    printf("Test 8 passed: Negative cost rejected.\n");
}

/* Test 9: Corrupted adjacency list. */
static void testInvalidAdjacency(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    list.entryCount = -1;

    assert(dijkstraShortestPaths(&graph, &list, 0, &result) == 0);

    printf("Test 9 passed: Invalid adjacency rejected.\n");
}

/* Test 10: Dijkstra must not modify the graph. */
static void testGraphPreservation(void) {
    Graph graph;
    Graph original;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    original = graph;

    assert(dijkstraShortestPaths(&graph, &list, 0, &result) == 1);

    assert(graph.nodeCount == original.nodeCount);
    assert(graph.edgeCount == original.edgeCount);

    for (int i = 0; i < graph.nodeCount; i++) {
        assert(graph.nodes[i].id == original.nodes[i].id);
        assert(graph.nodes[i].type == original.nodes[i].type);
        assert(graph.nodes[i].demand == original.nodes[i].demand);
        assert(graph.nodes[i].supply == original.nodes[i].supply);
        assert(graph.nodes[i].priority == original.nodes[i].priority);
        assert(strcmp(graph.nodes[i].name,
                      original.nodes[i].name) == 0);
    }

    for (int i = 0; i < graph.edgeCount; i++) {
        assert(graph.edges[i].from == original.edges[i].from);
        assert(graph.edges[i].to == original.edges[i].to);
        assert(graph.edges[i].distance == original.edges[i].distance);
        assert(graph.edges[i].capacity == original.edges[i].capacity);
        assert(graph.edges[i].cost == original.edges[i].cost);
        assert(graph.edges[i].active == original.edges[i].active);
    }

    printf("Test 10 passed: Original graph preserved.\n");
}


/* Test 11: Redundant network, all lines active. */
static void testRedundantNetwork(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    assert(loadNetworkFromFile(
        &graph, "data/network_redundant.txt") == 1);

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(graph.nodeCount == 9);
    assert(graph.edgeCount == 11);

    assert(dijkstraShortestPaths(
        &graph, &list, 0, &result) == 1);

    assert(approximatelyEqual(result.distance[4], 16.0));
    assert(result.parent[4] == 1);

    printf("Test 11 passed: Redundant network.\n");
}

/* Test 12: Reroute after line 1-4 fails. */
static void testAlternativeRoute(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    assert(loadNetworkFromFile(
        &graph, "data/network_redundant.txt") == 1);

    int edgeIndex = findEdgeIndex(&graph, 1, 4);
    assert(edgeIndex >= 0);

    graph.edges[edgeIndex].active = 0;

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(dijkstraShortestPaths(
        &graph, &list, 0, &result) == 1);

    assert(approximatelyEqual(result.distance[4], 23.0));
    assert(result.parent[4] == 2);
    assert(result.parent[2] == 0);

    assert(displayShortestPath(&graph, &result, 4) == 1);

    printf("Test 12 passed: Alternative route found.\n");
}

/* Test 13: Reroute after two line failures. */
static void testMultipleFailures(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    assert(loadNetworkFromFile(
        &graph, "data/network_redundant.txt") == 1);

    int first = findEdgeIndex(&graph, 1, 4);
    int second = findEdgeIndex(&graph, 0, 2);

    assert(first >= 0);
    assert(second >= 0);

    graph.edges[first].active = 0;
    graph.edges[second].active = 0;

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(dijkstraShortestPaths(
        &graph, &list, 0, &result) == 1);

    assert(approximatelyEqual(result.distance[4], 24.0));
    assert(result.parent[4] == 2);
    assert(result.parent[2] == 1);
    assert(result.parent[1] == 0);

    assert(displayShortestPath(&graph, &result, 4) == 1);

    printf("Test 13 passed: Multiple failures handled.\n");
}


/* Test 14: Hospital isolated by two failed lines. */
static void testHospitalIsolation(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    assert(loadNetworkFromFile(
        &graph, "data/network_redundant.txt") == 1);

    int first = findEdgeIndex(&graph, 1, 4);
    int second = findEdgeIndex(&graph, 2, 4);

    assert(first >= 0);
    assert(second >= 0);

    graph.edges[first].active = 0;
    graph.edges[second].active = 0;

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(dijkstraShortestPaths(
        &graph, &list, 0, &result) == 1);

    assert(isinf(result.distance[4]));
    assert(result.parent[4] == -1);

    printf("Test 14 passed: Hospital isolation.\n");
}

/* Test 15: Compare Dijkstra and Week 1 reachability. */
static void testReachabilityConsistency(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;
    int reachable[MAX_NODES];

    assert(loadNetworkFromFile(
        &graph, "data/network.txt") == 1);

    int edgeIndex = findEdgeIndex(&graph, 0, 1);
    assert(edgeIndex >= 0);

    graph.edges[edgeIndex].active = 0;

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    findGeneratorReachability(&graph, &list, reachable);

    assert(dijkstraShortestPaths(
        &graph, &list, 0, &result) == 1);

    for (int i = 0; i < graph.nodeCount; i++) {
        assert(reachable[i] ==
               (isfinite(result.distance[i]) ? 1 : 0));
    }

    assert(isinf(result.distance[4]));
    assert(isinf(result.distance[5]));

    printf("Test 15 passed: Reachability consistency.\n");
}

/* Test 16: Source-to-source shortest path. */
static void testSourceToSource(void) {
    Graph graph;
    AdjList list;
    DijkstraResult result;

    setupNetwork(&graph, &list);

    assert(dijkstraShortestPaths(
        &graph, &list, 0, &result) == 1);

    assert(approximatelyEqual(
        result.distance[result.sourceIndex], 0.0));

    assert(result.parent[result.sourceIndex] == -1);

    assert(displayShortestPath(&graph, &result, 0) == 1);

    printf("Test 16 passed: Source-to-source route.\n");
}


int main(void) {
    printf("\n========== DIJKSTRA TESTS ==========\n\n");

    testOriginalNetwork();
    testHospitalRoute();
    testLineFailure();
    testLineRestoration();
    testInvalidSource();
    testNoncontiguousIDs();
    testZeroCostEdge();
    testNegativeCost();
    testInvalidAdjacency();
    testGraphPreservation();
    testRedundantNetwork();
    testAlternativeRoute();
    testMultipleFailures();
    testHospitalIsolation();
    testReachabilityConsistency();
    testSourceToSource();

    printf("\nAll 16 Dijkstra tests passed!\n");

    return 0;
}
