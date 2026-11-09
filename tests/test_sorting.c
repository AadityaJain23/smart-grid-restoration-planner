#include <assert.h>
#include <stdio.h>
#include "sorting.h"

void testQuickSort(void) {

    Graph graph = {0};

    graph.edgeCount = 8;

    double costs[] = {
        10, 14, 12, 6, 8, 10, 6, 8
    };

    for (int i = 0; i < graph.edgeCount; i++) {
        graph.edges[i].from = i;
        graph.edges[i].to = i + 1;
        graph.edges[i].capacity = 50;
        graph.edges[i].cost = costs[i];
        graph.edges[i].active = 1;
    }

    int indices[MAX_EDGES];

    int count = extractEdgeIndices(
        &graph, indices
    );

    assert(count == 8);

    quickSortEdges(
        &graph,
        indices,
        0,
        count - 1
    );

    double expected[] = {
        6, 6, 8, 8, 10, 10, 12, 14
    };

    for (int i = 0; i < count; i++) {

        double actual =
            graph.edges[indices[i]].cost;

        assert(actual == expected[i]);
    }

    /* Verify original edge records are unchanged */
    for (int i = 0; i < graph.edgeCount; i++) {
        assert(graph.edges[i].cost == costs[i]);
    }

    printf("Quick Sort tests passed.\n");

    displaySortedEdges(
        &graph,
        indices,
        count
    );
}

int main(void) {

    Graph graph = {0};

    graph.nodeCount = 5;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = 25;

    graph.nodes[2].id = 30;
    graph.nodes[2].type = SUBSTATION;

    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 10;

    graph.nodes[4].id = 50;
    graph.nodes[4].type = CONSUMER;
    graph.nodes[4].demand = 20;

    int indices[MAX_NODES];

    int count = extractConsumerIndices(
        &graph, indices
    );

    assert(count == 3);

    assert(indices[0] == 1);
    assert(indices[1] == 3);
    assert(indices[2] == 4);

    assert(graph.nodes[1].id == 20);
    assert(graph.nodes[3].id == 40);

    printf("Consumer extraction tests passed.\n");

    displayConsumerOrder(
        &graph, indices, count
    );

    mergeSortConsumers(
    &graph,
    indices,
    0,
    count - 1
);

assert(indices[0] == 3);
assert(indices[1] == 4);
assert(indices[2] == 1);

assert(graph.nodes[1].id == 20);
assert(graph.nodes[3].id == 40);
assert(graph.nodes[4].id == 50);

printf("Merge Sort tests passed.\n");

displayConsumerOrder(
    &graph,
    indices,
    count
);

testQuickSort();

    return 0;
}