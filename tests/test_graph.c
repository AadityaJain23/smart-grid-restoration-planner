
#include <stdio.h>
#include <assert.h>
#include "graph.h"

// Test 1: Graph initialization
void testInitializeGraph() {

    Graph grid;
    initializeGraph(&grid);

    assert(grid.nodeCount == 0);
    assert(grid.edgeCount == 0);

    printf("PASS: initializeGraph()\n");
}

// Test 2: Valid node insertion
void testAddNode() {

    Graph grid;
    initializeGraph(&grid);

    Node generator = {
        0, "Power Station",
        GENERATOR, 0, 100, 1
    };

    assert(addNode(&grid, generator) == 1);
    assert(grid.nodeCount == 1);
    assert(grid.nodes[0].id == 0);

    printf("PASS: addNode()\n");
}

// Test 3: Duplicate node rejection
void testDuplicateNode() {

    Graph grid;
    initializeGraph(&grid);

    Node node1 = {
        1, "Substation A",
        SUBSTATION, 0, 0, 2
    };

    Node node2 = {
        1, "Substation B",
        SUBSTATION, 0, 0, 2
    };

    assert(addNode(&grid, node1) == 1);
    assert(addNode(&grid, node2) == 0);

    assert(grid.nodeCount == 1);

    printf("PASS: Duplicate node rejection\n");
}

// Test 4: Valid edge insertion
void testAddEdge() {

    Graph grid;
    initializeGraph(&grid);

    Node generator = {
        0, "Power Station",
        GENERATOR, 0, 100, 1
    };

    Node substation = {
        1, "Substation A",
        SUBSTATION, 0, 0, 2
    };

    assert(addNode(&grid, generator) == 1);
    assert(addNode(&grid, substation) == 1);

    Edge line = {
        0, 1, 5.0, 60.0, 10.0, 1
    };

    assert(addEdge(&grid, line) == 1);
    assert(grid.edgeCount == 1);

    printf("PASS: addEdge()\n");
}

// Test 5: Invalid edge endpoint
void testInvalidEdge() {

    Graph grid;
    initializeGraph(&grid);

    Node generator = {
        0, "Power Station",
        GENERATOR, 0, 100, 1
    };

    assert(addNode(&grid, generator) == 1);

    Edge invalidLine = {
        0, 99, 5.0, 60.0, 10.0, 1
    };

    assert(addEdge(&grid, invalidLine) == 0);
    assert(grid.edgeCount == 0);

    printf("PASS: Invalid edge rejection\n");
}

// Test 6: File loading
void testLoadNetwork() {

    Graph grid;
    initializeGraph(&grid);

    assert(loadNetworkFromFile(
        &grid, "data/network.txt") == 1);

    assert(grid.nodeCount == 9);
    assert(grid.edgeCount == 8);

    printf("PASS: loadNetworkFromFile()\n");
}



int calculateDegree(const Graph *graph, int nodeId) {

    int degree = 0;

    for (int i = 0; i < graph->edgeCount; i++) {

        Edge edge = graph->edges[i];

        if (edge.from == nodeId ||
            edge.to == nodeId) {

            degree++;
        }
    }

    return degree;
}


void testNodeDegrees() {

    Graph grid;
    initializeGraph(&grid);

    assert(loadNetworkFromFile(
        &grid, "data/network.txt") == 1);

    int expectedDegrees[9] = {
        3, 3, 2, 3, 1, 1, 1, 1, 1
    };

    for (int i = 0; i < 9; i++) {

        assert(calculateDegree(&grid, i)
               == expectedDegrees[i]);
    }

    printf("PASS: Node degree verification\n");
}



void testSupplyDemand() {

    Graph grid;
    initializeGraph(&grid);

    assert(loadNetworkFromFile(
        &grid, "data/network.txt") == 1);

    double totalSupply = 0;
    double totalDemand = 0;

    for (int i = 0; i < grid.nodeCount; i++) {

        totalSupply += grid.nodes[i].supply;
        totalDemand += grid.nodes[i].demand;
    }

    assert(totalSupply == 100.0);
    assert(totalDemand == 100.0);

    printf("PASS: Supply-demand verification\n");

    printf("Total Supply: %.2f MW\n", totalSupply);
    printf("Total Demand: %.2f MW\n", totalDemand);
}



void testLineFailure() {

    Graph grid;
    initializeGraph(&grid);

    assert(loadNetworkFromFile(
        &grid, "data/network.txt") == 1);

    // Simulate failure of edge 1 -> 4
    int found = 0;

    for (int i = 0; i < grid.edgeCount; i++) {

        Edge *edge = &grid.edges[i];

        if ((edge->from == 1 && edge->to == 4) ||
            (edge->from == 4 && edge->to == 1)) {

            edge->active = 0;
            found = 1;
            break;
        }
    }

    assert(found == 1);

    // Verify that the line is inactive
    for (int i = 0; i < grid.edgeCount; i++) {

        Edge edge = grid.edges[i];

        if ((edge.from == 1 && edge.to == 4) ||
            (edge.from == 4 && edge.to == 1)) {

            assert(edge.active == 0);
        }
    }

    printf("PASS: Transmission line failure simulation\n");
}

void testNegativeDemand() {

    Graph grid;
    initializeGraph(&grid);

    Node invalidNode = {
        5, "Invalid Consumer",
        CONSUMER, -20, 0, 3
    };

    assert(addNode(&grid, invalidNode) == 0);
    assert(grid.nodeCount == 0);

    printf("PASS: Negative demand rejection\n");
}

void testSelfLoop() {

    Graph grid;
    initializeGraph(&grid);

    Node node = {
        0, "Power Station",
        GENERATOR, 0, 100, 1
    };

    assert(addNode(&grid, node) == 1);

    Edge invalidLine = {
        0, 0, 5, 50, 10, 1
    };

    assert(addEdge(&grid, invalidLine) == 0);
    assert(grid.edgeCount == 0);

    printf("PASS: Self-loop rejection\n");
}

int main() {

    printf("\nSMART GRID UNIT TESTS\n\n");

    testInitializeGraph();
    testAddNode();
    testDuplicateNode();
    testAddEdge();
    testInvalidEdge();
    testLoadNetwork();

    printf("\nAll tests passed!\n");


    testLineFailure();
    testSupplyDemand();
    // calculateDegree(&grid, 0);
    testNodeDegrees();
    testSelfLoop();
    testNegativeDemand();


    return 0;
}
