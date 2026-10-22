
#include <stdio.h>
#include "graph.h"

void initializeGraph(Graph *graph) {
    if (graph == NULL) {
        return;
    }

    graph->nodeCount = 0;
    graph->edgeCount = 0;
}



int addNode(Graph *graph, Node node) {

    // Step 1: Check graph pointer
    if (graph == NULL) {
        return 0;
    }

    // Step 2: Check graph capacity
    if (graph->nodeCount >= MAX_NODES) {
        printf("Error: Graph is full.\n");
        return 0;
    }

    // Step 3: Validate node ID
    if (node.id < 0) {
        printf("Error: Invalid node ID.\n");
        return 0;
    }

    // Step 4: Validate node type
    if (node.type != GENERATOR &&
        node.type != SUBSTATION &&
        node.type != CONSUMER) {
        printf("Error: Invalid node type.\n");
        return 0;
    }

    // Step 5: Validate electricity values
    if (node.demand < 0 || node.supply < 0) {
        printf("Error: Negative demand or supply.\n");
        return 0;
    }

    if (node.priority < 1 || node.priority > 4) {
        printf("Error: Invalid priority.\n");
        return 0;
    }

    // Step 6: Check for duplicate IDs
    for (int i = 0; i < graph->nodeCount; i++) {

        if (graph->nodes[i].id == node.id) {
            printf("Error: Duplicate node ID %d.\n",
                   node.id);
            return 0;
        }
    }

    // Step 7: Insert node
    graph->nodes[graph->nodeCount] = node;

    // Step 8: Increase node count
    graph->nodeCount++;

    return 1;
}



int addEdge(Graph *graph, Edge edge) {

    if (graph == NULL) {
        return 0;
    }

    // Check edge capacity
    if (graph->edgeCount >= MAX_EDGES) {
        printf("Error: Edge limit reached.\n");
        return 0;
    }

    // Prevent self-loops
    if (edge.from == edge.to) {
        printf("Error: Self-loop not allowed.\n");
        return 0;
    }

    // Validate line properties
    if (edge.distance <= 0 ||
        edge.capacity <= 0 ||
        edge.cost < 0) {
        printf("Error: Invalid edge values.\n");
        return 0;
    }

    if (edge.active != 0 && edge.active != 1) {
        printf("Error: Invalid edge status.\n");
        return 0;
    }

    // Check that both endpoints exist
    int fromFound = 0;
    int toFound = 0;

    for (int i = 0; i < graph->nodeCount; i++) {

        if (graph->nodes[i].id == edge.from) {
            fromFound = 1;
        }

        if (graph->nodes[i].id == edge.to) {
            toFound = 1;
        }
    }

    if (!fromFound || !toFound) {
        printf("Error: Endpoint node not found.\n");
        return 0;
    }

    // Check for duplicate undirected edges
    for (int i = 0; i < graph->edgeCount; i++) {

        Edge existing = graph->edges[i];

        if ((existing.from == edge.from &&
             existing.to == edge.to) ||
            (existing.from == edge.to &&
             existing.to == edge.from)) {

            printf("Error: Duplicate edge.\n");
            return 0;
        }
    }

    // Insert edge
    graph->edges[graph->edgeCount] = edge;

    // Increase edge count
    graph->edgeCount++;

    return 1;
}



void displayNodes(const Graph *graph) {

    // Step 1: Validate graph pointer
    if (graph == NULL) {
        printf("Error: Invalid graph.\n");
        return;
    }

    // Step 2: Check whether graph is empty
    if (graph->nodeCount == 0) {
        printf("No nodes available.\n");
        return;
    }

    // Step 3: Print table heading
    printf("\n================ SMART GRID NODES ================\n");

    printf("%-5s %-22s %-12s %10s %10s %10s\n",
           "ID", "Name", "Type",
           "Demand", "Supply", "Priority");

    printf("--------------------------------------------------------------------------\n");

    // Step 4: Traverse all nodes
    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        const char *typeName;

        // Convert enum into readable text
        switch (node->type) {

            case GENERATOR:
                typeName = "Generator";
                break;

            case SUBSTATION:
                typeName = "Substation";
                break;

            case CONSUMER:
                typeName = "Consumer";
                break;

            default:
                typeName = "Unknown";
        }

        // Step 5: Print node information
        printf("%-5d %-22s %-12s %10.2f %10.2f %10d\n",
               node->id,
               node->name,
               typeName,
               node->demand,
               node->supply,
               node->priority);
    }

    printf("--------------------------------------------------------------------------\n");

    printf("Total Nodes: %d\n", graph->nodeCount);
}



