
#include <assert.h>
#include <stdio.h>
#include "reliability.h"

static void addTestNode(Graph *graph, int id) {
    Node node = {0};

    node.id = id;
    snprintf(node.name, NAME_SIZE, "Node%d", id);
    node.type = SUBSTATION;
    node.priority = 1;

    assert(addNode(graph, node) == 1);
}

static void addTestEdge(
    Graph *graph,
    int from,
    int to
) {
    Edge edge = {0};

    edge.from = from;
    edge.to = to;
    edge.distance = 1.0;
    edge.capacity = 10.0;
    edge.cost = 1.0;
    edge.active = 1;

    assert(addEdge(graph, edge) == 1);
}

static void runTest(
    Graph *graph,
    int expectedBridges,
    int expectedArticulations
) {
    AdjList list;
    ReliabilityResult result = {0};

    assert(buildAdjacencyList(graph, &list) == 1);
    assert(validateAdjacencyList(graph, &list) == 1);

    assert(findCriticalBridges(
        graph, &list, &result
    ) == 1);

    assert(findArticulationPoints(
        graph, &list, &result
    ) == 1);

    assert(result.bridgeCount == expectedBridges);
    assert(result.articulationCount ==
           expectedArticulations);
}

static void testChain(void) {
    Graph graph;
    initializeGraph(&graph);

    for (int i = 0; i < 4; i++) {
        addTestNode(&graph, i * 10);
    }

    addTestEdge(&graph, 0, 10);
    addTestEdge(&graph, 10, 20);
    addTestEdge(&graph, 20, 30);

    runTest(&graph, 3, 2);
    printf("[PASS] Chain network\n");
}

static void testTriangle(void) {
    Graph graph;
    initializeGraph(&graph);

    addTestNode(&graph, 10);
    addTestNode(&graph, 20);
    addTestNode(&graph, 30);

    addTestEdge(&graph, 10, 20);
    addTestEdge(&graph, 20, 30);
    addTestEdge(&graph, 30, 10);

    runTest(&graph, 0, 0);
    printf("[PASS] Triangle network\n");
}

static void testTriangleWithLeaf(void) {
    Graph graph;
    initializeGraph(&graph);

    for (int i = 0; i < 4; i++) {
        addTestNode(&graph, i);
    }

    addTestEdge(&graph, 0, 1);
    addTestEdge(&graph, 1, 2);
    addTestEdge(&graph, 2, 0);
    addTestEdge(&graph, 1, 3);

    runTest(&graph, 1, 1);
    printf("[PASS] Triangle with leaf\n");
}

static void testInactiveEdge(void) {
    Graph graph;
    initializeGraph(&graph);

    for (int i = 0; i < 3; i++) {
        addTestNode(&graph, i);
    }

    addTestEdge(&graph, 0, 1);
    addTestEdge(&graph, 1, 2);
    addTestEdge(&graph, 2, 0);

    graph.edges[2].active = 0;

    runTest(&graph, 2, 1);
    printf("[PASS] Inactive edge\n");
}

int main(void) {
    testChain();
    testTriangle();
    testTriangleWithLeaf();
    testInactiveEdge();

    printf("\nAll initial reliability tests passed!\n");
    return 0;
}
