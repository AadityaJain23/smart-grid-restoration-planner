
#include "priority_flow.h"
#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <float.h>


/*
 * Forward declarations for functions
 * defined later in this file.
 */
static int validatePriorityGraph(
    const Graph *graph
);


/*
 * Forward declaration.
 *
 * Implemented later in priority_flow.c.
 */
static int augmentPriorityPath(
    PriorityNetwork *network,
    const int parentArc[MAX_FLOW_VERTICES],
    double *augmentedFlow,
    PriorityCost *costChange
);


/*
 * Forward declaration for the complete
 * flow-conservation validator.
 *
 * Actual implementation appears later.
 */
static int validateFlowConservation(
    const Graph *graph,
    const PriorityNetwork *network
);


/*
 * Construct a zero lexicographic cost.
 */
static PriorityCost zeroPriorityCost(void)
{
    PriorityCost result = {{0.0, 0.0, 0.0}};
    return result;
}

/*
 * Add two priority cost vectors.
 */
static PriorityCost addPriorityCost(
    PriorityCost a,
    PriorityCost b
)
{
    PriorityCost result = zeroPriorityCost();

    for (int i = 0; i < PRIORITY_LEVELS; i++) {
        result.component[i] =
            a.component[i] + b.component[i];
    }

    return result;
}

/*
 * Negate a priority cost vector.
 *
 * Used for reverse residual arcs.
 */
static PriorityCost negatePriorityCost(
    PriorityCost cost
)
{
    PriorityCost result = zeroPriorityCost();

    for (int i = 0; i < PRIORITY_LEVELS; i++) {
        result.component[i] =
            -cost.component[i];
    }

    return result;
}

/*
 * Compare two lexicographic costs.
 *
 * Returns:
 * -1 if a is smaller
 *  0 if equal
 *  1 if a is greater
 */
static int comparePriorityCost(
    PriorityCost a,
    PriorityCost b
)
{
    for (int i = 0; i < PRIORITY_LEVELS; i++) {

        if (a.component[i] < b.component[i]) {
            return -1;
        }

        if (a.component[i] > b.component[i]) {
            return 1;
        }
    }

    return 0;
}

/*
 * Construct the cost of delivering one MW
 * to a consumer of the specified priority.
 */
static PriorityCost makeConsumerCost(
    int priority
)
{
    PriorityCost result = zeroPriorityCost();

    if (priority >= 1 &&
        priority <= PRIORITY_LEVELS) {

        result.component[priority - 1] = -1.0;
    }

    return result;
}


/*
 * Add a paired residual connection.
 *
 * forwardCapacity and reverseCapacity
 * specify initial residual capacities.
 *
 * The two arcs are reverse partners.
 */
static int addPriorityArcPair(
    PriorityNetwork *network,
    int from,
    int to,
    double forwardCapacity,
    double reverseCapacity,
    PriorityCost forwardCost
)
{
    if (network == NULL) {
        return 0;
    }

    if (network->vertexCount < 2 ||
        network->vertexCount > MAX_FLOW_VERTICES ||
        from < 0 ||
        to < 0 ||
        from >= network->vertexCount ||
        to >= network->vertexCount ||
        from == to) {
        return 0;
    }

    if (!isfinite(forwardCapacity) ||
        !isfinite(reverseCapacity) ||
        forwardCapacity < 0.0 ||
        reverseCapacity < 0.0) {
        return 0;
    }

    for (int i = 0; i < PRIORITY_LEVELS; i++) {
        if (!isfinite(forwardCost.component[i])) {
            return 0;
        }
    }

    if (network->arcCount < 0 ||
        network->arcCount > MAX_PRIORITY_ARCS - 2) {
        return 0;
    }

    int forward = network->arcCount;
    int reverse = forward + 1;

    PriorityArc *a = &network->arcs[forward];
    PriorityArc *b = &network->arcs[reverse];

    a->from = from;
    a->to = to;
    a->reverseIndex = reverse;
    a->capacity = forwardCapacity;
    a->cost = forwardCost;

    b->from = to;
    b->to = from;
    b->reverseIndex = forward;
    b->capacity = reverseCapacity;
    b->cost = negatePriorityCost(forwardCost);

    network->arcCount += 2;

    return 1;
}


/*
 * Validate the complete residual network.
 *
 * Checks:
 * - Vertex and arc bounds
 * - Valid source and sink
 * - Finite nonnegative capacities
 * - Valid reverse-arc relationships
 * - Opposite reverse costs
 */
static int validatePriorityNetwork(
    const PriorityNetwork *network
)
{
    if (network == NULL) {
        return 0;
    }

    int n = network->vertexCount;
    int m = network->arcCount;

    if (n < 2 || n > MAX_FLOW_VERTICES ||
        m < 0 || m > MAX_PRIORITY_ARCS ||
        network->sourceIndex < 0 ||
        network->sourceIndex >= n ||
        network->sinkIndex < 0 ||
        network->sinkIndex >= n ||
        network->sourceIndex ==
            network->sinkIndex) {
        return 0;
    }

    for (int e = 0; e < m; e++) {

        const PriorityArc *arc =
            &network->arcs[e];

        if (arc->from < 0 || arc->from >= n ||
            arc->to < 0 || arc->to >= n ||
            arc->from == arc->to ||
            arc->reverseIndex < 0 ||
            arc->reverseIndex >= m ||
            arc->reverseIndex == e ||
            !isfinite(arc->capacity) ||
            arc->capacity < 0.0) {
            return 0;
        }

        const PriorityArc *reverse =
            &network->arcs[arc->reverseIndex];

        if (reverse->reverseIndex != e ||
            reverse->from != arc->to ||
            reverse->to != arc->from) {
            return 0;
        }

        for (int k = 0; k < PRIORITY_LEVELS; k++) {

            double cost = arc->cost.component[k];
            double reverseCost =
                reverse->cost.component[k];

            if (!isfinite(cost) ||
                !isfinite(reverseCost) ||
                cost != -reverseCost) {
                return 0;
            }
        }
    }

    return 1;
}


/*
 * Find any source-to-sink augmenting path.
 *
 * Returns:
 *  1 = path found
 *  0 = no path exists
 * -1 = invalid network
 */
static int bfsPriorityAugmentingPath(
    const PriorityNetwork *network,
    int parentArc[MAX_FLOW_VERTICES]
)
{
    if (network == NULL ||
        parentArc == NULL ||
        !validatePriorityNetwork(network)) {
        return -1;
    }

    int n = network->vertexCount;
    int source = network->sourceIndex;
    int sink = network->sinkIndex;

    int visited[MAX_FLOW_VERTICES] = {0};
    int queue[MAX_FLOW_VERTICES];

    int front = 0;
    int rear = 0;

    for (int i = 0; i < n; i++) {
        parentArc[i] = -1;
    }

    visited[source] = 1;
    queue[rear++] = source;

    while (front < rear) {

        int u = queue[front++];

        for (int e = 0; e < network->arcCount; e++) {

            const PriorityArc *arc =
                &network->arcs[e];

            if (arc->from != u ||
                arc->capacity <= FLOW_EPS) {
                continue;
            }

            int v = arc->to;

            if (visited[v]) {
                continue;
            }

            visited[v] = 1;
            parentArc[v] = e;

            if (v == sink) {
                return 1;
            }

            queue[rear++] = v;
        }
    }

    return 0;
}

