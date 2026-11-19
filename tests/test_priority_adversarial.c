
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "../src/priority_flow.c"

int main(void)
{
    Graph graph = {0};

    graph.nodeCount = 4;
    graph.edgeCount = 3;

    /* Generator */
    graph.nodes[0].id = 10;
    graph.nodes[0].type = GENERATOR;
    graph.nodes[0].supply = 20.0;

    /* Substation */
    graph.nodes[1].id = 20;
    graph.nodes[1].type = SUBSTATION;

    /* Hospital */
    graph.nodes[2].id = 30;
    graph.nodes[2].type = CONSUMER;
    graph.nodes[2].demand = 10.0;
    graph.nodes[2].priority = 1;

    /* Industry */
    graph.nodes[3].id = 40;
    graph.nodes[3].type = CONSUMER;
    graph.nodes[3].demand = 20.0;
    graph.nodes[3].priority = 3;

    /* Generator -> Substation */
    graph.edges[0].from = 10;
    graph.edges[0].to = 20;
    graph.edges[0].capacity = 20.0;
    graph.edges[0].active = 1;

    /* Substation -> Hospital */
    graph.edges[1].from = 20;
    graph.edges[1].to = 30;
    graph.edges[1].capacity = 10.0;
    graph.edges[1].active = 1;

    /* Substation -> Industry */
    graph.edges[2].from = 20;
    graph.edges[2].to = 40;
    graph.edges[2].capacity = 20.0;
    graph.edges[2].active = 1;

    PriorityNetwork network = {0};

    assert(buildPriorityNetwork(
        &graph, &network) == 1);

    printf("[PASS] Adversarial network built\n");

    double totalFlow = 0.0;
    PriorityCost totalCost =
        zeroPriorityCost();

    int paths = 0;

    assert(runPriorityFlowPrototype(
        &network,
        &totalFlow,
        &totalCost,
        &paths) == 1);

    assert(fabs(totalFlow - 20.0) < FLOW_EPS);

    printf("[PASS] Total delivery = 20 MW\n");

    double hospital = 0.0;
    double industry = 0.0;

    for (int e = 0; e < network.arcCount; e++) {

        const PriorityArc *arc =
            &network.arcs[e];

        if (arc->to != network.sinkIndex) {
            continue;
        }

        if (arc->from == 2) {
            hospital =
                graph.nodes[2].demand -
                arc->capacity;
        }

        if (arc->from == 3) {
            industry =
                graph.nodes[3].demand -
                arc->capacity;
        }
    }

    assert(fabs(hospital - 10.0) < FLOW_EPS);
    assert(fabs(industry - 10.0) < FLOW_EPS);

    printf("[PASS] Hospital receives 10 MW\n");
    printf("[PASS] Industry receives 10 MW\n");

    assert(fabs(
        totalCost.component[0] + 10.0
    ) < FLOW_EPS);

    assert(fabs(
        totalCost.component[2] + 10.0
    ) < FLOW_EPS);

    printf("[PASS] Lexicographic priority respected\n");

    printf("\nAll adversarial tests passed!\n");

    return 0;
}
