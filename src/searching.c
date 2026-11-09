
#include <stddef.h>
#include "searching.h"

/*
 * Merge two sorted portions of the node-index array.
 * The comparison key is Node.id.
 */
static void mergeNodeIndices(
    const Graph *graph,
    int indices[],
    int temp[],
    int left,
    int mid,
    int right
) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {

        int leftID = graph->nodes[indices[i]].id;
        int rightID = graph->nodes[indices[j]].id;

        if (leftID <= rightID) {
            temp[k++] = indices[i++];
        } else {
            temp[k++] = indices[j++];
        }
    }

    /* Copy remaining elements from left half */
    while (i <= mid) {
        temp[k++] = indices[i++];
    }

    /* Copy remaining elements from right half */
    while (j <= right) {
        temp[k++] = indices[j++];
    }

    /* Copy sorted indices back */
    for (int p = left; p <= right; p++) {
        indices[p] = temp[p];
    }
}

/*
 * Recursive Merge Sort for node indices.
 * Original graph nodes are never rearranged.
 */
static void mergeSortNodeIndices(
    const Graph *graph,
    int indices[],
    int temp[],
    int left,
    int right
) {
    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;

    mergeSortNodeIndices(
        graph, indices, temp, left, mid
    );

    mergeSortNodeIndices(
        graph, indices, temp, mid + 1, right
    );

    mergeNodeIndices(
        graph, indices, temp, left, mid, right
    );
}

/*
 * Prepare an array of node indices sorted by Node.id.
 * Returns the number of nodes, or -1 on invalid input.
 *
 * The caller must provide space for MAX_NODES indices.
 */
int prepareSortedNodeIndices(
    const Graph *graph,
    int indices[]
) {
    if (graph == NULL || indices == NULL) {
        return -1;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES) {
        return -1;
    }

    int count = graph->nodeCount;

    /* Store original node array indices */
    for (int i = 0; i < count; i++) {
        indices[i] = i;
    }

    /* Sort indices according to node IDs */
    if (count > 1) {

        int temp[MAX_NODES];

        mergeSortNodeIndices(
            graph,
            indices,
            temp,
            0,
            count - 1
        );
    }

    return count;
}

/*
 * Iterative Binary Search by Node ID.
 *
 * Precondition:
 * sortedIndices must contain valid node indices
 * arranged in ascending order of Node.id.
 *
 * Returns original graph node index if found.
 * Returns -1 if absent or input is invalid.
 */
int iterativeBinarySearchNode(
    const Graph *graph,
    const int sortedIndices[],
    int count,
    int targetID
) {
    if (graph == NULL || sortedIndices == NULL) {
        return -1;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        count < 0 ||
        count > graph->nodeCount) {
        return -1;
    }

    int low = 0;
    int high = count - 1;

    while (low <= high) {

        int mid = low + (high - low) / 2;

        /* Retrieve original graph array index */
        int nodeIndex = sortedIndices[mid];

        if (nodeIndex < 0 ||
            nodeIndex >= graph->nodeCount) {
            return -1;
        }

        /* Compare actual node ID */
        int currentID = graph->nodes[nodeIndex].id;

        if (currentID == targetID) {
            return nodeIndex;
        }

        if (currentID < targetID) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

/*
 * Internal recursive Binary Search helper.
 *
 * Returns the original graph node index
 * or -1 if the target is absent.
 */
static int recursiveBinarySearchHelper(
    const Graph *graph,
    const int sortedIndices[],
    int low,
    int high,
    int targetID
) {
    /* Base case: search interval is empty */
    if (low > high) {
        return -1;
    }

    int mid = low + (high - low) / 2;

    int nodeIndex = sortedIndices[mid];

    /* Validate original node array index */
    if (nodeIndex < 0 ||
        nodeIndex >= graph->nodeCount) {
        return -1;
    }

    int currentID = graph->nodes[nodeIndex].id;

    /* Base case: target found */
    if (currentID == targetID) {
        return nodeIndex;
    }

    /* Recursive search in left half */
    if (targetID < currentID) {

        return recursiveBinarySearchHelper(
            graph,
            sortedIndices,
            low,
            mid - 1,
            targetID
        );
    }

    /* Recursive search in right half */
    return recursiveBinarySearchHelper(
        graph,
        sortedIndices,
        mid + 1,
        high,
        targetID
    );
}

/*
 * Public Recursive Binary Search function.
 *
 * Expects sortedIndices to be ordered
 * by ascending Node.id.
 */
int recursiveBinarySearchNode(
    const Graph *graph,
    const int sortedIndices[],
    int count,
    int targetID
) {
    if (graph == NULL || sortedIndices == NULL) {
        return -1;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        count < 0 ||
        count > graph->nodeCount) {
        return -1;
    }

    return recursiveBinarySearchHelper(
        graph,
        sortedIndices,
        0,
        count - 1,
        targetID
    );
}
