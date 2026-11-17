
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include <float.h>

#include "maxflow.h"

static int testsPassed = 0;

static void checkClose(double actual, double expected)
{
    assert(isfinite(actual));
    assert(fabs(actual - expected) < 1e-6);
}

static void pass(const char *name)
{
    testsPassed++;
    printf("[PASS] %s\n", name);
}

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
    assert(g != NULL);
    assert(g->nodeCount < MAX_NODES);

    Node *n = &g->nodes[g->nodeCount++];

    memset(n, 0, sizeof(*n));

    n->id = id;
    snprintf(n->name, sizeof(n->name), "%s", name);
    n->type = type;
    n->demand = demand;
    n->supply = supply;
    n->priority = priority;
}

static void addTestLine(
    Graph *g,
    int from,
    int to,
    double capacity,
    int active
)
{
    assert(g != NULL);
    assert(g->edgeCount < MAX_EDGES);

    Edge *e = &g->edges[g->edgeCount++];

    memset(e, 0, sizeof(*e));

    e->from = from;
    e->to = to;
    e->capacity = capacity;
    e->active = active;
}

/* Change a transmission line's status in test data. */
static void setTestLineStatus(
    Graph *g,
    int from,
    int to,
    int active
)
{
    for (int i = 0; i < g->edgeCount; i++) {
        Edge *e = &g->edges[i];

        if ((e->from == from && e->to == to) ||
            (e->from == to && e->to == from)) {
            e->active = active;
            return;
        }
    }

    assert(0 && "Test transmission line not found");
}

/*
 * Validate reported consumer allocations.
 * These are necessary consistency conditions.
 */
static void checkAllocation(
    const Graph *g,
    const MaxFlowResult *r
)
{
    double delivered = 0.0;

    for (int i = 0; i < g->nodeCount; i++) {
        if (g->nodes[i].type != 2) {
            continue;
        }

        double value = r->deliveredToNode[i];

        assert(isfinite(value));
        assert(value >= -1e-6);
        assert(value <= g->nodes[i].demand + 1e-6);

        delivered += value;
    }

    checkClose(delivered, r->maxFlow);
    checkClose(r->totalDemand - delivered,
               r->totalUnserved);

    assert(r->maxFlow <= r->totalSupply + 1e-6);
    assert(r->maxFlow <= r->totalDemand + 1e-6);
}


static void createOriginalGrid(Graph *g)
{
    memset(g, 0, sizeof(*g));

    addTestNode(g, 0, "Main_Power_Station",
                0, 0, 100, 0);

    addTestNode(g, 1, "Substation_A",
                1, 0, 0, 0);

    addTestNode(g, 2, "Substation_B",
                1, 0, 0, 0);

    addTestNode(g, 3, "Substation_C",
                1, 0, 0, 0);

    addTestNode(g, 4, "Hospital",
                2, 20, 0, 1);

    addTestNode(g, 5, "Residential",
                2, 25, 0, 3);

    addTestNode(g, 6, "Industrial",
                2, 30, 0, 3);

    addTestNode(g, 7, "School",
                2, 10, 0, 2);

    addTestNode(g, 8, "Market",
                2, 15, 0, 3);

    addTestLine(g, 0, 1, 60, 1);
    addTestLine(g, 0, 2, 50, 1);
    addTestLine(g, 0, 3, 50, 1);

    addTestLine(g, 1, 4, 30, 1);
    addTestLine(g, 1, 5, 40, 1);
    addTestLine(g, 2, 6, 40, 1);
    addTestLine(g, 3, 7, 20, 1);
    addTestLine(g, 3, 8, 25, 1);
}

static void createRedundantGrid(Graph *g)
{
    createOriginalGrid(g);

    addTestLine(g, 2, 4, 30, 1);
    addTestLine(g, 1, 2, 40, 1);
    addTestLine(g, 2, 3, 40, 1);
}


static void testOriginalGrid(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    createOriginalGrid(&g);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.totalSupply, 100);
    checkClose(result.totalDemand, 100);
    checkClose(result.maxFlow, 100);
    checkClose(result.totalUnserved, 0);

    checkAllocation(&g, &result);

    pass("Original grid - 100 MW");
}



static void testHospitalLineFailure(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    createOriginalGrid(&g);

    setTestLineStatus(&g, 1, 4, 0);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 80);
    checkClose(result.totalUnserved, 20);

    /* Hospital is graph-array index 4. */
    checkClose(result.deliveredToNode[4], 0);

    checkAllocation(&g, &result);

    pass("Hospital line failure - 80 MW");
}


static void testRedundantNetwork(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    createRedundantGrid(&g);

    setTestLineStatus(&g, 1, 4, 0);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 100);
    checkClose(result.totalUnserved, 0);

    checkClose(result.deliveredToNode[4], 20);

    checkAllocation(&g, &result);

    pass("Redundant network restores Hospital");
}


