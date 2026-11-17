#include <stddef.h>
#include "dsu.h"

/* Initialize every element as its own component. */
int dsuInitialize(DSU *dsu, int count) {
    if (dsu == NULL ||
        count <= 0 ||
        count > MAX_NODES) {
        return 0;
    }

    dsu->size = count;

    for (int i = 0; i < count; i++) {
        dsu->parent[i] = i;
        dsu->rank[i] = 0;
    }

    return 1;
}

/* Find representative using path compression. */
int dsuFind(DSU *dsu, int element) {
    if (dsu == NULL ||
        dsu->size <= 0 ||
        dsu->size > MAX_NODES ||
        element < 0 ||
        element >= dsu->size) {
        return -1;
    }

    if (dsu->parent[element] != element) {
        int root = dsuFind(
            dsu, dsu->parent[element]
        );

        if (root == -1) {
            return -1;
        }

        dsu->parent[element] = root;
    }

    return dsu->parent[element];
}

/* Merge two components using union by rank. */
int dsuUnion(DSU *dsu, int a, int b) {
    int rootA = dsuFind(dsu, a);
    int rootB = dsuFind(dsu, b);

    if (rootA == -1 || rootB == -1) {
        return -1;
    }

    if (rootA == rootB) {
        return 0;
    }

    if (dsu->rank[rootA] < dsu->rank[rootB]) {
        dsu->parent[rootA] = rootB;
    }
    else if (dsu->rank[rootA] > dsu->rank[rootB]) {
        dsu->parent[rootB] = rootA;
    }
    else {
        dsu->parent[rootB] = rootA;
        dsu->rank[rootA]++;
    }

    return 1;
}

/* Determine whether two elements share a component. */
int dsuConnected(DSU *dsu, int a, int b) {
    int rootA = dsuFind(dsu, a);
    int rootB = dsuFind(dsu, b);

    if (rootA == -1 || rootB == -1) {
        return -1;
    }

    return rootA == rootB;
}
