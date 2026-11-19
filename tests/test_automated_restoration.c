
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "graph.h"
#include "automated_restoration.h"

#define EPSILON 1e-6

static void addTestNode(
    Graph *graph,
    int id,
    NodeType type,
    double supply,
    double demand,
    int priority
) {
    Node node = {0};

    node.id = id;
    snprintf(node.name, NAME_SIZE, "Node%d", id);
    node.type = type;
    node.supply = supply;
    node.demand = demand;
    node.priority = priority;

    assert(addNode(graph, node) == 1);
}

static void addTestEdge(
    Graph *graph,
    int from,
    int to,
    double capacity
) {
    Edge edge = {0};

    edge.from = from;
    edge.to = to;
    edge.distance = 1.0;
    edge.capacity = capacity;
    edge.cost = 1.0;
    edge.active = 1;

    assert(addEdge(graph, edge) == 1);
}

/*
 * Small network:
 *
 * Generator(0) -- Substation(10) -- Hospital(20)
 *
 * Supply: 10 MW
 * Demand: 10 MW
 * Both lines: 10 MW capacity
 */
static void createTestNetwork(Graph *graph) {
    initializeGraph(graph);

    addTestNode(
        graph, 0, GENERATOR, 10.0, 0.0, 1
    );

    addTestNode(
        graph, 10, SUBSTATION, 0.0, 0.0, 1
    );

    addTestNode(
        graph, 20, CONSUMER, 0.0, 10.0, 1
    );

    addTestEdge(graph, 0, 10, 10.0);
    addTestEdge(graph, 10, 20, 10.0);
}

static void testNoFailure(void) {
    Graph graph = {0};
    AutomatedRestorationResult result = {0};

    createTestNetwork(&graph);

    assert(analyzeAutomatedRestoration(
        &graph, NULL, 0, &result
    ) == 1);

    assert(fabs(
        result.beforeFailure.totalDelivered - 10.0
    ) < EPSILON);

    assert(fabs(
        result.afterFailure.totalDelivered - 10.0
    ) < EPSILON);

    printf("[PASS] No additional failures\n");
}

static void testSingleFailure(void) {
    Graph graph = {0};
    AutomatedRestorationResult result = {0};

    createTestNetwork(&graph);

    int failed[] = {0};

    assert(analyzeAutomatedRestoration(
        &graph, failed, 1, &result
    ) == 1);

    assert(fabs(
        result.beforeFailure.totalDelivered - 10.0
    ) < EPSILON);

    assert(fabs(
        result.afterFailure.totalDelivered
    ) < EPSILON);

    assert(fabs(
        result.afterFailure.totalUnserved - 10.0
    ) < EPSILON);

    printf("[PASS] Single bridge failure\n");
}

static void testMultipleFailures(void) {
    Graph graph = {0};
    AutomatedRestorationResult result = {0};

    createTestNetwork(&graph);

    int failed[] = {0, 1};

    assert(analyzeAutomatedRestoration(
        &graph, failed, 2, &result
    ) == 1);

    assert(result.failedLineCount == 2);

    assert(fabs(
        result.afterFailure.totalDelivered
    ) < EPSILON);

    printf("[PASS] Multiple failures\n");
}

static void testInvalidFailures(void) {
    Graph graph = {0};
    AutomatedRestorationResult result = {0};

    createTestNetwork(&graph);

    int duplicate[] = {0, 0};
    int invalid[] = {99};
    int negative[] = {-1};

    assert(!analyzeAutomatedRestoration(
        &graph, duplicate, 2, &result
    ));

    assert(!analyzeAutomatedRestoration(
        &graph, invalid, 1, &result
    ));

    assert(!analyzeAutomatedRestoration(
        &graph, negative, 1, &result
    ));

    assert(!analyzeAutomatedRestoration(
        &graph, NULL, 1, &result
    ));

    graph.edges[0].active = 0;

    int inactive[] = {0};

    assert(!analyzeAutomatedRestoration(
        &graph, inactive, 1, &result
    ));

    printf("[PASS] Invalid failure requests\n");
}

static void testGraphPreservation(void) {
    Graph graph = {0};
    AutomatedRestorationResult result = {0};

    createTestNetwork(&graph);

    Graph original = graph;

    int failed[] = {0, 1};

    assert(analyzeAutomatedRestoration(
        &graph, failed, 2, &result
    ) == 1);

    assert(graph.nodeCount == original.nodeCount);
    assert(graph.edgeCount == original.edgeCount);

    for (int i = 0; i < graph.edgeCount; i++) {
        assert(graph.edges[i].active ==
               original.edges[i].active);

        assert(graph.edges[i].from ==
               original.edges[i].from);

        assert(graph.edges[i].to ==
               original.edges[i].to);
    }

    printf("[PASS] Original graph preservation\n");
}

static void testAllocationInvariants(void) {
    Graph graph = {0};
    AutomatedRestorationResult result = {0};

    createTestNetwork(&graph);

    int failed[] = {1};

    assert(analyzeAutomatedRestoration(
        &graph, failed, 1, &result
    ) == 1);

    const PriorityFlowResult *before =
        &result.beforeFailure;

    const PriorityFlowResult *after =
        &result.afterFailure;

    assert(before->totalDelivered >= -EPSILON);
    assert(after->totalDelivered >= -EPSILON);

    assert(before->totalDelivered <=
           before->totalSupply + EPSILON);

    assert(after->totalDelivered <=
           after->totalSupply + EPSILON);

    assert(before->totalDelivered <=
           before->totalDemand + EPSILON);

    assert(after->totalDelivered <=
           after->totalDemand + EPSILON);

    assert(fabs(
        after->totalDelivered +
        after->totalUnserved -
        after->totalDemand
    ) < EPSILON);

    double priorityTotal = 0.0;

    for (int p = 0; p < PRIORITY_LEVELS; p++) {
        priorityTotal += after->deliveredByPriority[p];
    }

    assert(fabs(
        priorityTotal - after->totalDelivered
    ) < EPSILON);

    printf("[PASS] Power allocation invariants\n");
}

int main(void) {
    testNoFailure();
    testSingleFailure();
    testMultipleFailures();
    testInvalidFailures();
    testGraphPreservation();
    testAllocationInvariants();

    printf("\nAll automated restoration tests passed!\n");

    return 0;
}