/*
 * Calculate maximum source-to-sink flow
 * using BFS and residual augmentation.
 *
 * Returns:
 * 1 = success
 * 0 = invalid input or calculation failure
 *
 * The original network remains unchanged
 * if the calculation fails.
 */
static int runPriorityMaximumFlow(
    PriorityNetwork *network,
    double *maximumFlow,
    int *augmentingPaths
)
{
    if (network == NULL ||
        maximumFlow == NULL ||
        augmentingPaths == NULL ||
        !validatePriorityNetwork(network)) {
        return 0;
    }

    PriorityNetwork candidate = *network;

    double totalFlow = 0.0;
    int paths = 0;

    int parentArc[MAX_FLOW_VERTICES];

    while (1) {

        int status = bfsPriorityAugmentingPath(
            &candidate,
            parentArc
        );

        if (status < 0) {
            return 0;
        }

        if (status == 0) {
            break;
        }

        double deltaFlow = 0.0;

        PriorityCost deltaCost =
            zeroPriorityCost();

        if (!augmentPriorityPath(
                &candidate,
                parentArc,
                &deltaFlow,
                &deltaCost)) {
            return 0;
        }

        if (!isfinite(deltaFlow) ||
            deltaFlow <= FLOW_EPS ||
            totalFlow > DBL_MAX - deltaFlow ||
            paths == INT_MAX) {
            return 0;
        }

        totalFlow += deltaFlow;
        paths++;
    }

    /*
     * Commit only after successful completion.
     */
    *network = candidate;
    *maximumFlow = totalFlow;
    *augmentingPaths = paths;

    return 1;
}


/*
 * Push flow through one residual arc.
 *
 * Updates both the selected arc and its
 * reverse partner.
 */
static int pushPriorityFlow(
    PriorityNetwork *network,
    int arcIndex,
    double amount
)
{
    
    if (network == NULL ||
        !validatePriorityNetwork(network) ||
        arcIndex < 0 ||
        arcIndex >= network->arcCount ||
        !isfinite(amount) ||
        amount <= FLOW_EPS) {
        return 0;
    }


    PriorityArc *forward =
        &network->arcs[arcIndex];

    int reverseIndex = forward->reverseIndex;

    if (reverseIndex < 0 ||
        reverseIndex >= network->arcCount ||
        reverseIndex == arcIndex) {
        return 0;
    }

    PriorityArc *reverse =
        &network->arcs[reverseIndex];

    if (reverse->reverseIndex != arcIndex ||
        forward->from != reverse->to ||
        forward->to != reverse->from) {
        return 0;
    }

    if (!isfinite(forward->capacity) ||
        !isfinite(reverse->capacity) ||
        amount > forward->capacity ||
        reverse->capacity > DBL_MAX - amount) {
        return 0;
    }

    forward->capacity -= amount;
    reverse->capacity += amount;

    return 1;
}


/*
 * Convert a logical node ID into
 * its original graph-array index.
 */
static int findPriorityNodeIndex(
    const Graph *graph,
    int nodeId
)
{
    if (graph == NULL) {
        return -1;
    }

    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i].id == nodeId) {
            return i;
        }
    }

    return -1;
}


/*
 * Construct the complete priority-aware
 * residual network from an existing Graph.
 *
 * The original graph is not modified.
 */
static int buildPriorityNetwork(
    const Graph *graph,
    PriorityNetwork *network
)
{
    if (graph == NULL || network == NULL) {
        return 0;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES) {
        return 0;
    }

    /*
     * Reuse the existing Week 4 graph
     * validation and network builder.
     */
    FlowNetwork validationNetwork;

    if (!buildFlowNetwork(
            graph, &validationNetwork)) {
        return 0;
    }

    /*
     * Additional validation for priorities.
     */
    if (!validatePriorityGraph(graph)) {
        return 0;
    }

    PriorityNetwork candidate = {0};

    int n = graph->nodeCount;

    candidate.vertexCount = n + 2;
    candidate.sourceIndex = n;
    candidate.sinkIndex = n + 1;

    PriorityCost zero = zeroPriorityCost();

    /*
     * Add physical transmission lines.
     */
    for (int i = 0; i < graph->edgeCount; i++) {

        const Edge *edge = &graph->edges[i];

        if (!edge->active) {
            continue;
        }

        int u = findPriorityNodeIndex(
            graph, edge->from);

        int v = findPriorityNodeIndex(
            graph, edge->to);

        if (u < 0 || v < 0) {
            return 0;
        }

        /*
         * Equal forward/reverse residual
         * capacities model shared undirected
         * line capacity.
         */
        if (!addPriorityArcPair(
                &candidate,
                u, v,
                edge->capacity,
                edge->capacity,
                zero)) {
            return 0;
        }
    }

    /*
     * Add generator and consumer connections.
     */
    for (int i = 0; i < n; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type == GENERATOR) {

            if (!addPriorityArcPair(
                    &candidate,
                    candidate.sourceIndex,
                    i,
                    node->supply,
                    0.0,
                    zero)) {
                return 0;
            }
        }

        else if (node->type == CONSUMER) {

            PriorityCost cost =
                makeConsumerCost(node->priority);

            if (!addPriorityArcPair(
                    &candidate,
                    i,
                    candidate.sinkIndex,
                    node->demand,
                    0.0,
                    cost)) {
                return 0;
            }
        }
    }

    *network = candidate;

    return 1;
}



/*
 * Lexicographic Bellman-Ford.
 *
 * Returns:
 *  1 = sink reachable, shortest path found
 *  0 = sink unreachable
 * -1 = invalid network or negative cycle
 *
 * parentArc[v] stores the residual arc
 * used to reach vertex v.
 */


#ifdef PRIORITY_FLOW_LEGACY_TESTS


