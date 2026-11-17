
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>

#include "maxflow.h"

static int testsPassed = 0;

static void addTestNode(
    Graph *g,
    int id,
    const char *name,
    int type,
    double demand,
    double supply,
    int priority
)
{
    assert(g->nodeCount < MAX_NODES);

    Node *node = &g->nodes[g->nodeCount++];

    memset(node, 0, sizeof(*node));

    node->id = id;

    snprintf(node->name, sizeof(node->name),
             "%s", name);

    node->type = type;
    node->demand = demand;
    node->supply = supply;
    node->priority = priority;
}

static void addLine(
    Graph *g,
    int from,
    int to,
    double capacity,
    int active
)
{
    assert(g->edgeCount < MAX_EDGES);

    Edge *edge = &g->edges[g->edgeCount++];

    memset(edge, 0, sizeof(*edge));

    edge->from = from;
    edge->to = to;
    edge->capacity = capacity;
    edge->active = active;
}

static void checkClose(double actual, double expected)
{
    assert(fabs(actual - expected) < 1e-6);
}

/* Test 1: Simple network */
static void testSimpleNetwork(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 10, "Generator", 0, 0, 40, 0);
    addTestNode(&g, 20, "Sub_A", 1, 0, 0, 0);
    addTestNode(&g, 30, "Sub_B", 1, 0, 0, 0);
    addTestNode(&g, 40, "Consumer", 2, 40, 0, 1);

    addLine(&g, 10, 20, 20, 1);
    addLine(&g, 10, 30, 15, 1);
    addLine(&g, 20, 40, 10, 1);
    addLine(&g, 30, 40, 15, 1);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 25);
    checkClose(result.totalUnserved, 15);
    checkClose(result.deliveredToNode[3], 25);

    printf("[PASS] Simple network - 25 MW\n");
    testsPassed++;
}

/* Test 2: Transmission bottleneck */
static void testBottleneck(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 10, "Generator", 0, 0, 100, 0);
    addTestNode(&g, 20, "Substation", 1, 0, 0, 0);
    addTestNode(&g, 30, "Consumer", 2, 100, 0, 1);

    addLine(&g, 10, 20, 30, 1);
    addLine(&g, 20, 30, 80, 1);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 30);

    printf("[PASS] Transmission bottleneck - 30 MW\n");
    testsPassed++;
}

/* Test 3: Failed line */
static void testFailedLine(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 10, "Generator", 0, 0, 50, 0);
    addTestNode(&g, 20, "Consumer", 2, 50, 0, 1);

    addLine(&g, 10, 20, 50, 0);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 0);
    checkClose(result.totalUnserved, 50);

    printf("[PASS] Failed transmission line\n");
    testsPassed++;
}

/* Test 4: Multiple generators */
static void testMultipleGenerators(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 10, "Generator_A", 0, 0, 20, 0);
    addTestNode(&g, 20, "Generator_B", 0, 0, 30, 0);
    addTestNode(&g, 30, "Consumer", 2, 60, 0, 1);

    addLine(&g, 10, 30, 20, 1);
    addLine(&g, 20, 30, 30, 1);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 50);
    checkClose(result.totalUnserved, 10);

    printf("[PASS] Multiple generators - 50 MW\n");
    testsPassed++;
}

/* Test 5: Noncontiguous node IDs */
static void testNoncontiguousIds(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 100, "Generator", 0, 0, 25, 0);
    addTestNode(&g, 900, "Consumer", 2, 25, 0, 1);

    addLine(&g, 100, 900, 25, 1);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 25);

    printf("[PASS] Noncontiguous node IDs\n");
    testsPassed++;
}

int main(void)
{
    testSimpleNetwork();
    testBottleneck();
    testFailedLine();
    testMultipleGenerators();
    testNoncontiguousIds();

    printf("\n========================================\n");
    printf("MAXIMUM FLOW TEST SUMMARY\n");
    printf("========================================\n");
    printf("Tests passed: %d/5\n", testsPassed);

    return 0;
}

