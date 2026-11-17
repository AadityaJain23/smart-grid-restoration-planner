
#include "maxflow.h"

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <float.h>

/* Find the array index of a logical node ID. */
static int findFlowNodeIndex(
    const Graph *graph,
    int nodeId
)
{
    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i].id == nodeId) {
            return i;
        }
    }

    return -1;
}

/* Validate the graph before building a flow network. */
static int validateFlowGraph(const Graph *graph)
{
    if (graph == NULL) {
        return 0;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES) {
        return 0;
    }

    for (int i = 0; i < graph->nodeCount; i++) {
        const Node *node = &graph->nodes[i];

        if (node->type < 0 || node->type > 2 ||
            !isfinite(node->demand) ||
            !isfinite(node->supply) ||
            node->demand < 0.0 ||
            node->supply < 0.0) {
            return 0;
        }

        for (int j = 0; j < i; j++) {
            if (graph->nodes[j].id == node->id) {
                return 0;
            }
        }
    }

    for (int i = 0; i < graph->edgeCount; i++) {
        const Edge *edge = &graph->edges[i];

        if ((edge->active != 0 && edge->active != 1) ||
            !isfinite(edge->capacity) ||
            edge->capacity < 0.0) {
            return 0;
        }

        int u = findFlowNodeIndex(graph, edge->from);
        int v = findFlowNodeIndex(graph, edge->to);

        if (u < 0 || v < 0 || u == v) {
            return 0;
        }
    }

    return 1;
}


int buildFlowNetwork(
    const Graph *graph,
    FlowNetwork *network
)
{
    if (network == NULL || !validateFlowGraph(graph)) {
        return 0;
    }

    FlowNetwork candidate = {0};

    int n = graph->nodeCount;

    candidate.vertexCount = n + 2;
    candidate.sourceIndex = n;
    candidate.sinkIndex = n + 1;

    /* Add active undirected transmission lines. */
    for (int i = 0; i < graph->edgeCount; i++) {

        const Edge *edge = &graph->edges[i];

        if (!edge->active) {
            continue;
        }

        int u = findFlowNodeIndex(graph, edge->from);
        int v = findFlowNodeIndex(graph, edge->to);

        double capacity = edge->capacity;

        /* Accumulate parallel-line capacities safely. */
        if (candidate.residual[u][v] >
            DBL_MAX - capacity) {
            return 0;
        }

        candidate.residual[u][v] += capacity;
        candidate.residual[v][u] =
            candidate.residual[u][v];
    }

    /* Add artificial source and sink connections. */
    for (int i = 0; i < n; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type == 0) {
            candidate.residual[candidate.sourceIndex][i] =
                node->supply;
        }

        if (node->type == 2) {
            candidate.residual[i][candidate.sinkIndex] =
                node->demand;
        }
    }

    *network = candidate;

    return 1;
}


static int bfsFlowPath(
    const FlowNetwork *network,
    int parent[MAX_FLOW_VERTICES]
)
{
    int visited[MAX_FLOW_VERTICES] = {0};
    int queue[MAX_FLOW_VERTICES];

    int front = 0;
    int rear = 0;

    int source = network->sourceIndex;
    int sink = network->sinkIndex;

    for (int i = 0; i < network->vertexCount; i++) {
        parent[i] = -1;
    }

    visited[source] = 1;
    queue[rear++] = source;

    while (front < rear) {

        int u = queue[front++];

        for (int v = 0; v < network->vertexCount; v++) {

            if (!visited[v] &&
                network->residual[u][v] > FLOW_EPS) {

                visited[v] = 1;
                parent[v] = u;

                if (v == sink) {
                    return 1;
                }

                queue[rear++] = v;
            }
        }
    }

    return 0;
}