void displayEdges(const Graph *graph) {

    // Step 1: Validate graph pointer
    if (graph == NULL) {
        printf("Error: Invalid graph.\n");
        return;
    }

    // Step 2: Check whether edges exist
    if (graph->edgeCount == 0) {
        printf("No edges available.\n");
        return;
    }

    // Step 3: Print heading
    printf("\n================ SMART GRID EDGES ================\n");

    printf("%-8s %-8s %12s %12s %10s %-10s\n",
           "From", "To", "Distance",
           "Capacity", "Cost", "Status");

    printf("-----------------------------------------------------------------\n");

    // Step 4: Traverse edges
    for (int i = 0; i < graph->edgeCount; i++) {

        const Edge *edge = &graph->edges[i];

        const char *status;

        // Determine line status
        if (edge->active == 1) {
            status = "Active";
        } else {
            status = "Inactive";
        }

        // Step 5: Print edge information
        printf("%-8d %-8d %12.2f %12.2f %10.2f %-10s\n",
               edge->from,
               edge->to,
               edge->distance,
               edge->capacity,
               edge->cost,
               status);
    }

    printf("-----------------------------------------------------------------\n");

    printf("Total Edges: %d\n", graph->edgeCount);
}



int loadNetworkFromFile(Graph *graph, const char *filename) {

    // Step 1: Validate input pointers
    if (graph == NULL || filename == NULL) {
        printf("Error: Invalid graph or filename.\n");
        return 0;
    }

    // Step 2: Open file
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        printf("Error: Cannot open file %s\n", filename);
        return 0;
    }

    // Step 3: Create temporary graph
    Graph temp;
    initializeGraph(&temp);

    int nodeCount;

    // Step 4: Read number of nodes
    if (fscanf(file, "%d", &nodeCount) != 1 ||
        nodeCount < 0 ||
        nodeCount > MAX_NODES) {

        printf("Error: Invalid node count.\n");
        fclose(file);
        return 0;
    }

    // Step 5: Read and insert nodes
    for (int i = 0; i < nodeCount; i++) {

        Node node;
        int type;

        if (fscanf(file, "%d %49s %d %lf %lf %d",
                   &node.id,
                   node.name,
                   &type,
                   &node.demand,
                   &node.supply,
                   &node.priority) != 6) {

            printf("Error: Invalid node record %d.\n", i + 1);
            fclose(file);
            return 0;
        }

        // Validate enum before converting
        if (type < GENERATOR || type > CONSUMER) {
            printf("Error: Invalid node type.\n");
            fclose(file);
            return 0;
        }

        node.type = (NodeType)type;

        // Reuse existing validation
        if (!addNode(&temp, node)) {
            printf("Error: Failed to add node %d.\n", node.id);
            fclose(file);
            return 0;
        }
    }

    int edgeCount;

    // Step 6: Read number of edges
    if (fscanf(file, "%d", &edgeCount) != 1 ||
        edgeCount < 0 ||
        edgeCount > MAX_EDGES) {

        printf("Error: Invalid edge count.\n");
        fclose(file);
        return 0;
    }

    // Step 7: Read and insert edges
    for (int i = 0; i < edgeCount; i++) {

        Edge edge;

        if (fscanf(file, "%d %d %lf %lf %lf %d",
                   &edge.from,
                   &edge.to,
                   &edge.distance,
                   &edge.capacity,
                   &edge.cost,
                   &edge.active) != 6) {

            printf("Error: Invalid edge record %d.\n", i + 1);
            fclose(file);
            return 0;
        }

        // Reuse existing edge validation
        if (!addEdge(&temp, edge)) {
            printf("Error: Failed to add edge %d.\n", i + 1);
            fclose(file);
            return 0;
        }
    }

    // Step 8: Reject unexpected extra data
    char extra[2];

    if (fscanf(file, " %1s", extra) == 1) {
        printf("Error: Unexpected data after edge records.\n");
        fclose(file);
        return 0;
    }

    // Step 9: Close file
    if (fclose(file) != 0) {
        printf("Error: Failed to close network file.\n");
        return 0;
    }

    // Step 10: Commit fully validated graph
    *graph = temp;

    printf("Network loaded successfully!\n");

    return 1;
}
