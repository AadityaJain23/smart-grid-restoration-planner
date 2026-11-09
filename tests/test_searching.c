#include <assert.h>
#include <stdio.h>

#include "searching.h"

int main(void) {

    Graph graph = {0};

    graph.nodeCount = 5;

    int ids[] = {40, 10, 70, 20, 50};

    for (int i = 0; i < graph.nodeCount; i++) {
        graph.nodes[i].id = ids[i];
    }

    int indices[MAX_NODES];

    int count = prepareSortedNodeIndices(
        &graph,
        indices
    );

    assert(count == 5);

    /* Verify sorted index order */
    assert(indices[0] == 1);
    assert(indices[1] == 3);
    assert(indices[2] == 0);
    assert(indices[3] == 4);
    assert(indices[4] == 2);

    /* Search existing IDs */
    assert(iterativeBinarySearchNode(
        &graph, indices, count, 10
    ) == 1);

    assert(iterativeBinarySearchNode(
        &graph, indices, count, 50
    ) == 4);

    assert(iterativeBinarySearchNode(
        &graph, indices, count, 70
    ) == 2);

    /* Search missing ID */
    assert(iterativeBinarySearchNode(
        &graph, indices, count, 99
    ) == -1);

    /* Verify original graph order is preserved */
    for (int i = 0; i < graph.nodeCount; i++) {
        assert(graph.nodes[i].id == ids[i]);
    }

    /* Empty graph */
    Graph empty = {0};

    assert(prepareSortedNodeIndices(
        &empty, indices
    ) == 0);

    assert(iterativeBinarySearchNode(
        &empty, indices, 0, 10
    ) == -1);

    printf("Iterative Binary Search tests passed.\n");

    /* Lecture 8: Recursive Binary Search tests */

assert(recursiveBinarySearchNode(
    &graph, indices, count, 10
) == 1);

assert(recursiveBinarySearchNode(
    &graph, indices, count, 50
) == 4);

assert(recursiveBinarySearchNode(
    &graph, indices, count, 70
) == 2);

/* Missing target */
assert(recursiveBinarySearchNode(
    &graph, indices, count, 99
) == -1);

/* Empty graph */
assert(recursiveBinarySearchNode(
    &empty, indices, 0, 10
) == -1);

/* Compare iterative and recursive results */
int targets[] = {10, 20, 40, 50, 70, 99};

for (int i = 0; i < 6; i++) {

    int iterativeResult =
        iterativeBinarySearchNode(
            &graph, indices, count, targets[i]
        );

    int recursiveResult =
        recursiveBinarySearchNode(
            &graph, indices, count, targets[i]
        );

    assert(iterativeResult == recursiveResult);
}

printf("Recursive Binary Search tests passed.\n");

    return 0;
}