static int bellmanFordPriority(
    const PriorityNetwork *network,
    int parentArc[MAX_FLOW_VERTICES],
    PriorityCost dist[MAX_FLOW_VERTICES]
)
{
    if (network == NULL ||
        parentArc == NULL ||
        dist == NULL) {
        return -1;
    }


if (!validatePriorityNetwork(network)) {
    return -1;
}


    int n = network->vertexCount;
    int source = network->sourceIndex;
    int sink = network->sinkIndex;

    if (n < 2 || n > MAX_FLOW_VERTICES ||
        source < 0 || source >= n ||
        sink < 0 || sink >= n ||
        source == sink ||
        network->arcCount < 0 ||
        network->arcCount > MAX_PRIORITY_ARCS) {
        return -1;
    }

    int reachable[MAX_FLOW_VERTICES] = {0};

    for (int i = 0; i < n; i++) {
        parentArc[i] = -1;
        dist[i] = zeroPriorityCost();
    }

    /*
     * A separate reachable array avoids
     * using infinity inside cost vectors.
     */
    reachable[source] = 1;

    /*
     * Relax all residual arcs at most V-1 times.
     */
    for (int iteration = 0;
         iteration < n - 1;
         iteration++) {

        int changed = 0;

        for (int e = 0;
             e < network->arcCount;
             e++) {

            const PriorityArc *arc =
                &network->arcs[e];

            int u = arc->from;
            int v = arc->to;

            if (u < 0 || u >= n ||
                v < 0 || v >= n ||
                !isfinite(arc->capacity) ||
                arc->capacity < 0.0) {
                return -1;
            }

            if (!reachable[u] ||
                arc->capacity <= FLOW_EPS) {
                continue;
            }

            PriorityCost candidate =
                addPriorityCost(
                    dist[u], arc->cost);

            for (int k = 0;
                 k < PRIORITY_LEVELS;
                 k++) {

                if (!isfinite(candidate.component[k])) {
                    return -1;
                }
            }

            if (!reachable[v] ||
                comparePriorityCost(
                    candidate, dist[v]) < 0) {

                dist[v] = candidate;
                parentArc[v] = e;
                reachable[v] = 1;
                changed = 1;
            }
        }

        if (!changed) {
            break;
        }
    }

    /*
     * Detect reachable negative-cost cycles.
     */
    for (int e = 0;
         e < network->arcCount;
         e++) {

        const PriorityArc *arc =
            &network->arcs[e];

        if (arc->capacity <= FLOW_EPS ||
            !reachable[arc->from]) {
            continue;
        }

        PriorityCost candidate =
            addPriorityCost(
                dist[arc->from], arc->cost);

        for (int k = 0;
             k < PRIORITY_LEVELS;
             k++) {

            if (!isfinite(candidate.component[k])) {
                return -1;
            }
        }

        if (!reachable[arc->to] ||
            comparePriorityCost(
                candidate, dist[arc->to]) < 0) {
            return -1;
        }
    }

    return reachable[sink] ? 1 : 0;
}


#endif /* PRIORITY_FLOW_LEGACY_TESTS */


/*
 * Augment flow along a parent-arc path.
 *
 * Returns:
 * 1 = augmentation succeeded
 * 0 = invalid path or insufficient capacity
 *
 * The original network is preserved on failure.
 */
static int augmentPriorityPath(
    PriorityNetwork *network,
    const int parentArc[MAX_FLOW_VERTICES],
    double *augmentedFlow,
    PriorityCost *costChange
)
{
    if (network == NULL ||
        parentArc == NULL ||
        augmentedFlow == NULL ||
        costChange == NULL) {
        return 0;
    }

    int n = network->vertexCount;
    int source = network->sourceIndex;
    int sink = network->sinkIndex;

    if (n < 2 || n > MAX_FLOW_VERTICES ||
        source < 0 || source >= n ||
        sink < 0 || sink >= n ||
        source == sink ||
        network->arcCount < 0 ||
        network->arcCount > MAX_PRIORITY_ARCS) {
        return 0;
    }

    int path[MAX_FLOW_VERTICES];
    int pathLength = 0;

    int visited[MAX_FLOW_VERTICES] = {0};

    double bottleneck = DBL_MAX;

    /*
     * Reconstruct path backward from sink.
     */
    int v = sink;

    while (v != source) {

        if (v < 0 || v >= n ||
            visited[v] ||
            pathLength >= n) {
            return 0;
        }

        visited[v] = 1;

        int e = parentArc[v];

        if (e < 0 || e >= network->arcCount) {
            return 0;
        }

        const PriorityArc *arc =
            &network->arcs[e];

        if (arc->to != v ||
            arc->from < 0 ||
            arc->from >= n ||
            !isfinite(arc->capacity) ||
            arc->capacity <= FLOW_EPS) {
            return 0;
        }

        int reverse = arc->reverseIndex;

        if (reverse < 0 ||
            reverse >= network->arcCount ||
            reverse == e) {
            return 0;
        }

        const PriorityArc *reverseArc =
            &network->arcs[reverse];

        if (reverseArc->reverseIndex != e ||
            reverseArc->from != arc->to ||
            reverseArc->to != arc->from ||
            !isfinite(reverseArc->capacity) ||
            reverseArc->capacity < 0.0) {
            return 0;
        }

        if (arc->capacity < bottleneck) {
            bottleneck = arc->capacity;
        }

        path[pathLength++] = e;
        v = arc->from;
    }

    if (!isfinite(bottleneck) ||
        bottleneck <= FLOW_EPS) {
        return 0;
    }

    /*
     * Calculate the cost of the path.
     */
    PriorityCost pathCost =
        zeroPriorityCost();

    for (int i = 0; i < pathLength; i++) {

        PriorityArc *arc =
            &network->arcs[path[i]];

        pathCost = addPriorityCost(
            pathCost, arc->cost);
    }

    PriorityCost deltaCost =
        zeroPriorityCost();

    for (int k = 0; k < PRIORITY_LEVELS; k++) {

        deltaCost.component[k] =
            pathCost.component[k] * bottleneck;

        if (!isfinite(deltaCost.component[k])) {
            return 0;
        }
    }

    /*
     * Validate all reverse-capacity additions
     * before changing the network.
     */
    for (int i = 0; i < pathLength; i++) {

        PriorityArc *arc =
            &network->arcs[path[i]];

        PriorityArc *reverse =
            &network->arcs[arc->reverseIndex];

        if (reverse->capacity >
            DBL_MAX - bottleneck) {
            return 0;
        }
    }

    /*
     * Use a candidate copy so a failed
     * update cannot corrupt the network.
     */
    PriorityNetwork candidate = *network;

    for (int i = 0; i < pathLength; i++) {

        if (!pushPriorityFlow(
                &candidate,
                path[i],
                bottleneck)) {
            return 0;
        }
    }

    /*
     * Commit only after every update succeeds.
     */
    *network = candidate;
    *augmentedFlow = bottleneck;
    *costChange = deltaCost;

    return 1;
}


/*
 * Intermediate lexicographic flow solver.
 *
 * Handles shortest-path augmentation.
 *
 * Negative residual cycles are rejected,
 * not yet optimized.
 *
 * Returns:
 * 1 = completed without detected errors
 * 0 = invalid input or unsupported cycl
 * 
 * e
 */

#ifdef PRIORITY_FLOW_PROTOTYPE_TEST
static int runPriorityFlowPrototype(
    PriorityNetwork *network,
    double *totalFlow,
    PriorityCost *totalCost,
    int *augmentingPaths
)
{
    if (network == NULL ||
        totalFlow == NULL ||
        totalCost == NULL ||
        augmentingPaths == NULL) {
        return 0;
    }

    PriorityNetwork candidate = *network;

    double flow = 0.0;
    PriorityCost cost = zeroPriorityCost();
    int paths = 0;

    int parentArc[MAX_FLOW_VERTICES];
    PriorityCost dist[MAX_FLOW_VERTICES];

    while (1) {

        int status = bellmanFordPriority(
            &candidate,
            parentArc,
            dist
        );

        if (status < 0) {
            return 0;
        }

        if (status == 0) {
            break;
        }

        /*
         * Stop if the best source-to-sink
         * path cannot improve the objective.
         */
        PriorityCost zero =
            zeroPriorityCost();

        if (comparePriorityCost(
                dist[candidate.sinkIndex],
                zero) >= 0) {
            break;
        }

        double deltaFlow = 0.0;
        PriorityCost deltaCost =
            zeroPriorityCost();

        if (!augmentPriorityPath(
                &candidate,
                parentArc,
                &deltaFlow,
                &deltaCost)) {
            return 0;
        }

        if (!isfinite(deltaFlow) ||
            deltaFlow <= FLOW_EPS ||
            flow > DBL_MAX - deltaFlow ||
            paths == INT_MAX) {
            return 0;
        }

        flow += deltaFlow;

        cost = addPriorityCost(
            cost,
            deltaCost
        );

        for (int k = 0;
             k < PRIORITY_LEVELS;
             k++) {

            if (!isfinite(cost.component[k])) {
                return 0;
            }
        }

        paths++;
    }

    /*
     * Commit only after successful completion.
     */
    *network = candidate;
    *totalFlow = flow;
    *totalCost = cost;
    *augmentingPaths = paths;

    return 1;
}


