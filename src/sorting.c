#include <stdio.h>
#include "sorting.h"

int extractConsumerIndices(
    const Graph *graph,
    int indices[]
) {
    if (graph == NULL || indices == NULL) {
        return -1;
    }

    int count = 0;

    for (int i = 0; i < graph->nodeCount; i++) {

        if (graph->nodes[i].type == CONSUMER) {

            indices[count] = i;
            count++;
        }
    }

    return count;
}

void displayConsumerOrder(
    const Graph *graph,
    const int indices[],
    int count
) {
    if (graph == NULL || indices == NULL ||
        count < 0 || count > graph->nodeCount) {
        return;
    }

    printf("\n%-5s %-25s %-12s\n",
           "ID", "Consumer", "Demand(MW)");

    printf("-------------------------------------------\n");

    for (int i = 0; i < count; i++) {

        int index = indices[i];

        if (index < 0 || index >= graph->nodeCount) {
            continue;
        }

        const Node *node = &graph->nodes[index];

        printf("%-5d %-25s %-12.2f\n",
               node->id,
               node->name,
               node->demand);
    }
}

static void merge(
    const Graph *graph,
    int indices[],
    int left,
    int mid,
    int right
) {
    int temp[MAX_NODES];

    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {

        double leftDemand =
            graph->nodes[indices[i]].demand;

        double rightDemand =
            graph->nodes[indices[j]].demand;

        if (leftDemand <= rightDemand) {
            temp[k] = indices[i];
            i++;
        }
        else {
            temp[k] = indices[j];
            j++;
        }

        k++;
    }

    while (i <= mid) {
        temp[k] = indices[i];
        i++;
        k++;
    }

    while (j <= right) {
        temp[k] = indices[j];
        j++;
        k++;
    }

    for (int p = left; p <= right; p++) {
        indices[p] = temp[p];
    }
}

int extractEdgeIndices(
    const Graph *graph,
    int indices[]
) {
    if (graph == NULL || indices == NULL) {
        return -1;
    }

    int count = 0;

    for (int i = 0; i < graph->edgeCount; i++) {
        indices[count] = i;
        count++;
    }

    return count;
}

static int partitionEdges(
    const Graph *graph,
    int indices[],
    int left,
    int right
) {
    int pivotIndex = indices[left];

    double pivotCost =
        graph->edges[pivotIndex].cost;

    int i = left;
    int j = right + 1;

    while (1) {

        do {
            i++;
        } while (
            i <= right &&
            graph->edges[indices[i]].cost < pivotCost
        );

        do {
            j--;
        } while (
            graph->edges[indices[j]].cost > pivotCost
        );

        if (i >= j) {
            break;
        }

        int temp = indices[i];
        indices[i] = indices[j];
        indices[j] = temp;
    }

    int temp = indices[left];
    indices[left] = indices[j];
    indices[j] = temp;

    return j;
}

void quickSortEdges(
    const Graph *graph,
    int indices[],
    int left,
    int right
) {
    if (graph == NULL || indices == NULL) {
        return;
    }

    if (graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES) {
        return;
    }

    if (left < 0 ||
        right >= graph->edgeCount) {
        return;
    }

    if (left >= right) {
        return;
    }

    int pivotPosition = partitionEdges(
        graph,
        indices,
        left,
        right
    );

    quickSortEdges(
        graph,
        indices,
        left,
        pivotPosition - 1
    );

    quickSortEdges(
        graph,
        indices,
        pivotPosition + 1,
        right
    );
}

void displaySortedEdges(
    const Graph *graph,
    const int indices[],
    int count
) {
    if (graph == NULL || indices == NULL ||
        count < 0 || count > graph->edgeCount) {
        return;
    }

    printf("\nTransmission Lines Sorted by Cost\n");

    printf("%-8s %-8s %-8s %-12s %-10s %-8s\n",
           "Edge", "From", "To",
           "Capacity", "Cost", "Active");

    printf("--------------------------------------------------------\n");

    for (int i = 0; i < count; i++) {

        int index = indices[i];

        if (index < 0 || index >= graph->edgeCount) {
            continue;
        }

        const Edge *edge = &graph->edges[index];

        printf("%-8d %-8d %-8d %-12.2f %-10.2f %-8d\n",
               index,
               edge->from,
               edge->to,
               edge->capacity,
               edge->cost,
               edge->active);
    }
}

void mergeSortConsumers(
    const Graph *graph,
    int indices[],
    int left,
    int right
) {
    if (graph == NULL || indices == NULL) {
        return;
    }

    if (left < 0 || right >= graph->nodeCount ||
        right >= MAX_NODES) {
        return;
    }

    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;

    mergeSortConsumers(
        graph, indices, left, mid
    );

    mergeSortConsumers(
        graph, indices, mid + 1, right
    );

    merge(
        graph, indices, left, mid, right
    );
}