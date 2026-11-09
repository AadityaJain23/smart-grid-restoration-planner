
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "sorting.h"
#include "searching.h"
#include "divide_conquer.h"

static void testWeek2Integration(void) {

    Graph graph = {0};

    graph.nodeCount = 5;
    graph.edgeCount = 4;

    /* Node IDs deliberately differ from array indices */
    int ids[] = {100, 20, 70, 40, 10};

    NodeType types[] = {
        GENERATOR,
        SUBSTATION,
        CONSUMER,
        CONSUMER,
        CONSUMER
    };

    double demands[] = {
        0, 0, 30, 10, 20
    };

    const char *names[] = {
        "Generator",
        "Substation",
        "Industrial",
        "School",
        "Hospital"
    };

    for (int i = 0; i < graph.nodeCount; i++) {

        graph.nodes[i].id = ids[i];
        graph.nodes[i].type = types[i];
        graph.nodes[i].demand = demands[i];

        graph.nodes[i].supply =
            (i == 0) ? 100.0 : 0.0;

        graph.nodes[i].priority =
            (i == 4) ? 1 : 2;

        strcpy(graph.nodes[i].name, names[i]);
    }

    double costs[] = {12, 6, 10, 8};

    for (int i = 0; i < graph.edgeCount; i++) {

        graph.edges[i].from = 100;
        graph.edges[i].to = ids[i + 1];
        graph.edges[i].capacity = 50.0;
        graph.edges[i].distance = 5.0;
        graph.edges[i].cost = costs[i];
        graph.edges[i].active = 1;
    }

    /* Preserve original records */
    Graph original = graph;

    /* Test Merge Sort */
    int consumerIndices[MAX_NODES];

    int consumerCount = extractConsumerIndices(
        &graph, consumerIndices
    );

    assert(consumerCount == 3);

    mergeSortConsumers(
        &graph,
        consumerIndices,
        0,
        consumerCount - 1
    );

    assert(graph.nodes[consumerIndices[0]].demand == 10);
    assert(graph.nodes[consumerIndices[1]].demand == 20);
    assert(graph.nodes[consumerIndices[2]].demand == 30);

    printf("Integration: Merge Sort passed.\n");

    /* Test Quick Sort */
    int edgeIndices[MAX_EDGES];

    int edgeCount = extractEdgeIndices(
        &graph, edgeIndices
    );

    assert(edgeCount == 4);

    quickSortEdges(
        &graph,
        edgeIndices,
        0,
        edgeCount - 1
    );

    assert(graph.edges[edgeIndices[0]].cost == 6);
    assert(graph.edges[edgeIndices[1]].cost == 8);
    assert(graph.edges[edgeIndices[2]].cost == 10);
    assert(graph.edges[edgeIndices[3]].cost == 12);

    printf("Integration: Quick Sort passed.\n");

    /* Test Binary Search */
    int sortedNodes[MAX_NODES];

    int count = prepareSortedNodeIndices(
        &graph, sortedNodes
    );

    assert(count == 5);

    int iterative = iterativeBinarySearchNode(
        &graph, sortedNodes, count, 40
    );

    int recursive = recursiveBinarySearchNode(
        &graph, sortedNodes, count, 40
    );

    assert(iterative == 3);
    assert(recursive == 3);

    printf("Integration: Binary Search passed.\n");

    /* Test Maximum Subarray */
    double changes[] = {-4, 7, 3, -2, 8, -10, 4};

    SubarrayResult subarray;

    assert(maximumSubarray(
        changes, 7, &subarray
    ) == 1);

    assert(subarray.sum == 16);
    assert(subarray.start == 1);
    assert(subarray.end == 4);

    printf("Integration: Maximum Subarray passed.\n");

    /* Test Fast Exponentiation */
    double projection;

    assert(projectDemand(
        100.0, 0.05, 5U, &projection
    ) == 1);

    assert(fabs(projection - 127.62815625) < 1e-8);

    printf("Integration: Fast Exponentiation passed.\n");

    /* Confirm original graph data is unchanged */
    for (int i = 0; i < graph.nodeCount; i++) {

        assert(graph.nodes[i].id ==
               original.nodes[i].id);

        assert(graph.nodes[i].demand ==
               original.nodes[i].demand);
    }

    for (int i = 0; i < graph.edgeCount; i++) {

        assert(graph.edges[i].from ==
               original.edges[i].from);

        assert(graph.edges[i].to ==
               original.edges[i].to);

        assert(graph.edges[i].cost ==
               original.edges[i].cost);
    }

    printf("Integration: Graph integrity passed.\n");

    printf("\nAll Week 2 integration tests passed.\n");
}

int main(void) {

    testWeek2Integration();

    return 0;
}