#endif /* PRIORITY_FLOW_PROTOTYPE_TEST */

/*
 * Find a negative lexicographic residual cycle.
 *
 * Returns:
 *  1 = negative cycle found
 *  0 = no negative cycle
 * -1 = invalid network or reconstruction error
 *
 * cycleArcs[] contains residual arc indices.
 */
static int findNegativePriorityCycle(
    const PriorityNetwork *network,
    int cycleArcs[MAX_FLOW_VERTICES],
    int *cycleLength
)
{
    if (network == NULL ||
        cycleArcs == NULL ||
        cycleLength == NULL) {
        return -1;
    }


if (!validatePriorityNetwork(network)) {
    return -1;
}


    int n = network->vertexCount;

    if (n < 2 || n > MAX_FLOW_VERTICES ||
        network->arcCount < 0 ||
        network->arcCount > MAX_PRIORITY_ARCS) {
        return -1;
    }

    *cycleLength = 0;

    PriorityCost dist[MAX_FLOW_VERTICES];
    int parentArc[MAX_FLOW_VERTICES];

    for (int i = 0; i < n; i++) {
        dist[i] = zeroPriorityCost();
        parentArc[i] = -1;
    }

    /*
     * Validate residual arcs before relaxation.
     */
    for (int e = 0; e < network->arcCount; e++) {

        const PriorityArc *arc = &network->arcs[e];

        if (arc->from < 0 || arc->from >= n ||
            arc->to < 0 || arc->to >= n ||
            !isfinite(arc->capacity) ||
            arc->capacity < 0.0) {
            return -1;
        }

        for (int k = 0; k < PRIORITY_LEVELS; k++) {
            if (!isfinite(arc->cost.component[k])) {
                return -1;
            }
        }
    }

    int updatedVertex = -1;

    /*
     * Initialize all distances to zero.
     * This detects cycles anywhere in the graph.
     */
    for (int iteration = 0; iteration < n;
         iteration++) {

        updatedVertex = -1;

        for (int e = 0; e < network->arcCount; e++) {

            const PriorityArc *arc =
                &network->arcs[e];

            if (arc->capacity <= FLOW_EPS) {
                continue;
            }

            PriorityCost candidate =
                addPriorityCost(
                    dist[arc->from],
                    arc->cost
                );

            for (int k = 0; k < PRIORITY_LEVELS; k++) {
                if (!isfinite(candidate.component[k])) {
                    return -1;
                }
            }

            if (comparePriorityCost(
                    candidate,
                    dist[arc->to]) < 0) {

                dist[arc->to] = candidate;
                parentArc[arc->to] = e;
                updatedVertex = arc->to;
            }
        }

        if (updatedVertex == -1) {
            return 0;
        }
    }

    /*
     * An update on iteration V indicates
     * a negative-cost cycle.
     *
     * Follow predecessors V times to
     * enter the cycle.
     */
    int vertex = updatedVertex;

    for (int i = 0; i < n; i++) {

        int e = parentArc[vertex];

        if (e < 0 || e >= network->arcCount ||
            network->arcs[e].to != vertex) {
            return -1;
        }

        vertex = network->arcs[e].from;
    }

    int start = vertex;
    int length = 0;

    /*
     * Reconstruct cycle by following
     * predecessor arcs.
     */
    do {

        if (length >= n) {
            return -1;
        }

        int e = parentArc[vertex];

        if (e < 0 || e >= network->arcCount ||
            network->arcs[e].to != vertex) {
            return -1;
        }

        cycleArcs[length++] = e;
        vertex = network->arcs[e].from;

    } while (vertex != start);

    /*
     * Verify that the reconstructed
     * cycle has negative total cost.
     */
    PriorityCost totalCost =
        zeroPriorityCost();

    for (int i = 0; i < length; i++) {
        totalCost = addPriorityCost(
            totalCost,
            network->arcs[cycleArcs[i]].cost
        );
    }

    if (comparePriorityCost(
            totalCost,
            zeroPriorityCost()) >= 0) {
        return -1;
    }

    *cycleLength = length;

    return 1;
}



/*
 * Augment flow around a negative residual cycle.
 *
 * Returns:
 * 1 = success
 * 0 = invalid cycle or failed update
 *
 * The original network remains unchanged
 * if augmentation fails.
 */
static int augmentNegativePriorityCycle(
    PriorityNetwork *network,
    const int cycleArcs[MAX_FLOW_VERTICES],
    int cycleLength,
    double *augmentedFlow,
    PriorityCost *costChange
)
{
    if (network == NULL ||
        cycleArcs == NULL ||
        augmentedFlow == NULL ||
        costChange == NULL) {
        return 0;
    }

    int n = network->vertexCount;

    if (n < 2 || n > MAX_FLOW_VERTICES ||
        cycleLength < 2 ||
        cycleLength > n ||
        network->arcCount < 0 ||
        network->arcCount > MAX_PRIORITY_ARCS) {
        return 0;
    }

    double bottleneck = DBL_MAX;
    PriorityCost cycleCost =
        zeroPriorityCost();

    /*
     * Validate all cycle arcs.
     *
     * The cycle is stored in predecessor
     * traversal order.
     */
    for (int i = 0; i < cycleLength; i++) {

        int e = cycleArcs[i];

        if (e < 0 || e >= network->arcCount) {
            return 0;
        }

        const PriorityArc *arc =
            &network->arcs[e];

        int nextIndex =
            cycleArcs[(i + 1) % cycleLength];

        if (nextIndex < 0 ||
            nextIndex >= network->arcCount) {
            return 0;
        }

        const PriorityArc *next =
            &network->arcs[nextIndex];

        /*
         * Predecessor-order cycle condition.
         */
        if (arc->from != next->to) {
            return 0;
        }

        if (arc->from < 0 || arc->from >= n ||
            arc->to < 0 || arc->to >= n ||
            !isfinite(arc->capacity) ||
            arc->capacity <= FLOW_EPS) {
            return 0;
        }

        int reverseIndex = arc->reverseIndex;

        if (reverseIndex < 0 ||
            reverseIndex >= network->arcCount ||
            reverseIndex == e) {
            return 0;
        }

        const PriorityArc *reverse =
            &network->arcs[reverseIndex];

        if (reverse->reverseIndex != e ||
            reverse->from != arc->to ||
            reverse->to != arc->from ||
            !isfinite(reverse->capacity) ||
            reverse->capacity < 0.0) {
            return 0;
        }

        if (arc->capacity < bottleneck) {
            bottleneck = arc->capacity;
        }

        cycleCost = addPriorityCost(
            cycleCost,
            arc->cost
        );

        for (int k = 0; k < PRIORITY_LEVELS; k++) {
            if (!isfinite(cycleCost.component[k])) {
                return 0;
            }
        }
    }

    /*
     * Only negative-cost cycles improve
     * the lexicographic objective.
     */
    if (comparePriorityCost(
            cycleCost,
            zeroPriorityCost()) >= 0) {
        return 0;
    }

    if (!isfinite(bottleneck) ||
        bottleneck <= FLOW_EPS) {
        return 0;
    }

    PriorityCost deltaCost =
        zeroPriorityCost();

    for (int k = 0; k < PRIORITY_LEVELS; k++) {

        deltaCost.component[k] =
            cycleCost.component[k] * bottleneck;

        if (!isfinite(deltaCost.component[k])) {
            return 0;
        }
    }

    /*
     * Work on a candidate copy.
     */
    PriorityNetwork candidate = *network;

    for (int i = 0; i < cycleLength; i++) {

        if (!pushPriorityFlow(
                &candidate,
                cycleArcs[i],
                bottleneck)) {
            return 0;
        }
    }

    /*
     * Commit only after successful updates.
     */
    *network = candidate;
    *augmentedFlow = bottleneck;
    *costChange = deltaCost;

    return 1;
}


