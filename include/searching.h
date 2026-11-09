#ifndef SEARCHING_H
#define SEARCHING_H

#include "graph.h"

/* Prepare node indices sorted by Node ID */
int prepareSortedNodeIndices(
    const Graph *graph,
    int indices[]
);

/* Iterative Binary Search by Node ID */
int iterativeBinarySearchNode(
    const Graph *graph,
    const int sortedIndices[],
    int count,
    int targetID
);

/* Lecture 8: Recursive Binary Search */
int recursiveBinarySearchNode(
    const Graph *graph,
    const int sortedIndices[],
    int count,
    int targetID
);

#endif