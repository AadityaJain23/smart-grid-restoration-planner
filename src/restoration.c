
#include <stdio.h>
#include <math.h>
#include <float.h>
#include "restoration.h"

#define REST_EPS 1e-9





/* Return the smaller of two values. */
static double minDouble(double a, double b) {
    return a < b ? a : b;
}

/* Sort consumers by priority, then node ID. */
static int prepareConsumerPriorityOrder(
    const Graph *graph,
    int order[MAX_NODES]
) {
    int count = 0;

    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i].type == CONSUMER) {
            order[count++] = i;
        }
    }

    /* Stable insertion sort of auxiliary indices. */
    for (int i = 1; i < count; i++) {
        int selected = order[i];
        int j = i - 1;

        while (j >= 0) {
            const Node *a = &graph->nodes[order[j]];
            const Node *b = &graph->nodes[selected];

            if (a->priority < b->priority ||
                (a->priority == b->priority &&
                 a->id <= b->id)) {
                break;
            }

            order[j + 1] = order[j];
            j--;
        }

        order[j + 1] = selected;
    }

    return count;
}

/*
 * Capacity-filtered Dijkstra.
 * Returns 1 when a path exists, 0 otherwise.
 *
 * parentEdge[v] records the original edge index
 * used to reach vertex v.
 */
static int findCapacityAwarePath(
    const Graph *graph,
    const AdjList *list,
    const RestorationResult *state,
    int source,
    int target,
    int parent[MAX_NODES],
    int parentEdge[MAX_NODES],
    double *pathCost
) {
    double distance[MAX_NODES];
    int visited[MAX_NODES] = {0};

    for (int i = 0; i < graph->nodeCount; i++) {
        distance[i] = INFINITY;
        parent[i] = -1;
        parentEdge[i] = -1;
    }

    distance[source] = 0.0;

    for (int count = 0; count < graph->nodeCount; count++) {
        int u = -1;
        double best = INFINITY;

        /* Select minimum-distance unvisited node. */
        for (int i = 0; i < graph->nodeCount; i++) {
            if (!visited[i] && distance[i] < best) {
                best = distance[i];
                u = i;
            }
        }

        if (u == -1) {
            break;
        }

        if (u == target) {
            break;
        }

        visited[u] = 1;

        int current = list->head[u];

        while (current != -1) {
            AdjEntry entry = list->entries[current];

            int v = entry.neighborIndex;
            int e = entry.edgeIndex;

            const Edge *edge = &graph->edges[e];

            if (!visited[v] &&
                edge->active == 1 &&
                state->residualCapacity[e] > REST_EPS) {

                double weight = edge->cost;

                if (distance[u] <= DBL_MAX - weight) {
                    double candidate =
                        distance[u] + weight;

                    if (candidate < distance[v]) {
                        distance[v] = candidate;
                        parent[v] = u;
                        parentEdge[v] = e;
                    }
                }
            }

            current = entry.next;
        }
    }

    if (!isfinite(distance[target])) {
        return 0;
    }

    *pathCost = distance[target];
    return 1;
}

/*
 * Reconstruct a path and calculate its
 * minimum residual transmission capacity.
 *
 * Returns 1 on success, 0 on invalid path.
 */
static int reconstructPath(
    const Graph *graph,
    const RestorationResult *state,
    int source,
    int target,
    const int parent[MAX_NODES],
    const int parentEdge[MAX_NODES],
    int pathEdges[MAX_NODES - 1],
    int *pathLength,
    double *bottleneck
) {
    int current = target;
    int length = 0;
    double minimum = INFINITY;

    while (current != source) {
        if (current < 0 ||
            current >= graph->nodeCount ||
            length >= graph->nodeCount - 1) {
            return 0;
        }

        int e = parentEdge[current];
        int p = parent[current];

        if (e < 0 || e >= graph->edgeCount ||
            p < 0 || p >= graph->nodeCount) {
            return 0;
        }

        pathEdges[length++] = e;

        minimum = minDouble(
            minimum, state->residualCapacity[e]
        );

        current = p;
    }

    if (length == 0 || !isfinite(minimum)) {
        return 0;
    }

    *pathLength = length;
    *bottleneck = minimum;

    return 1;
}

/* Initialize all remaining demands and capacities. */
static int initializeRestorationResult(
    const Graph *graph,
    RestorationResult *state
) {
    *state = (RestorationResult){0};

    for (int i = 0; i < graph->nodeCount; i++) {
        const Node *node = &graph->nodes[i];

        if (!validateNodeData(node)) {
            return 0;
        }

        if (node->type == GENERATOR) {
            state->remainingSupply[i] = node->supply;
        }

        if (node->type == CONSUMER) {
            state->remainingDemand[i] = node->demand;

            if (state->totalDemand >
                DBL_MAX - node->demand) {
                return 0;
            }

            state->totalDemand += node->demand;
        }
    }

    for (int e = 0; e < graph->edgeCount; e++) {
        const Edge *edge = &graph->edges[e];

        if (!validateEdgeData(edge)) {
            return 0;
        }

        state->residualCapacity[e] =
            edge->active ? edge->capacity : 0.0;
    }

    return 1;
}

/*
 * Allocate power without exceeding generator,
 * consumer or transmission-line limits.
 */
