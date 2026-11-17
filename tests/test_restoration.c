
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include "graph.h"
#include "restoration.h"

#define EPSILON 1e-7


/* Add a node to an in-memory test network. */
static void addTestNode(
    Graph *graph,
    int id,
    const char *name,
    NodeType type,
    double demand,
    double supply,
    int priority
) {
    Node node = {0};

    node.id = id;
    node.type = type;
    node.demand = demand;
    node.supply = supply;
    node.priority = priority;

    snprintf(node.name, sizeof(node.name),
             "%s", name);

    assert(addNode(graph, node) == 1);
}

/* Add an active transmission line. */
static void addTestLine(
    Graph *graph,
    int from,
    int to,
    double capacity,
    double cost
) {
    Edge edge = {0};

    edge.from = from;
    edge.to = to;
    edge.distance = 1.0;
    edge.capacity = capacity;
    edge.cost = cost;
    edge.active = 1;

    assert(addEdge(graph, edge) == 1);
}

/* Build and validate the adjacency list. */
static void prepareTestAdjacency(
    const Graph *graph,
    AdjList *list
) {
    assert(buildAdjacencyList(graph, list) == 1);
    assert(validateAdjacencyList(graph, list) == 1);
}


static int equalDouble(double a, double b) {
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

/* Verify fundamental allocation constraints. */
static void verifyAllocationLimits(
    const Graph *graph,
    const RestorationResult *result
) {
    double allocatedTotal = 0.0;

    for (int i = 0; i < graph->nodeCount; i++) {
        const Node *node = &graph->nodes[i];

        if (node->type == CONSUMER) {
            assert(result->allocated[i] >= 0.0);
            assert(result->allocated[i] <=
                   node->demand + EPSILON);

            allocatedTotal += result->allocated[i];
        }

        if (node->type == GENERATOR) {
            assert(result->remainingSupply[i] >= -EPSILON);
            assert(result->remainingSupply[i] <=
                   node->supply + EPSILON);
        }
    }

    for (int e = 0; e < graph->edgeCount; e++) {
        const Edge *edge = &graph->edges[e];

        assert(result->edgeAllocated[e] >= -EPSILON);
        assert(result->residualCapacity[e] >= -EPSILON);

        assert(result->edgeAllocated[e] <=
               edge->capacity + EPSILON);

        if (edge->active == 0) {
            assert(equalDouble(
                result->edgeAllocated[e], 0.0));
        }
    }

    assert(equalDouble(
        allocatedTotal, result->totalAllocated));

    assert(equalDouble(
        result->totalDemand,
        result->totalAllocated + result->totalUnserved));

    
/* Check generator supply conservation. */
    double totalSupplyUsed = 0.0;

    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i].type == GENERATOR) {
            totalSupplyUsed +=
                graph->nodes[i].supply -
                result->remainingSupply[i];
        }
    }

    assert(equalDouble(
        totalSupplyUsed, result->totalAllocated
    ));

    /* Check edge capacity accounting. */
    for (int e = 0; e < graph->edgeCount; e++) {
        double initialCapacity =
            graph->edges[e].active
            ? graph->edges[e].capacity
            : 0.0;

        assert(equalDouble(
            result->edgeAllocated[e] +
            result->residualCapacity[e],
            initialCapacity
        ));
    }

}

/* Test 1: Original network can serve all consumers. */
static void testOriginalNetwork(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    loadTestNetwork(&graph, &list, "data/network.txt");

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.totalDemand, 100.0));
    assert(equalDouble(result.totalAllocated, 100.0));
    assert(equalDouble(result.totalUnserved, 0.0));

    assert(result.fullyServedConsumers == 5);

    verifyAllocationLimits(&graph, &result);

    displayRestorationReport(&graph, &result);

    printf("Test 1 passed: Original allocation.\n");
}

/* Test 2: Critical loads are served first. */
static void testReducedSupply(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    loadTestNetwork(&graph, &list, "data/network.txt");

    graph.nodes[0].supply = 30.0;

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[4], 20.0));
    assert(equalDouble(result.allocated[7], 10.0));

    assert(equalDouble(result.totalAllocated, 30.0));
    assert(equalDouble(result.totalUnserved, 70.0));

    verifyAllocationLimits(&graph, &result);

    printf("Test 2 passed: Priority allocation.\n");
}

/* Test 3: Failed Hospital line prevents allocation. */
static void testHospitalFailure(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    loadTestNetwork(&graph, &list, "data/network.txt");

    graph.edges[3].active = 0;

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[4], 0.0));
    assert(equalDouble(result.totalAllocated, 80.0));
    assert(equalDouble(result.totalUnserved, 20.0));

    verifyAllocationLimits(&graph, &result);

    printf("Test 3 passed: Hospital failure.\n");
}

/* Test 4: Redundant network supports rerouting. */
static void testRedundantRestoration(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    loadTestNetwork(
        &graph, &list, "data/network_redundant.txt"
    );

    /* Original edge 1-4. */
    graph.edges[3].active = 0;

    assert(buildAdjacencyList(&graph, &list) == 1);
    assert(validateAdjacencyList(&graph, &list) == 1);

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[4], 20.0));

    verifyAllocationLimits(&graph, &result);

    printf("Test 4 passed: Redundant restoration.\n");
}