static void testReducedSupply(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    createOriginalGrid(&g);

    g.nodes[0].supply = 30;

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.totalSupply, 30);
    checkClose(result.maxFlow, 30);
    checkClose(result.totalUnserved, 70);

    checkAllocation(&g, &result);

    pass("Reduced generator supply - 30 MW");
}


static void testMultipleGeneratorsBottleneck(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 10, "Generator_A",
                0, 0, 40, 0);

    addTestNode(&g, 20, "Generator_B",
                0, 0, 40, 0);

    addTestNode(&g, 30, "Substation",
                1, 0, 0, 0);

    addTestNode(&g, 40, "Consumer",
                2, 80, 0, 1);

    addTestLine(&g, 10, 30, 40, 1);
    addTestLine(&g, 20, 30, 40, 1);
    addTestLine(&g, 30, 40, 50, 1);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.totalSupply, 80);
    checkClose(result.maxFlow, 50);
    checkClose(result.totalUnserved, 30);

    checkAllocation(&g, &result);

    pass("Multiple generators - shared bottleneck");
}


static void testParallelLines(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 10, "Generator",
                0, 0, 50, 0);

    addTestNode(&g, 20, "Consumer",
                2, 50, 0, 1);

    addTestLine(&g, 10, 20, 20, 1);
    addTestLine(&g, 10, 20, 15, 1);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 35);

    checkAllocation(&g, &result);

    pass("Parallel transmission lines - 35 MW");
}

static void testFractionalCapacity(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 10, "Generator",
                0, 0, 12.5, 0);

    addTestNode(&g, 20, "Consumer",
                2, 12.5, 0, 1);

    addTestLine(&g, 10, 20, 7.5, 1);

    assert(calculateMaximumFlow(&g, &result) == 1);

    checkClose(result.maxFlow, 7.5);
    checkClose(result.totalUnserved, 5.0);

    checkAllocation(&g, &result);

    pass("Fractional capacity - 7.5 MW");
}


static void testReverseResidualRerouting(void)
{
    FlowNetwork network = {0};

    /*
     * Vertex mapping:
     * 0 = S
     * 1 = A
     * 2 = B
     * 3 = C
     * 4 = D
     * 5 = T
     */
    network.vertexCount = 6;
    network.sourceIndex = 0;
    network.sinkIndex = 5;

    network.residual[0][1] = 1;
    network.residual[0][2] = 1;

    network.residual[1][3] = 1;
    network.residual[1][4] = 1;
    network.residual[2][3] = 1;

    network.residual[3][5] = 1;
    network.residual[4][5] = 1;

    double maximumFlow = 0.0;
    int paths = 0;

    assert(edmondsKarp(
        &network,
        &maximumFlow,
        &paths
    ) == 1);

    checkClose(maximumFlow, 2.0);
    assert(paths == 2);

    /* The initial A->C flow was canceled. */
    checkClose(network.residual[1][3], 1.0);
    checkClose(network.residual[3][1], 0.0);

    pass("Reverse residual rerouting - 2 units");
}


static void testInvalidInputs(void)
{
    Graph g = {0};
    MaxFlowResult result = {0};

    addTestNode(&g, 10, "Generator",
                0, 0, 50, 0);

    addTestNode(&g, 20, "Consumer",
                2, 50, 0, 1);

    addTestLine(&g, 10, 20, -5, 1);

    /* Negative transmission capacity */
    assert(calculateMaximumFlow(&g, &result) == 0);

    g.edges[0].capacity = NAN;

    /* Non-finite transmission capacity */
    assert(calculateMaximumFlow(&g, &result) == 0);

    g.edges[0].capacity = 20;
    g.edges[0].to = 999;

    /* Unknown endpoint ID */
    assert(calculateMaximumFlow(&g, &result) == 0);

    g.edges[0].to = 20;
    g.nodes[1].id = 10;

    /* Duplicate node IDs */
    assert(calculateMaximumFlow(&g, &result) == 0);

    /* Null input */
    assert(calculateMaximumFlow(NULL, &result) == 0);
    assert(calculateMaximumFlow(&g, NULL) == 0);

    pass("Invalid graph inputs rejected");
}



static void testGraphPreservation(void)
{
    Graph g = {0};
    Graph original;
    MaxFlowResult result = {0};

    createRedundantGrid(&g);

    original = g;

    assert(calculateMaximumFlow(&g, &result) == 1);

    /*
     * Both structures are initialized to zero
     * before nodes and edges are populated.
     */
    assert(memcmp(&g, &original, sizeof(g)) == 0);

    checkAllocation(&g, &result);

    pass("Original graph preserved");
}


int main(void)
{
    testOriginalGrid();
    testHospitalLineFailure();
    testRedundantNetwork();
    testReducedSupply();

    testMultipleGeneratorsBottleneck();
    testParallelLines();
    testFractionalCapacity();
    testReverseResidualRerouting();

    testInvalidInputs();
    testGraphPreservation();

    printf("\n========================================\n");
    printf("ADVANCED MAXIMUM FLOW TEST SUMMARY\n");
    printf("========================================\n");
    printf("Tests passed: %d/10\n", testsPassed);

    return 0;
}