static int allocateAlongPath(
    RestorationResult *state,
    int generator,
    int consumer,
    const int pathEdges[MAX_NODES - 1],
    int pathLength,
    double amount
) {
    if (!isfinite(amount) || amount <= REST_EPS ||
        amount > state->remainingSupply[generator] ||
        amount > state->remainingDemand[consumer]) {
        return 0;
    }

    for (int i = 0; i < pathLength; i++) {
        int e = pathEdges[i];

        if (amount > state->residualCapacity[e]) {
            return 0;
        }
    }

    if (state->totalAllocated > DBL_MAX - amount) {
        return 0;
    }

    /* Commit only after all checks pass. */
    state->remainingSupply[generator] -= amount;
    state->remainingDemand[consumer] -= amount;
    state->allocated[consumer] += amount;
    state->totalAllocated += amount;

    for (int i = 0; i < pathLength; i++) {
        int e = pathEdges[i];

        state->residualCapacity[e] -= amount;
        state->edgeAllocated[e] += amount;
    }

    return 1;
}

/* Main priority-based restoration heuristic. */
int planPowerRestoration(
    const Graph *graph,
    const AdjList *list,
    RestorationResult *result
) {
    if (graph == NULL || list == NULL ||
        result == NULL) {
        return 0;
    }

    if (graph->nodeCount <= 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES ||
        !validateAdjacencyList(graph, list)) {
        return 0;
    }

    RestorationResult state;

    if (!initializeRestorationResult(graph, &state)) {
        return 0;
    }

    int order[MAX_NODES];
    int consumerCount =
        prepareConsumerPriorityOrder(graph, order);

    for (int c = 0; c < consumerCount; c++) {
        int consumer = order[c];

        /*
         * Each successful partial allocation must
         * exhaust a generator, saturate a line,
         * or finish this consumer (within EPS).
         */
        int maxAttempts =
            graph->edgeCount + graph->nodeCount + 1;

        for (int attempt = 0;
             attempt < maxAttempts &&
             state.remainingDemand[consumer] > REST_EPS;
             attempt++) {

            int bestGenerator = -1;
            double bestCost = INFINITY;
            double bestAmount = 0.0;

            int bestPath[MAX_NODES - 1];
            int bestLength = 0;

            /* Examine every generator with supply. */
            for (int g = 0; g < graph->nodeCount; g++) {
                if (graph->nodes[g].type != GENERATOR ||
                    state.remainingSupply[g] <= REST_EPS) {
                    continue;
                }

                int parent[MAX_NODES];
                int parentEdge[MAX_NODES];
                int path[MAX_NODES - 1];

                int pathLength = 0;
                double pathCost = INFINITY;
                double bottleneck = 0.0;

                if (!findCapacityAwarePath(
                        graph, list, &state,
                        g, consumer,
                        parent, parentEdge,
                        &pathCost)) {
                    continue;
                }

                if (!reconstructPath(
                        graph, &state, g, consumer,
                        parent, parentEdge,
                        path, &pathLength,
                        &bottleneck)) {
                    return 0;
                }

                double amount = minDouble(
                    state.remainingDemand[consumer],
                    state.remainingSupply[g]
                );

                amount = minDouble(amount, bottleneck);

                if (amount <= REST_EPS) {
                    continue;
                }

                /* Choose the cheapest eligible route. */
                if (bestGenerator == -1 ||
                    pathCost < bestCost) {

                    bestGenerator = g;
                    bestCost = pathCost;
                    bestAmount = amount;
                    bestLength = pathLength;

                    for (int k = 0; k < pathLength; k++) {
                        bestPath[k] = path[k];
                    }
                }
            }

            if (bestGenerator == -1) {
                break;
            }

            if (!allocateAlongPath(
                    &state, bestGenerator, consumer,
                    bestPath, bestLength,
                    bestAmount)) {
                return 0;
            }
        }

        /*
         * If demand remains and another positive
         * route is possible, the attempt bound
         * would indicate an algorithmic problem.
         * The bound is conservative for this model.
         */
    }

    /* Calculate final consumer service counts. */
    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i].type != CONSUMER) {
            continue;
        }

        double allocated = state.allocated[i];
        double demand = graph->nodes[i].demand;

        if (allocated >= demand - REST_EPS) {
            state.fullyServedConsumers++;
        }
        else if (allocated > REST_EPS) {
            state.partiallyServedConsumers++;
        }
        else {
            state.unservedConsumers++;
        }
    }

    state.totalUnserved =
        state.totalDemand - state.totalAllocated;

    if (!isfinite(state.totalUnserved) ||
        state.totalUnserved < -REST_EPS) {
        return 0;
    }

    if (state.totalUnserved < 0.0) {
        state.totalUnserved = 0.0;
    }

    *result = state;

    return 1;
}

/* Display the restoration allocation report. */
void displayRestorationReport(
    const Graph *graph,
    const RestorationResult *result
) {
    if (graph == NULL || result == NULL ||
        graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES) {
        printf("Error: Invalid restoration report.\n");
        return;
    }

    printf("\n========== RESTORATION REPORT ==========\n");

    printf("%-22s %-8s %-10s %-10s\n",
           "Consumer", "Priority", "Demand", "Allocated");

    printf("----------------------------------------------------\n");

    for (int i = 0; i < graph->nodeCount; i++) {
        const Node *node = &graph->nodes[i];

        if (node->type != CONSUMER) {
            continue;
        }

        printf("%-22s %-8d %-10.2f %-10.2f\n",
               node->name,
               node->priority,
               node->demand,
               result->allocated[i]);
    }

    printf("----------------------------------------------------\n");

    printf("Total Demand: %.2f MW\n",
           result->totalDemand);

    printf("Total Allocated: %.2f MW\n",
           result->totalAllocated);

    printf("Total Unserved: %.2f MW\n",
           result->totalUnserved);

    printf("Fully Served Consumers: %d\n",
           result->fullyServedConsumers);

    printf("Partially Served Consumers: %d\n",
           result->partiallyServedConsumers);

    printf("Unserved Consumers: %d\n",
           result->unservedConsumers);

    printf("\nNote: Simplified capacity-aware allocation.\n");
    printf("This is not an AC power-flow calculation.\n");
}
