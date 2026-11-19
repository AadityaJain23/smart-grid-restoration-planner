
#include <assert.h>
#include <stdio.h>

/*
 * Temporary white-box test.
 *
 * Include the implementation directly so
 * static helper functions can be tested.
 *
 * Do not also compile priority_flow.c
 * separately for this test.
 */
#include "../src/priority_flow.c"

int main(void)
{
    PriorityCost zero = zeroPriorityCost();

    assert(zero.component[0] == 0.0);
    assert(zero.component[1] == 0.0);
    assert(zero.component[2] == 0.0);

    PriorityCost p1 = makeConsumerCost(1);
    PriorityCost p2 = makeConsumerCost(2);
    PriorityCost p3 = makeConsumerCost(3);

    assert(p1.component[0] == -1.0);
    assert(p2.component[1] == -1.0);
    assert(p3.component[2] == -1.0);

    assert(comparePriorityCost(p1, p2) < 0);
    assert(comparePriorityCost(p2, p3) < 0);

    PriorityCost combined =
        addPriorityCost(p1, p2);

    assert(combined.component[0] == -1.0);
    assert(combined.component[1] == -1.0);

    PriorityCost reversed =
        negatePriorityCost(p1);

    assert(reversed.component[0] == 1.0);

    PriorityCost a = {{-20.0, -10.0, -30.0}};
    PriorityCost b = {{-20.0, 0.0, -40.0}};

    assert(comparePriorityCost(a, b) < 0);
    assert(comparePriorityCost(b, a) > 0);
    assert(comparePriorityCost(a, a) == 0);

    printf("[PASS] Zero priority cost\n");
    printf("[PASS] Consumer priority costs\n");
    printf("[PASS] Lexicographic comparison\n");
    printf("[PASS] Cost addition\n");
    printf("[PASS] Reverse cost negation\n");
    printf("[PASS] Allocation comparison\n");


/*
 * Verify existing priority network helpers.
 */
Graph graph = {0};
FlowNetwork network;

graph.nodeCount = 2;
graph.edgeCount = 1;

/* Generator */
graph.nodes[0].id = 10;
graph.nodes[0].type = GENERATOR;
graph.nodes[0].supply = 60.0;

/* Hospital */
graph.nodes[1].id = 20;
graph.nodes[1].type = CONSUMER;
graph.nodes[1].demand = 20.0;
graph.nodes[1].priority = 1;

/* Transmission line */
graph.edges[0].from = 10;
graph.edges[0].to = 20;
graph.edges[0].capacity = 30.0;
graph.edges[0].active = 1;

/* Test initialization */
assert(initializePriorityNetwork(
    &graph, &network) == 1);

int sink = network.sinkIndex;

assert(network.residual[1][sink] == 0.0);

/* Test priority activation */
assert(activatePriorityClass(
    &graph, &network, 1) == 1);

assert(network.residual[1][sink] == 20.0);

printf("[PASS] Priority network initialization\n");
printf("[PASS] Priority class activation\n");


    printf("\nAll priority cost tests passed!\n");

    return 0;
}
