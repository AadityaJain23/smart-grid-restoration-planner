
#ifndef DSU_H
#define DSU_H

#include "graph.h"

/* Disjoint Set Union data structure. */
typedef struct {
    int parent[MAX_NODES];
    int rank[MAX_NODES];
    int size;
} DSU;

/* Initialize a DSU containing count elements. */
int dsuInitialize(DSU *dsu, int count);

/* Find the representative with path compression. */
int dsuFind(DSU *dsu, int element);

/*
 * Merge the components containing a and b.
 *
 * Returns:
 *  1 = components merged
 *  0 = already in the same component
 * -1 = invalid input
 */
int dsuUnion(DSU *dsu, int a, int b);

/*
 * Check whether two elements are connected.
 *
 * Returns:
 *  1 = connected
 *  0 = not connected
 * -1 = invalid input
 */
int dsuConnected(DSU *dsu, int a, int b);

#endif