/*
 * Repeatedly cancel negative residual cycles.
 *
 * Returns:
 * 1 = no negative cycles remain
 * 0 = invalid input or optimization failure
 *
 * Original network is unchanged on failure.
 */
static int cancelNegativePriorityCycles(
    PriorityNetwork *network,
    int maxIterations,
    int *cyclesCanceled,
    PriorityCost *totalCostChange
)
{
    if (network == NULL ||
        cyclesCanceled == NULL ||
        totalCostChange == NULL ||
        maxIterations <= 0) {
        return 0;
    }

    /*
     * Work on a candidate network.
     */
    PriorityNetwork candidate = *network;

    PriorityCost accumulated =
        zeroPriorityCost();

    int count = 0;

    while (1) {

        int cycleArcs[MAX_FLOW_VERTICES];
        int cycleLength = 0;

        int status = findNegativePriorityCycle(
            &candidate,
            cycleArcs,
            &cycleLength
        );

        if (status < 0) {
            return 0;
        }

        /*
         * No negative cycle remains.
         */
        if (status == 0) {
            break;
        }

        /*
         * Never report success when the
         * safety limit is exhausted.
         */
        if (count >= maxIterations) {
            return 0;
        }

        double deltaFlow = 0.0;

        PriorityCost deltaCost =
            zeroPriorityCost();

        if (!augmentNegativePriorityCycle(
                &candidate,
                cycleArcs,
                cycleLength,
                &deltaFlow,
                &deltaCost)) {
            return 0;
        }

        if (!isfinite(deltaFlow) ||
            deltaFlow <= FLOW_EPS) {
            return 0;
        }

        accumulated = addPriorityCost(
            accumulated,
            deltaCost
        );

        for (int k = 0;
             k < PRIORITY_LEVELS;
             k++) {

            if (!isfinite(
                    accumulated.component[k])) {
                return 0;
            }
        }

        count++;
    }

    /*
     * Commit only after successful
     * optimization.
     */
    *network = candidate;
    *cyclesCanceled = count;
    *totalCostChange = accumulated;

    return 1;
}


/*
 * Extract consumer deliveries from an
 * optimized residual network.
 *
 * Returns 1 on success, 0 on failure.
 * Does not modify graph or network.
 */
static int extractPriorityFlowResult(
    const Graph *graph,
    const PriorityNetwork *network,
    int augmentingPaths,
    PriorityFlowResult *result
)
{
    if (graph == NULL || network == NULL ||
        result == NULL ||
        augmentingPaths < 0 ||
        !validatePriorityGraph(graph) ||
        !validatePriorityNetwork(network)) {
        return 0;
    }

    if (network->vertexCount != graph->nodeCount + 2 ||
        network->sourceIndex != graph->nodeCount ||
        network->sinkIndex != graph->nodeCount + 1) {
        return 0;
    }

    PriorityFlowResult candidate = {0};
    int seen[MAX_NODES] = {0};
    int sink = network->sinkIndex;

    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (!isfinite(node->demand) ||
            !isfinite(node->supply) ||
            node->demand < 0.0 ||
            node->supply < 0.0) {
            return 0;
        }

        if (node->type == GENERATOR) {
            if (candidate.totalSupply >
                DBL_MAX - node->supply) {
                return 0;
            }

            candidate.totalSupply += node->supply;
        }
        else if (node->type == CONSUMER) {
            if (candidate.totalDemand >
                DBL_MAX - node->demand) {
                return 0;
            }

            candidate.totalDemand += node->demand;
        }
    }

    /*
     * Find each consumer's forward sink arc.
     */
    for (int e = 0; e < network->arcCount; e++) {

        const PriorityArc *arc = &network->arcs[e];

        if (arc->to != sink ||
            arc->from < 0 ||
            arc->from >= graph->nodeCount) {
            continue;
        }

        int i = arc->from;
        const Node *node = &graph->nodes[i];

        if (node->type != CONSUMER) {
            continue;
        }

        /*
         * Forward consumer arcs have
         * negative priority cost.
         */
        PriorityCost expected =
            makeConsumerCost(node->priority);

        if (comparePriorityCost(
                arc->cost, expected) != 0 ||
            seen[i]) {
            return 0;
        }

        seen[i] = 1;

        double delivered =
            node->demand - arc->capacity;

        if (!isfinite(delivered) ||
            delivered < -FLOW_EPS ||
            delivered > node->demand + FLOW_EPS) {
            return 0;
        }

        if (delivered < 0.0) {
            delivered = 0.0;
        }

        if (delivered > node->demand) {
            delivered = node->demand;
        }

        candidate.deliveredToNode[i] = delivered;

        int p = node->priority - 1;

        if (candidate.deliveredByPriority[p] >
            DBL_MAX - delivered ||
            candidate.totalDelivered >
            DBL_MAX - delivered) {
            return 0;
        }

        candidate.deliveredByPriority[p] += delivered;
        candidate.totalDelivered += delivered;
    }

    /*
     * Every consumer must have exactly
     * one forward consumer-to-sink arc.
     */
    for (int i = 0; i < graph->nodeCount; i++) {
        if (graph->nodes[i].type == CONSUMER &&
            !seen[i]) {
            return 0;
        }
    }

    if (candidate.totalDelivered >
            candidate.totalSupply + FLOW_EPS ||
        candidate.totalDelivered >
            candidate.totalDemand + FLOW_EPS) {
        return 0;
    }

    candidate.totalUnserved =
        candidate.totalDemand -
        candidate.totalDelivered;

    if (candidate.totalUnserved < 0.0 &&
        candidate.totalUnserved > -FLOW_EPS) {
        candidate.totalUnserved = 0.0;
    }

    if (candidate.totalUnserved < 0.0) {
        return 0;
    }

    candidate.augmentingPaths = augmentingPaths;

    *result = candidate;
    return 1;
}


