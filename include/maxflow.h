
#ifndef MAXFLOW_H
#define MAXFLOW_H

#include "graph.h"

/*
 * Week 4 - Maximum Flow
 *
 * Edmonds-Karp algorithm using BFS.
 *
 * Supports:
 * - Multiple generators
 * - Multiple consumers
 * - Active transmission lines
 * - Undirected shared-capacity lines
 * - Super-source and super-sink
 */

#define MAX_FLOW_VERTICES (MAX_NODES + 2)
#define FLOW_EPS 1e-9

/*
 * Internal residual-network representation.
 *
 * Original graph vertices use array indices
 * 0 to graph->nodeCount - 1.
 *
 * Two additional indices are reserved for
 * the super-source and super-sink.
 */
typedef struct {
    double residual[MAX_FLOW_VERTICES]
                   [MAX_FLOW_VERTICES];

    int vertexCount;
    int sourceIndex;
    int sinkIndex;
} FlowNetwork;

/*
 * Maximum Flow computation result.
 *
 * deliveredToNode[i] corresponds to
 * graph->nodes[i], not logical node ID i.
 */
typedef struct {
    double maxFlow;

    double totalSupply;
    double totalDemand;
    double totalUnserved;

    double deliveredToNode[MAX_NODES];

    int augmentingPaths;
} MaxFlowResult;

/*
 * Builds a residual network from the graph.
 *
 * Returns:
 * 1 = success
 * 0 = invalid input or failure
 *
 * Does not modify the original graph.
 */
int buildFlowNetwork(
    const Graph *graph,
    FlowNetwork *network
);

/*
 * Runs Edmonds-Karp on a constructed
 * residual network.
 *
 * The network is modified during execution.
 *
 * Returns:
 * 1 = success
 * 0 = invalid input or failure
 */
int edmondsKarp(
    FlowNetwork *network,
    double *maximumFlow,
    int *augmentingPaths
);

/*
 * Complete Smart Grid Maximum Flow planner.
 *
 * Builds the network, runs Edmonds-Karp,
 * and records consumer allocations.
 */
int calculateMaximumFlow(
    const Graph *graph,
    MaxFlowResult *result
);

/*
 * Displays Maximum Flow statistics
 * and consumer allocations.
 */
void displayMaximumFlowReport(
    const Graph *graph,
    const MaxFlowResult *result
);

#endif
