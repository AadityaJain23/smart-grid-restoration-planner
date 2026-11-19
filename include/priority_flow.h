
#ifndef PRIORITY_FLOW_H
#define PRIORITY_FLOW_H

#include "graph.h"
#include "maxflow.h"

/*
 * Week 5 - Priority-Aware Maximum Flow
 *
 * Objectives:
 * 1. Serve higher-priority consumers first.
 * 2. Respect generator supply.
 * 3. Respect transmission-line capacities.
 * 4. Support active and failed lines.
 * 5. Report consumer allocations.
 */

#define PRIORITY_LEVELS 3


/*
 * Lexicographic cost vector.
 *
 * component[0] = Priority 1 cost
 * component[1] = Priority 2 cost
 * component[2] = Priority 3 cost
 */
typedef struct {
    double component[PRIORITY_LEVELS];
} PriorityCost;

/*
 * One directed residual arc.
 *
 * reverseIndex identifies the corresponding
 * reverse arc in the residual edge array.
 */
typedef struct {
    int from;
    int to;
    int reverseIndex;

    double capacity;
    PriorityCost cost;
} PriorityArc;

/*
 * Each physical transmission line will be
 * represented using residual arcs.
 *
 * MAX_EDGES physical lines require up to
 * four directed residual arcs each.
 *
 * Generator and consumer connections
 * require two residual arcs each.
 */
#define MAX_PRIORITY_ARCS \
    (4 * MAX_EDGES + 4 * MAX_NODES)

/*
 * Priority-aware residual network.
 */
typedef struct {
    PriorityArc arcs[MAX_PRIORITY_ARCS];

    int arcCount;
    int vertexCount;

    int sourceIndex;
    int sinkIndex;
} PriorityNetwork;


typedef struct {

    double totalDelivered;

    double totalSupply;
    double totalDemand;
    double totalUnserved;

    double deliveredByPriority[PRIORITY_LEVELS];

    double deliveredToNode[MAX_NODES];

    int augmentingPaths;

} PriorityFlowResult;

/*
 * Calculates priority-aware power allocation.
 *
 * Returns:
 * 1 = success
 * 0 = invalid input or calculation failure
 *
 * Does not modify the original graph.
 */
int calculatePriorityFlow(
    const Graph *graph,
    PriorityFlowResult *result
);

/*
 * Displays priority-aware restoration results.
 */
void displayPriorityFlowReport(
    const Graph *graph,
    const PriorityFlowResult *result
);

#endif
