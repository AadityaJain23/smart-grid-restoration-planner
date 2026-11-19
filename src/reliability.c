
#include <stdio.h>
#include "reliability.h"

/*
 * DFS workspace for Tarjan's bridge algorithm.
 *
 * Arrays use graph node-array indices,
 * not logical node IDs.
 */
typedef struct {
    int visited[MAX_NODES];
    int disc[MAX_NODES];
    int low[MAX_NODES];
    int timer;
} BridgeDFSState;

/*
 * Recursive DFS for bridge detection.
 *
 * parentEdge is the original graph edge index
 * used to enter the current node.
 *
 * -1 indicates the DFS root.
 */
static void bridgeDFS(
    const AdjList *list,
    int u,
    int parentEdge,
    BridgeDFSState *state,
    ReliabilityResult *result
) {
    state->visited[u] = 1;

    state->timer++;
    state->disc[u] = state->timer;
    state->low[u] = state->timer;

    int current = list->head[u];

    while (current != -1) {

        const AdjEntry *entry =
            &list->entries[current];

        int v = entry->neighborIndex;
        int edgeIndex = entry->edgeIndex;

        /*
         * Ignore only the exact parent edge.
         * This also handles parallel edges correctly
         * if the graph representation permits them.
         */
        if (edgeIndex == parentEdge) {
            current = entry->next;
            continue;
        }

        if (!state->visited[v]) {

            bridgeDFS(
                list,
                v,
                edgeIndex,
                state,
                result
            );

            /*
             * Update low[u] using the earliest
             * reachable discovery time from v.
             */
            if (state->low[v] < state->low[u]) {
                state->low[u] = state->low[v];
            }

            /*
             * Bridge condition:
             *
             * low[v] > disc[u]
             */
            if (state->low[v] > state->disc[u]) {

                result->bridgeEdgeIndices[
                    result->bridgeCount
                ] = edgeIndex;

                result->bridgeCount++;
            }

        } else {

            /*
             * Back edge to an already visited node.
             */
            if (state->disc[v] < state->low[u]) {
                state->low[u] = state->disc[v];
            }
        }

        current = entry->next;
    }
}

/*
 * Public function:
 * Find all bridges in the active smart-grid graph.
 *
 * Returns:
 * 1 = success
 * 0 = invalid graph, adjacency list, or result
 */
int findCriticalBridges(
    const Graph *graph,
    const AdjList *list,
    ReliabilityResult *result
) {
    if (graph == NULL ||
        list == NULL ||
        result == NULL) {
        return 0;
    }

    /*
     * Validate graph/list consistency before DFS.
     */
    if (!validateAdjacencyList(graph, list)) {
        return 0;
    }

    /*
     * Reset bridge results only.
     * Other result fields are preserved.
     */
    result->bridgeCount = 0;

    BridgeDFSState state = {0};

    /*
     * Run DFS from every unvisited node.
     * This supports disconnected networks.
     */
    for (int i = 0; i < graph->nodeCount; i++) {

        if (!state.visited[i]) {

            bridgeDFS(
                list,
                i,
                -1,
                &state,
                result
            );
        }
    }

    return 1;
}



/*
 * Week 6 - Tarjan's Articulation Point Detection
 */

typedef struct {
    int visited[MAX_NODES];
    int disc[MAX_NODES];
    int low[MAX_NODES];
    int isArticulation[MAX_NODES];
    int timer;
} ArticulationDFSState;


/*
 * Recursive DFS to identify articulation points.
 */
static void articulationDFS(
    const AdjList *list,
    int u,
    int parentEdge,
    ArticulationDFSState *state
) {
    state->visited[u] = 1;

    state->timer++;
    state->disc[u] = state->timer;
    state->low[u] = state->timer;

    int childCount = 0;
    int current = list->head[u];

    while (current != -1) {

        const AdjEntry *entry =
            &list->entries[current];

        int v = entry->neighborIndex;
        int edgeIndex = entry->edgeIndex;

        /*
         * Skip the exact parent edge.
         */
        if (edgeIndex == parentEdge) {
            current = entry->next;
            continue;
        }

        if (!state->visited[v]) {

            childCount++;

            articulationDFS(
                list,
                v,
                edgeIndex,
                state
            );

            if (state->low[v] < state->low[u]) {
                state->low[u] = state->low[v];
            }

            /*
             * Non-root articulation condition.
             */
            if (parentEdge != -1 &&
                state->low[v] >= state->disc[u]) {

                state->isArticulation[u] = 1;
            }

        } else {

            /*
             * Update low value using a back edge.
             */
            if (state->disc[v] < state->low[u]) {
                state->low[u] = state->disc[v];
            }
        }

        current = entry->next;
    }

    /*
     * Root articulation condition.
     */
    if (parentEdge == -1 && childCount > 1) {
        state->isArticulation[u] = 1;
    }
}


/*
 * Public function:
 * Find articulation points in the active network.
 */
int findArticulationPoints(
    const Graph *graph,
    const AdjList *list,
    ReliabilityResult *result
) {
    if (graph == NULL ||
        list == NULL ||
        result == NULL) {
        return 0;
    }

    /*
     * Validate the adjacency representation.
     */
    if (!validateAdjacencyList(graph, list)) {
        return 0;
    }

    /*
     * Reset articulation results only.
     * Preserve previously calculated bridges.
     */
    result->articulationCount = 0;

    ArticulationDFSState state = {0};

    /*
     * Visit every connected component.
     */
    for (int i = 0; i < graph->nodeCount; i++) {

        if (!state.visited[i]) {

            articulationDFS(
                list,
                i,
                -1,
                &state
            );
        }
    }

    /*
     * Collect articulation points.
     */
    for (int i = 0; i < graph->nodeCount; i++) {

        if (state.isArticulation[i]) {

            result->articulationNodeIndices[
                result->articulationCount
            ] = i;

            result->articulationCount++;
        }
    }

    return 1;
}