/*
 * Validate generator output against supply.
 *
 * Returns:
 * 1 = all generator outputs valid
 * 0 = invalid allocation or network
 *
 * This checks generator source arcs.
 * Full flow conservation is tested later.
 */
static int validateGeneratorSupply(
    const Graph *graph,
    const PriorityNetwork *network
)
{
    if (graph == NULL ||
        network == NULL ||
        !validatePriorityNetwork(network)) {
        return 0;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        network->vertexCount != graph->nodeCount + 2 ||
        network->sourceIndex != graph->nodeCount ||
        network->sinkIndex != graph->nodeCount + 1) {
        return 0;
    }

    int source = network->sourceIndex;

    int seen[MAX_NODES] = {0};

    for (int e = 0; e < network->arcCount; e++) {

        const PriorityArc *arc =
            &network->arcs[e];

        if (arc->from != source ||
            arc->to < 0 ||
            arc->to >= graph->nodeCount) {
            continue;
        }

        int i = arc->to;
        const Node *node = &graph->nodes[i];

        if (node->type != GENERATOR) {
            return 0;
        }

        if (seen[i]) {
            return 0;
        }

        seen[i] = 1;

        if (!isfinite(node->supply) ||
            node->supply < 0.0) {
            return 0;
        }

        double supplied =
            node->supply - arc->capacity;

        if (!isfinite(supplied) ||
            supplied < -FLOW_EPS ||
            supplied > node->supply + FLOW_EPS) {
            return 0;
        }
    }

    /*
     * Every generator must have exactly
     * one source-to-generator arc.
     */
    for (int i = 0; i < graph->nodeCount; i++) {

        if (graph->nodes[i].type == GENERATOR &&
            !seen[i]) {
            return 0;
        }
    }

    return 1;
}


/*
 * Week 5 - Priority-Aware Flow Planner
 *
 * Phase 1: Maximum feasible flow
 * Phase 2: Lexicographic cycle optimization
 * Phase 3: Extract and validate results
 *
 * Returns:
 * 1 = success
 * 0 = invalid input or optimization failure
 */
int calculatePriorityFlow(
    const Graph *graph,
    PriorityFlowResult *result
)
{
    if (graph == NULL || result == NULL) {
        return 0;
    }

    /*
     * Build the priority residual network.
     */
    PriorityNetwork network;

    if (!buildPriorityNetwork(graph, &network)) {
        return 0;
    }

    /*
     * Phase 1: Calculate maximum flow.
     */
    double maximumFlow = 0.0;
    int augmentingPaths = 0;

    if (!runPriorityMaximumFlow(
            &network,
            &maximumFlow,
            &augmentingPaths)) {
        return 0;
    }

    /*
     * Phase 2: Improve lexicographic
     * priority allocation.
     *
     * Temporary iteration safeguard.
     */
    int cyclesCanceled = 0;

    PriorityCost costChange =
        zeroPriorityCost();

    const int maxCycleIterations = 10000;

    if (!cancelNegativePriorityCycles(
            &network,
            maxCycleIterations,
            &cyclesCanceled,
            &costChange)) {
        return 0;
    }


/*
 * Validate the optimized residual network.
 *
 * This includes:
 * - Generator supply constraints
 * - Consumer demand constraints
 * - Shared transmission-line capacities
 * - Flow conservation at every node
 */
if (!validateFlowConservation(
        graph,
        &network)) {
    return 0;
}


    /*
     * Phase 3: Extract consumer allocations.
     */
    PriorityFlowResult candidate = {0};

    if (!extractPriorityFlowResult(
            graph,
            &network,
            augmentingPaths,
            &candidate)) {
        return 0;
    }

    /*
     * Verify delivered power matches
     * the maximum-flow calculation.
     */
    double tolerance =
        FLOW_EPS * (1.0 + fabs(maximumFlow));

    if (fabs(candidate.totalDelivered -
             maximumFlow) > tolerance) {
        return 0;
    }

    /*
     * Commit only after all phases succeed.
     */
    *result = candidate;

    return 1;
}


/*
 * Validate each consumer's delivered power.
 *
 * Returns:
 * 1 = all consumer sink allocations valid
 * 0 = invalid network or allocation
 *
 * Does not independently verify flow
 * conservation or physical line capacities.
 */
static int validateConsumerDemand(
    const Graph *graph,
    const PriorityNetwork *network
)
{
    if (graph == NULL ||
        network == NULL ||
        !validatePriorityGraph(graph) ||
        !validatePriorityNetwork(network)) {
        return 0;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        network->vertexCount != graph->nodeCount + 2 ||
        network->sourceIndex != graph->nodeCount ||
        network->sinkIndex != graph->nodeCount + 1) {
        return 0;
    }

    int sink = network->sinkIndex;
    int seen[MAX_NODES] = {0};

    for (int e = 0; e < network->arcCount; e++) {

        const PriorityArc *arc =
            &network->arcs[e];

        if (arc->to != sink ||
            arc->from < 0 ||
            arc->from >= graph->nodeCount) {
            continue;
        }

        int i = arc->from;
        const Node *node = &graph->nodes[i];

        if (node->type != CONSUMER ||
            seen[i] ||
            !isfinite(node->demand) ||
            node->demand < 0.0) {
            return 0;
        }

        PriorityCost expectedCost =
            makeConsumerCost(node->priority);

        if (comparePriorityCost(
                arc->cost, expectedCost) != 0) {
            return 0;
        }

        seen[i] = 1;

        double delivered =
            node->demand - arc->capacity;

        if (!isfinite(delivered) ||
            delivered < -FLOW_EPS ||
            delivered > node->demand + FLOW_EPS) {
            return 0;
        }

        /*
         * Verify the reverse sink residual
         * agrees with the delivered amount.
         */
        const PriorityArc *reverse =
            &network->arcs[arc->reverseIndex];

        double tolerance =
            FLOW_EPS * (1.0 + node->demand);

        if (fabs(reverse->capacity - delivered) >
            tolerance) {
            return 0;
        }
    }

    /*
     * Every consumer must have exactly
     * one forward consumer-to-sink arc.
     */
    for (int i = 0; i < graph->nodeCount; i++) {

        if (graph->nodes[i].type == CONSUMER &&
            !seen[i]) {
            return 0;
        }
    }

    return 1;
}


/*
 * Validate every physical transmission line.
 *
 * Physical lines are inserted first by
 * buildPriorityNetwork(), in edge-list order.
 *
 * Each active line uses one residual arc pair.
 * Inactive lines use no residual arcs.
 *
 * Returns:
 * 1 = valid transmission capacities
 * 0 = invalid network or line flow
 */