int edmondsKarp(
    FlowNetwork *network,
    double *maximumFlow,
    int *augmentingPaths
)
{
    if (network == NULL ||
        maximumFlow == NULL ||
        augmentingPaths == NULL) {
        return 0;
    }

    int n = network->vertexCount;
    int source = network->sourceIndex;
    int sink = network->sinkIndex;

    if (n < 2 || n > MAX_FLOW_VERTICES ||
        source < 0 || source >= n ||
        sink < 0 || sink >= n ||
        source == sink) {
        return 0;
    }

    /* Validate the residual network. */
    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            double value = network->residual[u][v];

            if (!isfinite(value) || value < 0.0) {
                return 0;
            }
        }
    }

    /*
     * Work on a candidate so that errors
     * do not partially modify the input.
     */
    FlowNetwork candidate = *network;

    double totalFlow = 0.0;
    int pathCount = 0;

    int parent[MAX_FLOW_VERTICES];

    while (bfsFlowPath(&candidate, parent)) {

        double bottleneck = DBL_MAX;

        /* Find minimum residual capacity on path. */
        for (int v = sink; v != source; v = parent[v]) {

            int u = parent[v];

            if (u < 0 || u >= n) {
                return 0;
            }

            if (candidate.residual[u][v] < bottleneck) {
                bottleneck = candidate.residual[u][v];
            }
        }

        if (!isfinite(bottleneck) ||
            bottleneck <= FLOW_EPS ||
            bottleneck > DBL_MAX - totalFlow) {
            return 0;
        }

        /*
         * Check reverse residual updates
         * before modifying the candidate.
         */
        for (int v = sink; v != source; v = parent[v]) {
            int u = parent[v];

            if (candidate.residual[v][u] >
                DBL_MAX - bottleneck) {
                return 0;
            }
        }

        /* Update forward and reverse capacities. */
        for (int v = sink; v != source; v = parent[v]) {

            int u = parent[v];

            candidate.residual[u][v] -= bottleneck;
            candidate.residual[v][u] += bottleneck;

            if (candidate.residual[u][v] < 0.0 &&
                candidate.residual[u][v] > -FLOW_EPS) {
                candidate.residual[u][v] = 0.0;
            }
        }

        totalFlow += bottleneck;
        pathCount++;
    }

    *network = candidate;
    *maximumFlow = totalFlow;
    *augmentingPaths = pathCount;

    return 1;
}


int calculateMaximumFlow(
    const Graph *graph,
    MaxFlowResult *result
)
{
    if (result == NULL || !validateFlowGraph(graph)) {
        return 0;
    }

    FlowNetwork network;
    MaxFlowResult candidate = {0};

    if (!buildFlowNetwork(graph, &network)) {
        return 0;
    }

    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type == 0) {
            if (candidate.totalSupply >
                DBL_MAX - node->supply) {
                return 0;
            }

            candidate.totalSupply += node->supply;
        }

        if (node->type == 2) {
            if (candidate.totalDemand >
                DBL_MAX - node->demand) {
                return 0;
            }

            candidate.totalDemand += node->demand;
        }
    }

    if (!edmondsKarp(
            &network,
            &candidate.maxFlow,
            &candidate.augmentingPaths)) {
        return 0;
    }

    int sink = network.sinkIndex;

    /*
     * Consumer-to-sink edges started with
     * residual capacity equal to demand.
     */
    for (int i = 0; i < graph->nodeCount; i++) {

        if (graph->nodes[i].type == 2) {

            double demand = graph->nodes[i].demand;

            double remaining =
                network.residual[i][sink];

            double delivered = demand - remaining;

            if (delivered < 0.0 &&
                delivered > -FLOW_EPS) {
                delivered = 0.0;
            }

            if (!isfinite(delivered) ||
                delivered < 0.0 ||
                delivered > demand + FLOW_EPS) {
                return 0;
            }

            candidate.deliveredToNode[i] = delivered;
        }
    }

    if (candidate.maxFlow >
            candidate.totalSupply + FLOW_EPS ||
        candidate.maxFlow >
            candidate.totalDemand + FLOW_EPS) {
        return 0;
    }

    candidate.totalUnserved =
        candidate.totalDemand - candidate.maxFlow;

    if (candidate.totalUnserved < 0.0 &&
        candidate.totalUnserved > -FLOW_EPS) {
        candidate.totalUnserved = 0.0;
    }

    if (candidate.totalUnserved < 0.0) {
        return 0;
    }

    *result = candidate;

    return 1;
}


void displayMaximumFlowReport(
    const Graph *graph,
    const MaxFlowResult *result
)
{
    if (graph == NULL || result == NULL) {
        printf("Invalid Maximum Flow report.\n");
        return;
    }

    printf("\n========================================\n");
    printf("       SMART GRID MAXIMUM FLOW\n");
    printf("========================================\n");

    printf("Total Supply       : %.2f MW\n",
           result->totalSupply);

    printf("Total Demand       : %.2f MW\n",
           result->totalDemand);

    printf("Maximum Flow       : %.2f MW\n",
           result->maxFlow);

    printf("Unserved Demand    : %.2f MW\n",
           result->totalUnserved);

    printf("Augmenting Paths   : %d\n",
           result->augmentingPaths);

    printf("\nConsumer Allocations:\n");

    printf("%-8s %-18s %-12s %-12s\n",
           "Node ID", "Name", "Demand", "Delivered");

    printf("------------------------------------------------------\n");

    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type != 2) {
            continue;
        }

        printf("%-8d %-18.18s %-12.2f %-12.2f\n",
               node->id,
               node->name,
               node->demand,
               result->deliveredToNode[i]);
    }

    printf("\nNote: Maximum Flow optimizes total\n");
    printf("throughput, not consumer priority.\n");
}