/* Test 5: Invalid adjacency list is rejected. */
static void testInvalidAdjacency(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    loadTestNetwork(&graph, &list, "data/network.txt");

    list.entryCount = -1;

    assert(planPowerRestoration(
        &graph, &list, &result) == 0);

    printf("Test 5 passed: Invalid adjacency rejected.\n");
}

/* Test 6: NULL pointers are rejected. */
static void testNullInputs(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    assert(planPowerRestoration(
        NULL, &list, &result) == 0);

    assert(planPowerRestoration(
        &graph, NULL, &result) == 0);

    assert(planPowerRestoration(
        &graph, &list, NULL) == 0);

    printf("Test 6 passed: NULL input handling.\n");
}


static void testMultipleGenerators(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    initializeGraph(&graph);

    addTestNode(&graph, 10, "Generator_A",
                GENERATOR, 0, 20, 1);
    addTestNode(&graph, 20, "Generator_B",
                GENERATOR, 0, 30, 1);
    addTestNode(&graph, 30, "Substation",
                SUBSTATION, 0, 0, 2);
    addTestNode(&graph, 40, "Hospital",
                CONSUMER, 40, 0, 1);

    addTestLine(&graph, 10, 30, 20, 2);
    addTestLine(&graph, 20, 30, 30, 3);
    addTestLine(&graph, 30, 40, 50, 1);

    prepareTestAdjacency(&graph, &list);

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[3], 40));
    assert(equalDouble(result.totalAllocated, 40));

    assert(equalDouble(result.remainingSupply[0], 0));
    assert(equalDouble(result.remainingSupply[1], 10));

    verifyAllocationLimits(&graph, &result);

    printf("Test 7 passed: Multiple generators.\n");
}


static void testSharedLineSaturation(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    initializeGraph(&graph);

    addTestNode(&graph, 0, "Generator",
                GENERATOR, 0, 100, 1);
    addTestNode(&graph, 1, "Substation",
                SUBSTATION, 0, 0, 2);
    addTestNode(&graph, 2, "Critical_Load",
                CONSUMER, 20, 0, 1);
    addTestNode(&graph, 3, "Normal_Load",
                CONSUMER, 20, 0, 3);

    addTestLine(&graph, 0, 1, 30, 1);
    addTestLine(&graph, 1, 2, 20, 1);
    addTestLine(&graph, 1, 3, 20, 1);

    prepareTestAdjacency(&graph, &list);

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[2], 20));
    assert(equalDouble(result.allocated[3], 10));

    assert(equalDouble(result.totalAllocated, 30));
    assert(equalDouble(result.totalUnserved, 10));

    assert(equalDouble(result.residualCapacity[0], 0));
    assert(equalDouble(result.edgeAllocated[0], 30));

    verifyAllocationLimits(&graph, &result);

    printf("Test 8 passed: Shared line saturation.\n");
}


static void testPartialAllocation(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    initializeGraph(&graph);

    addTestNode(&graph, 0, "Generator",
                GENERATOR, 0, 50, 1);
    addTestNode(&graph, 1, "Hospital",
                CONSUMER, 40, 0, 1);

    addTestLine(&graph, 0, 1, 15, 2);

    prepareTestAdjacency(&graph, &list);

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[1], 15));
    assert(equalDouble(result.remainingDemand[1], 25));

    assert(equalDouble(result.totalAllocated, 15));
    assert(equalDouble(result.totalUnserved, 25));

    assert(result.partiallyServedConsumers == 1);
    assert(result.fullyServedConsumers == 0);

    verifyAllocationLimits(&graph, &result);

    printf("Test 9 passed: Partial allocation.\n");
}


static void testNoncontiguousIDs(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    initializeGraph(&graph);

    addTestNode(&graph, 100, "Generator",
                GENERATOR, 0, 40, 1);
    addTestNode(&graph, 250, "Substation",
                SUBSTATION, 0, 0, 2);
    addTestNode(&graph, 900, "Hospital",
                CONSUMER, 20, 0, 1);

    addTestLine(&graph, 100, 250, 30, 2);
    addTestLine(&graph, 250, 900, 25, 3);

    prepareTestAdjacency(&graph, &list);

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[2], 20));
    assert(equalDouble(result.totalAllocated, 20));

    verifyAllocationLimits(&graph, &result);

    printf("Test 10 passed: Noncontiguous IDs.\n");
}


