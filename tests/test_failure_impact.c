
#include <assert.h>
#include <math.h>
#include <stdio.h>

#include "failure_impact.h"

int main(void) {
    Graph graph = {0};
    FailureImpactResult impact = {0};

    initializeGraph(&graph);

    Node generator = {0};
    generator.id = 10;
    generator.type = GENERATOR;
    generator.supply = 20.0;
    generator.priority = 1;
    snprintf(generator.name, NAME_SIZE, "Generator");

    Node hospital = {0};
    hospital.id = 20;
    hospital.type = CONSUMER;
    hospital.demand = 10.0;
    hospital.priority = 1;
    snprintf(hospital.name, NAME_SIZE, "Hospital");

    assert(addNode(&graph, generator) == 1);
    assert(addNode(&graph, hospital) == 1);

    Edge line = {0};
    line.from = 10;
    line.to = 20;
    line.distance = 1.0;
    line.capacity = 20.0;
    line.cost = 1.0;
    line.active = 1;

    assert(addEdge(&graph, line) == 1);

    int failed[] = {0};

    assert(analyzeMultipleLineFailures(
        &graph, failed, 1, &impact
    ) == 1);

    assert(impact.failedLineCount == 1);
    assert(impact.disconnectedConsumers == 1);
    assert(impact.criticalDisconnected == 1);
    assert(fabs(impact.disconnectedDemand - 10.0) < 1e-6);
    assert(fabs(impact.reachableDemand) < 1e-6);

    printf("[PASS] Single-line failure impact\n");

    /* The original graph must remain active. */
    assert(graph.edges[0].active == 1);

    printf("[PASS] Original network preservation\n");

    /* Reject duplicate failure requests. */
    int duplicate[] = {0, 0};

    assert(analyzeMultipleLineFailures(
        &graph, duplicate, 2, &impact
    ) == 0);

    printf("[PASS] Duplicate failure rejection\n");

    /* Reject invalid line indices. */
    int invalid[] = {99};

    assert(analyzeMultipleLineFailures(
        &graph, invalid, 1, &impact
    ) == 0);

    printf("[PASS] Invalid line rejection\n");

    printf("\nAll failure impact tests passed!\n");

    return 0;
}
