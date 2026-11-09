#ifndef SORTING_H
#define SORTING_H

#include "graph.h"

/* Lecture 1: Consumer extraction */
int extractConsumerIndices(
    const Graph *graph,
    int indices[]
);

/* Lecture 1: Display consumer records */
void displayConsumerOrder(
    const Graph *graph,
    const int indices[],
    int count
);

/* Lecture 3: Merge Sort by consumer demand */
void mergeSortConsumers(
    const Graph *graph,
    int indices[],
    int left,
    int right
);

/* Lecture 5: Transmission-line extraction */
int extractEdgeIndices(
    const Graph *graph,
    int indices[]
);

/* Lecture 5: Quick Sort by transmission-line cost */
void quickSortEdges(
    const Graph *graph,
    int indices[],
    int left,
    int right
);

/* Lecture 5: Display sorted transmission lines */
void displaySortedEdges(
    const Graph *graph,
    const int indices[],
    int count
);

#endif