static int validateTransmissionCapacity(
    const Graph *graph,
    const PriorityNetwork *network
)
{
    if (graph == NULL ||
        network == NULL ||
        graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES ||
        !validatePriorityNetwork(network)) {
        return 0;
    }

    if (network->vertexCount != graph->nodeCount + 2 ||
        network->sourceIndex != graph->nodeCount ||
        network->sinkIndex != graph->nodeCount + 1) {
        return 0;
    }

    int arcIndex = 0;

    for (int i = 0; i < graph->edgeCount; i++) {

        const Edge *edge = &graph->edges[i];

        if (edge->active != 0 && edge->active != 1) {
            return 0;
        }

        if (!isfinite(edge->capacity) ||
            edge->capacity < 0.0) {
            return 0;
        }

        /*
         * Failed lines are not inserted
         * into the residual network.
         */
        if (!edge->active) {
            continue;
        }

        if (arcIndex > network->arcCount - 2) {
            return 0;
        }

        int u = findPriorityNodeIndex(
            graph, edge->from);

        int v = findPriorityNodeIndex(
            graph, edge->to);

        if (u < 0 || v < 0 || u == v) {
            return 0;
        }

        const PriorityArc *forward =
            &network->arcs[arcIndex];

        const PriorityArc *reverse =
            &network->arcs[arcIndex + 1];

        if (forward->from != u ||
            forward->to != v ||
            reverse->from != v ||
            reverse->to != u ||
            forward->reverseIndex != arcIndex + 1 ||
            reverse->reverseIndex != arcIndex) {
            return 0;
        }

        /*
         * Transmission arcs have zero cost.
         */
        PriorityCost zero = zeroPriorityCost();

        if (comparePriorityCost(
                forward->cost, zero) != 0 ||
            comparePriorityCost(
                reverse->cost, zero) != 0) {
            return 0;
        }

        double capacity = edge->capacity;

        double signedFlow =
            capacity - forward->capacity;

        double reverseFlow =
            reverse->capacity - capacity;

        double tolerance =
            FLOW_EPS * (1.0 + capacity);

        if (!isfinite(signedFlow) ||
            !isfinite(reverseFlow) ||
            fabs(signedFlow - reverseFlow) >
                tolerance ||
            signedFlow < -capacity - tolerance ||
            signedFlow > capacity + tolerance) {
            return 0;
        }

        arcIndex += 2;
    }

    /*
     * After physical lines, remaining arcs
     * must be generator/consumer connections.
     */
    for (int e = arcIndex; e < network->arcCount; e++) {

        const PriorityArc *arc =
            &network->arcs[e];

        if (arc->from != network->sourceIndex &&
            arc->to != network->sourceIndex &&
            arc->from != network->sinkIndex &&
            arc->to != network->sinkIndex) {
            return 0;
        }
    }

    return 1;
}


/*
 * Validate flow conservation at every
 * original graph node.
 *
 * Balance convention:
 * + incoming transmission flow
 * - outgoing transmission flow
 * + generator output
 * - consumer delivery
 *
 * Returns 1 if balanced, otherwise 0.
 */
static int validateFlowConservation(
    const Graph *graph,
    const PriorityNetwork *network
)
{
    if (graph == NULL || network == NULL ||
        !validateGeneratorSupply(graph, network) ||
        !validateConsumerDemand(graph, network) ||
        !validateTransmissionCapacity(graph, network)) {
        return 0;
    }

    int n = graph->nodeCount;
    int source = network->sourceIndex;
    int sink = network->sinkIndex;

    double balance[MAX_NODES] = {0};
    double magnitude[MAX_NODES] = {0};

    /*
     * Physical transmission arcs are
     * stored first, in edge-list order.
     */
    int arcIndex = 0;

    for (int e = 0; e < graph->edgeCount; e++) {

        const Edge *edge = &graph->edges[e];

        if (!edge->active) {
            continue;
        }

        int u = findPriorityNodeIndex(
            graph, edge->from);

        int v = findPriorityNodeIndex(
            graph, edge->to);

        if (u < 0 || v < 0) {
            return 0;
        }

        double flow = edge->capacity -
            network->arcs[arcIndex].capacity;

        /*
         * Positive flow travels u -> v.
         */
        balance[u] -= flow;
        balance[v] += flow;

        magnitude[u] += fabs(flow);
        magnitude[v] += fabs(flow);

        arcIndex += 2;
    }

    /*
     * Process generator and consumer arcs.
     */
    for (int e = arcIndex;
         e < network->arcCount;
         e++) {

        const PriorityArc *arc =
            &network->arcs[e];

        /*
         * Generator forward arc:
         * Source -> Generator
         */
        if (arc->from == source &&
            arc->to >= 0 &&
            arc->to < n &&
            graph->nodes[arc->to].type == GENERATOR) {

            int i = arc->to;

            double output =
                graph->nodes[i].supply -
                arc->capacity;

            balance[i] += output;
            magnitude[i] += fabs(output);
        }

        /*
         * Consumer forward arc:
         * Consumer -> Sink
         */
        if (arc->to == sink &&
            arc->from >= 0 &&
            arc->from < n &&
            graph->nodes[arc->from].type == CONSUMER) {

            int i = arc->from;

            double delivered =
                graph->nodes[i].demand -
                arc->capacity;

            balance[i] -= delivered;
            magnitude[i] += fabs(delivered);
        }
    }

    /*
     * Every original graph node must balance.
     */
    for (int i = 0; i < n; i++) {

        if (!isfinite(balance[i]) ||
            !isfinite(magnitude[i])) {
            return 0;
        }

        double tolerance =
            FLOW_EPS * (1.0 + magnitude[i]);

        if (fabs(balance[i]) > tolerance) {
            return 0;
        }
    }

    return 1;
}


/*
 * Intermediate lexicographic flow solver.
 *
 * Handles shortest-path augmentation.
 *
 * Negative residual cycles are rejected,
 * not yet optimized.
 *
 * Returns:
 * 1 = completed without detected errors
 * 0 = invalid input or unsupported cycle
 */


/*
 * Validate priority values.
 *
 * Consumer priorities must be 1, 2 or 3.
 */
static int validatePriorityGraph(
    const Graph *graph
)
{
    if (graph == NULL) {
        return 0;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES) {
        return 0;
    }

    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type == CONSUMER) {

            if (node->priority < 1 ||
                node->priority > PRIORITY_LEVELS) {
                return 0;
            }
        }
    }

    return 1;
}

#ifdef PRIORITY_FLOW_TEST

/*
 * Build a flow network with consumer demand
 * initially disabled.
 *
 * The original graph remains unchanged.
 */
static int initializePriorityNetwork(
    const Graph *graph,
    FlowNetwork *network
)
{
    if (network == NULL ||
        !validatePriorityGraph(graph)) {
        return 0;
    }

    /*
     * Reuse the Week 4 network builder.
     * This also validates the graph's flow
     * structure and transmission capacities.
     */
    if (!buildFlowNetwork(graph, network)) {
        return 0;
    }

    int sink = network->sinkIndex;

    /*
     * Initially disable every consumer's
     * connection to the super-sink.
     */
    for (int i = 0; i < graph->nodeCount; i++) {

        if (graph->nodes[i].type == CONSUMER) {

            network->residual[i][sink] = 0.0;
        }
    }

    return 1;
}


/*
 * Activate consumers belonging to one priority.
 *
 * Only previously disabled consumer-to-sink
 * edges are activated.
 *
 * Returns:
 * 1 = success
 * 0 = invalid input
 */