static void testNetworkPreservation(void) {
    Graph graph;
    Graph original;
    AdjList list;
    AdjList originalList;
    RestorationResult result;

    loadTestNetwork(
        &graph, &list, "data/network_redundant.txt"
    );

    original = graph;
    originalList = list;

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(graph.nodeCount == original.nodeCount);
    assert(graph.edgeCount == original.edgeCount);
    assert(list.entryCount == originalList.entryCount);

    for (int i = 0; i < graph.nodeCount; i++) {
        assert(graph.nodes[i].id == original.nodes[i].id);
        assert(graph.nodes[i].type == original.nodes[i].type);
        assert(graph.nodes[i].demand == original.nodes[i].demand);
        assert(graph.nodes[i].supply == original.nodes[i].supply);
        assert(graph.nodes[i].priority ==
               original.nodes[i].priority);

        assert(strcmp(graph.nodes[i].name,
                      original.nodes[i].name) == 0);

        assert(list.head[i] == originalList.head[i]);
    }

    for (int i = 0; i < graph.edgeCount; i++) {
        assert(graph.edges[i].from == original.edges[i].from);
        assert(graph.edges[i].to == original.edges[i].to);
        assert(graph.edges[i].distance ==
               original.edges[i].distance);
        assert(graph.edges[i].capacity ==
               original.edges[i].capacity);
        assert(graph.edges[i].cost == original.edges[i].cost);
        assert(graph.edges[i].active ==
               original.edges[i].active);
    }

    for (int i = 0; i < list.entryCount; i++) {
        assert(list.entries[i].neighborIndex ==
               originalList.entries[i].neighborIndex);
        assert(list.entries[i].edgeIndex ==
               originalList.entries[i].edgeIndex);
        assert(list.entries[i].next ==
               originalList.entries[i].next);
    }

    printf("Test 11 passed: Network preservation.\n");
}


static void testInvalidCapacity(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    loadTestNetwork(&graph, &list, "data/network.txt");

    graph.edges[0].capacity = -10.0;

    assert(planPowerRestoration(
        &graph, &list, &result) == 0);

    printf("Test 12 passed: Invalid capacity rejected.\n");
}


static void testMultiplePaths(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    initializeGraph(&graph);

    addTestNode(&graph, 0, "Generator",
                GENERATOR, 0, 50, 1);
    addTestNode(&graph, 1, "Substation_A",
                SUBSTATION, 0, 0, 2);
    addTestNode(&graph, 2, "Substation_B",
                SUBSTATION, 0, 0, 2);
    addTestNode(&graph, 3, "Hospital",
                CONSUMER, 30, 0, 1);

    addTestLine(&graph, 0, 1, 15, 1);
    addTestLine(&graph, 1, 3, 15, 1);

    addTestLine(&graph, 0, 2, 20, 3);
    addTestLine(&graph, 2, 3, 20, 3);

    prepareTestAdjacency(&graph, &list);

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[3], 30));
    assert(equalDouble(result.totalAllocated, 30));
    assert(equalDouble(result.totalUnserved, 0));

    assert(equalDouble(result.edgeAllocated[0], 15));
    assert(equalDouble(result.edgeAllocated[1], 15));

    assert(equalDouble(result.edgeAllocated[2], 15));
    assert(equalDouble(result.edgeAllocated[3], 15));

    verifyAllocationLimits(&graph, &result);

    printf("Test 13 passed: Multiple-path allocation.\n");
}


static void testPriorityUnderShortage(void) {
    Graph graph;
    AdjList list;
    RestorationResult result;

    initializeGraph(&graph);

    addTestNode(&graph, 0, "Generator",
                GENERATOR, 0, 30, 1);
    addTestNode(&graph, 1, "Substation",
                SUBSTATION, 0, 0, 2);

    /* Insert lower-priority consumer first. */
    addTestNode(&graph, 2, "Residential",
                CONSUMER, 25, 0, 3);
    addTestNode(&graph, 3, "School",
                CONSUMER, 15, 0, 2);
    addTestNode(&graph, 4, "Hospital",
                CONSUMER, 20, 0, 1);

    addTestLine(&graph, 0, 1, 60, 1);
    addTestLine(&graph, 1, 2, 30, 1);
    addTestLine(&graph, 1, 3, 30, 1);
    addTestLine(&graph, 1, 4, 30, 1);

    prepareTestAdjacency(&graph, &list);

    assert(planPowerRestoration(
        &graph, &list, &result) == 1);

    assert(equalDouble(result.allocated[4], 20));
    assert(equalDouble(result.allocated[3], 10));
    assert(equalDouble(result.allocated[2], 0));

    assert(equalDouble(result.totalAllocated, 30));
    assert(equalDouble(result.totalUnserved, 30));

    assert(result.fullyServedConsumers == 1);
    assert(result.partiallyServedConsumers == 1);
    assert(result.unservedConsumers == 1);

    verifyAllocationLimits(&graph, &result);

    printf("Test 14 passed: Priority under shortage.\n");
}




int main(void) {
    printf("\n========== RESTORATION TESTS ==========\n\n");

    /* Existing tests */
    testOriginalNetwork();
    testReducedSupply();
    testHospitalFailure();
    testRedundantRestoration();
    testInvalidAdjacency();
    testNullInputs();

    /* New advanced tests */
    testMultipleGenerators();
    testSharedLineSaturation();
    testPartialAllocation();
    testNoncontiguousIDs();
    testNetworkPreservation();
    testInvalidCapacity();
    testMultiplePaths();
    testPriorityUnderShortage();

    printf("\nAll 14 restoration tests passed!\n");

    return 0;
}