static int activatePriorityClass(
    const Graph *graph,
    FlowNetwork *network,
    int priority
)
{
    if (graph == NULL || network == NULL ||
        priority < 1 ||
        priority > PRIORITY_LEVELS) {
        return 0;
    }

    
    if (network->vertexCount !=
            graph->nodeCount + 2 ||
        network->sourceIndex !=
            graph->nodeCount ||
        network->sinkIndex !=
            graph->nodeCount + 1) {
        return 0;
    }


    int sink = network->sinkIndex;

    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type != CONSUMER ||
            node->priority != priority) {
            continue;
        }

        network->residual[i][sink] =
            node->demand;
    }

    return 1;
}

// #ifdef PRIORITY_FLOW_TEST

int main(void)
{
    Graph graph = {0};
    FlowNetwork network;

    graph.nodeCount = 2;
    graph.edgeCount = 1;

    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 60.0;

    graph.nodes[1].id = 20;
    graph.nodes[1].type = CONSUMER;
    graph.nodes[1].demand = 20.0;
    graph.nodes[1].priority = 1;

    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 30.0;
    graph.edges[0].active = 1;

    if (!initializePriorityNetwork(
            &graph, &network)) {
        printf("[FAIL] Initialization\n");
        return 1;
    }

    int sink = network.sinkIndex;

    if (network.residual[1][sink] != 0.0) {
        printf("[FAIL] Consumer sink not disabled\n");
        return 1;
    }

    if (network.residual[0][1] != 30.0) {
        printf("[FAIL] Transmission capacity changed\n");
        return 1;
    }

    printf("[PASS] Priority network initialization\n");


/*
 * Activate Priority 1 consumers.
 */
if (!activatePriorityClass(
        &graph, &network, 1)) {
    printf("[FAIL] Priority 1 activation\n");
    return 1;
}

if (network.residual[1][sink] != 20.0) {
    printf("[FAIL] Priority 1 demand\n");
    return 1;
}

/*
 * Run Maximum Flow for Priority 1 only.
 */
double stageFlow = 0.0;
int pathCount = 0;

if (!edmondsKarp(
        &network,
        &stageFlow,
        &pathCount)) {
    printf("[FAIL] Priority 1 flow\n");
    return 1;
}

if (fabs(stageFlow - 20.0) > FLOW_EPS) {
    printf("[FAIL] Incorrect Priority 1 flow\n");
    return 1;
}

printf("[PASS] Priority 1 activation\n");
printf("[PASS] Priority 1 Maximum Flow\n");
printf("Priority 1 Delivered: %.2f MW\n",
       stageFlow);
printf("Augmenting Paths: %d\n", pathCount);


/*
 * Add a second consumer with priority 2
 * in a separate small test network.
 */
Graph stageGraph = {0};
FlowNetwork stageNetwork;

stageGraph.nodeCount = 3;
stageGraph.edgeCount = 2;

/* Generator */
stageGraph.nodes[0].id = 10;
stageGraph.nodes[0].type = GENERATOR;
stageGraph.nodes[0].supply = 30.0;

/* Hospital */
stageGraph.nodes[1].id = 20;
stageGraph.nodes[1].type = CONSUMER;
stageGraph.nodes[1].demand = 20.0;
stageGraph.nodes[1].priority = 1;

/* School */
stageGraph.nodes[2].id = 30;
stageGraph.nodes[2].type = CONSUMER;
stageGraph.nodes[2].demand = 10.0;
stageGraph.nodes[2].priority = 2;

/* Generator -> Hospital */
stageGraph.edges[0].from = 10;
stageGraph.edges[0].to = 20;
stageGraph.edges[0].capacity = 20.0;
stageGraph.edges[0].active = 1;

/* Generator -> School */
stageGraph.edges[1].from = 10;
stageGraph.edges[1].to = 30;
stageGraph.edges[1].capacity = 10.0;
stageGraph.edges[1].active = 1;

if (!initializePriorityNetwork(
        &stageGraph, &stageNetwork)) {
    printf("[FAIL] Two-stage initialization\n");
    return 1;
}

/* Stage 1 */
if (!activatePriorityClass(
        &stageGraph, &stageNetwork, 1)) {
    return 1;
}

double flow1 = 0.0;
int paths1 = 0;

if (!edmondsKarp(
        &stageNetwork, &flow1, &paths1)) {
    return 1;
}

int stageSink = stageNetwork.sinkIndex;

double hospitalBefore =
    20.0 - stageNetwork.residual[1][stageSink];

/* Stage 2 */
if (!activatePriorityClass(
        &stageGraph, &stageNetwork, 2)) {
    return 1;
}

double flow2 = 0.0;
int paths2 = 0;

if (!edmondsKarp(
        &stageNetwork, &flow2, &paths2)) {
    return 1;
}

double hospitalAfter =
    20.0 - stageNetwork.residual[1][stageSink];

double schoolDelivered =
    10.0 - stageNetwork.residual[2][stageSink];

if (fabs(flow1 - 20.0) > FLOW_EPS ||
    fabs(flow2 - 10.0) > FLOW_EPS ||
    fabs(hospitalBefore - hospitalAfter) > FLOW_EPS ||
    fabs(schoolDelivered - 10.0) > FLOW_EPS) {

    printf("[FAIL] Priority preservation\n");
    return 1;
}

printf("[PASS] Two-stage priority preservation\n");
printf("Hospital: %.2f MW\n", hospitalAfter);
printf("School: %.2f MW\n", schoolDelivered);
printf("Total: %.2f MW\n", flow1 + flow2);


    return 0;
}

#endif /* PRIORITY_FLOW_TEST */

/*
 * Display the final priority-aware
 * electricity allocation.
 */
void displayPriorityFlowReport(
    const Graph *graph,
    const PriorityFlowResult *result
)
{
    if (graph == NULL || result == NULL) {
        printf("Error: Invalid Priority Flow report.\n");
        return;
    }

    printf("\n========================================\n");
    printf("       SMART GRID PRIORITY FLOW\n");
    printf("========================================\n");

    printf("Total Supply       : %.2f MW\n",
           result->totalSupply);

    printf("Total Demand       : %.2f MW\n",
           result->totalDemand);

    printf("Total Delivered    : %.2f MW\n",
           result->totalDelivered);

    printf("Unserved Demand    : %.2f MW\n",
           result->totalUnserved);

    printf("Augmenting Paths   : %d\n",
           result->augmentingPaths);

    printf("\nPriority Allocations:\n");

    for (int p = 0; p < PRIORITY_LEVELS; p++) {
        printf("Priority %-2d         : %.2f MW\n",
               p + 1,
               result->deliveredByPriority[p]);
    }

    printf("\nConsumer Allocations:\n");

    printf("%-8s %-20s %-10s %-12s %-12s\n",
           "Node ID", "Name", "Priority",
           "Demand", "Delivered");

    printf("------------------------------------------------"
           "----------------\n");

    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type != CONSUMER) {
            continue;
        }

        printf("%-8d %-20.20s %-10d %-12.2f %-12.2f\n",
               node->id,
               node->name,
               node->priority,
               node->demand,
               result->deliveredToNode[i]);
    }

    printf("\nNote: Higher-priority consumers are\n");
    printf("preferred using lexicographic optimization.\n");

    printf("========================================\n");
}
