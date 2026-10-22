
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

    if (!validateNodeData(&node)) {
        printf("Error: Invalid node data.\n");
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


    if (!validateEdgeData(&edge)) {
        printf("Error: Invalid transmission line.\n");
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


static int findNodeIndex(const Graph *graph, int nodeId) {

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


void initializeAdjList(AdjList *list) {

    if (list == NULL) {
        return;
    }

    list->entryCount = 0;

    for (int i = 0; i < MAX_NODES; i++) {
        list->head[i] = -1;
    }
}



int buildAdjacencyList(
    const Graph *graph,
    AdjList *list
) {

    if (graph == NULL || list == NULL) {
        return 0;
    }

    // Step 1: Initialize adjacency list
    initializeAdjList(list);

    // Step 2: Traverse all edges
    for (int i = 0; i < graph->edgeCount; i++) {

        const Edge *edge = &graph->edges[i];

        // Ignore inactive transmission lines
        if (edge->active == 0) {
            continue;
        }

        // Step 3: Find node array indices
        int fromIndex = findNodeIndex(
            graph, edge->from
        );

        int toIndex = findNodeIndex(
            graph, edge->to
        );

        if (fromIndex == -1 || toIndex == -1) {
            return 0;
        }

        // Check adjacency entry capacity
        if (list->entryCount + 2 > 2 * MAX_EDGES) {
            return 0;
        }

        // Step 4: Add from -> to
        int index = list->entryCount++;

        list->entries[index].neighborIndex = toIndex;
        list->entries[index].edgeIndex = i;
        list->entries[index].next = list->head[fromIndex];

        list->head[fromIndex] = index;

        // Step 5: Add to -> from
        index = list->entryCount++;

        list->entries[index].neighborIndex = fromIndex;
        list->entries[index].edgeIndex = i;
        list->entries[index].next = list->head[toIndex];

        list->head[toIndex] = index;
    }

    return 1;
}



void displayAdjacencyList(
    const Graph *graph,
    const AdjList *list
) {

    if (graph == NULL || list == NULL) {
        printf("Error: Invalid graph or adjacency list.\n");
        return;
    }

    printf("\n===== SMART GRID ADJACENCY LIST =====\n");

    for (int i = 0; i < graph->nodeCount; i++) {

        printf("%d (%s) -> ",
               graph->nodes[i].id,
               graph->nodes[i].name);

        int current = list->head[i];

        if (current == -1) {
            printf("No active connections");
        }

        while (current != -1) {

            int neighborIndex =
                list->entries[current].neighborIndex;

            printf("%d",
                   graph->nodes[neighborIndex].id);

            current = list->entries[current].next;

            if (current != -1) {
                printf(" -> ");
            }
        }

        printf("\n");
    }
}


void displayNetworkConnections(
    const Graph *graph,
    const AdjList *list
) {

    if (graph == NULL || list == NULL) {
        printf("Error: Invalid graph or list.\n");
        return;
    }

    printf("\n========== SMART GRID CONNECTIONS ==========\n");

    for (int i = 0; i < graph->nodeCount; i++) {

        printf("\n%s (ID %d)\n",
               graph->nodes[i].name,
               graph->nodes[i].id);

        int current = list->head[i];

        if (current == -1) {
            printf("  No active connections.\n");
        }

        while (current != -1) {

            AdjEntry entry = list->entries[current];

            const Node *neighbor =
                &graph->nodes[entry.neighborIndex];

            const Edge *edge =
                &graph->edges[entry.edgeIndex];

            printf("  -> %-20s (ID %d) | "
                   "%.2f km | %.2f MW | Cost %.2f\n",
                   neighbor->name,
                   neighbor->id,
                   edge->distance,
                   edge->capacity,
                   edge->cost);

            current = entry.next;
        }
    }
}


int areDirectlyConnected(
    const Graph *graph,
    const AdjList *list,
    int fromId,
    int toId
) {

    if (graph == NULL || list == NULL) {
        return 0;
    }

    int fromIndex = findNodeIndex(graph, fromId);
    int toIndex = findNodeIndex(graph, toId);

    if (fromIndex == -1 || toIndex == -1) {
        return 0;
    }

    int current = list->head[fromIndex];

    while (current != -1) {

        if (list->entries[current].neighborIndex == toIndex) {
            return 1;
        }

        current = list->entries[current].next;
    }

    return 0;
}


int getActiveDegree(
    const Graph *graph,
    const AdjList *list,
    int nodeId
) {

    if (graph == NULL || list == NULL) {
        return -1;
    }

    int nodeIndex = findNodeIndex(graph, nodeId);

    if (nodeIndex == -1) {
        return -1;
    }

    int degree = 0;
    int current = list->head[nodeIndex];

    while (current != -1) {

        degree++;

        current = list->entries[current].next;
    }

    return degree;
}



void displayIsolatedNodes(
    const Graph *graph,
    const AdjList *list
) {

    if (graph == NULL || list == NULL) {
        printf("Error: Invalid graph or list.\n");
        return;
    }

    printf("\n===== ISOLATED GRID COMPONENTS =====\n");

    int isolatedCount = 0;

    for (int i = 0; i < graph->nodeCount; i++) {

        int nodeId = graph->nodes[i].id;

        int degree = getActiveDegree(
            graph, list, nodeId
        );

        if (degree == 0) {

            printf("ID %d: %s\n",
                   nodeId,
                   graph->nodes[i].name);

            isolatedCount++;
        }
    }

    if (isolatedCount == 0) {
        printf("No isolated nodes found.\n");
    }

    printf("Total Isolated Nodes: %d\n", isolatedCount);
}



int validateAdjacencyList(
    const Graph *graph,
    const AdjList *list
) {

    if (graph == NULL || list == NULL) {
        return 0;
    }

    if (graph->nodeCount < 0 ||
        graph->nodeCount > MAX_NODES ||
        graph->edgeCount < 0 ||
        graph->edgeCount > MAX_EDGES ||
        list->entryCount < 0 ||
        list->entryCount > 2 * MAX_EDGES) {
        return 0;
    }

    // Count expected adjacency entries
    int activeEdges = 0;

    for (int i = 0; i < graph->edgeCount; i++) {
        if (graph->edges[i].active == 1) {
            activeEdges++;
        }
    }

    if (list->entryCount != 2 * activeEdges) {
        return 0;
    }

    // Track every adjacency entry exactly once
    int visited[2 * MAX_EDGES] = {0};

    for (int i = 0; i < graph->nodeCount; i++) {

        int current = list->head[i];

        while (current != -1) {

            // Validate entry index
            if (current < 0 ||
                current >= list->entryCount) {
                return 0;
            }

            // Detect repeated entries or cycles
            if (visited[current]) {
                return 0;
            }

            visited[current] = 1;

            AdjEntry entry = list->entries[current];

            // Validate neighbor and edge indices
            if (entry.neighborIndex < 0 ||
                entry.neighborIndex >= graph->nodeCount ||
                entry.edgeIndex < 0 ||
                entry.edgeIndex >= graph->edgeCount) {
                return 0;
            }

            const Edge *edge =
                &graph->edges[entry.edgeIndex];

            // Only active edges should be represented
            if (edge->active != 1) {
                return 0;
            }

            int fromId = graph->nodes[i].id;
            int toId =
                graph->nodes[entry.neighborIndex].id;

            // Check whether entry matches the edge
            if (!((edge->from == fromId &&
                   edge->to == toId) ||
                  (edge->from == toId &&
                   edge->to == fromId))) {
                return 0;
            }

            current = entry.next;
        }
    }

    // Check that all entries were reachable
    for (int i = 0; i < list->entryCount; i++) {
        if (!visited[i]) {
            return 0;
        }
    }

    // Verify exactly one entry in each direction
    for (int e = 0; e < graph->edgeCount; e++) {

        if (graph->edges[e].active != 1) {
            continue;
        }

        int fromIndex =
            findNodeIndex(graph, graph->edges[e].from);

        int toIndex =
            findNodeIndex(graph, graph->edges[e].to);

        if (fromIndex == -1 || toIndex == -1) {
            return 0;
        }

        int forward = 0;
        int reverse = 0;

        int current = list->head[fromIndex];

        while (current != -1) {
            AdjEntry entry = list->entries[current];

            if (entry.edgeIndex == e &&
                entry.neighborIndex == toIndex) {
                forward++;
            }

            current = entry.next;
        }

        current = list->head[toIndex];

        while (current != -1) {
            AdjEntry entry = list->entries[current];

            if (entry.edgeIndex == e &&
                entry.neighborIndex == fromIndex) {
                reverse++;
            }

            current = entry.next;
        }

        if (forward != 1 || reverse != 1) {
            return 0;
        }
    }

    return 1;

}


#include <math.h>

int validateNodeData(const Node *node) {

    if (node == NULL) {
        return 0;
    }

    if (node->id < 0) {
        return 0;
    }

    if (node->name[0] == '\0') {
        return 0;
    }

    if (!isfinite(node->demand) ||
        !isfinite(node->supply)) {
        return 0;
    }

    if (node->demand < 0 || node->supply < 0) {
        return 0;
    }

    if (node->priority < 1 || node->priority > 4) {
        return 0;
    }

    switch (node->type) {

        case GENERATOR:
            return node->supply > 0 &&
                   node->demand == 0;

        case SUBSTATION:
            return node->supply == 0 &&
                   node->demand == 0;

        case CONSUMER:
            return node->demand > 0 &&
                   node->supply == 0;

        default:
            return 0;
    }
}


void findGeneratorReachability(
    const Graph *graph,
    const AdjList *list,
    int reachable[MAX_NODES]
) {

    if (reachable == NULL) {
        return;
    }

    for (int i = 0; i < MAX_NODES; i++) {
        reachable[i] = 0;
    }

    if (graph == NULL || list == NULL) {
        return;
    }

    int stack[MAX_NODES];
    int top = -1;

    // Start from all generators
    for (int i = 0; i < graph->nodeCount; i++) {

        if (graph->nodes[i].type == GENERATOR &&
            graph->nodes[i].supply > 0) {

            reachable[i] = 1;
            stack[++top] = i;
        }
    }

    while (top >= 0) {

        int currentNode = stack[top--];

        int entryIndex = list->head[currentNode];

        while (entryIndex != -1) {

            int neighbor =
                list->entries[entryIndex].neighborIndex;

            if (!reachable[neighbor]) {

                reachable[neighbor] = 1;
                stack[++top] = neighbor;
            }

            entryIndex =
                list->entries[entryIndex].next;
        }
    }
}



void displayDisconnectedLoads(
    const Graph *graph,
    const AdjList *list
) {

    if (graph == NULL || list == NULL) {
        printf("Error: Invalid graph or list.\n");
        return;
    }

    int reachable[MAX_NODES];

    findGeneratorReachability(graph, list, reachable);

    double disconnectedDemand = 0.0;
    int disconnectedConsumers = 0;

    printf("\n===== DISCONNECTED CONSUMERS =====\n");

    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type == CONSUMER &&
            !reachable[i]) {

            printf("ID %d: %-22s Demand: %.2f MW\n",
                   node->id,
                   node->name,
                   node->demand);

            disconnectedDemand += node->demand;
            disconnectedConsumers++;
        }
    }

    if (disconnectedConsumers == 0) {
        printf("All consumers are reachable.\n");
    }

    printf("Disconnected Consumers: %d\n",
           disconnectedConsumers);

    printf("Disconnected Demand: %.2f MW\n",
           disconnectedDemand);
}


int validateEdgeData(const Edge *edge) {

    if (edge == NULL) {
        return 0;
    }

    if (edge->from < 0 || edge->to < 0) {
        return 0;
    }

    if (edge->from == edge->to) {
        return 0;
    }

    if (!isfinite(edge->distance) ||
        !isfinite(edge->capacity) ||
        !isfinite(edge->cost)) {
        return 0;
    }

    if (edge->distance <= 0 ||
        edge->capacity <= 0 ||
        edge->cost < 0) {
        return 0;
    }

    if (edge->active != 0 && edge->active != 1) {
        return 0;
    }

    return 1;
}



int calculateNetworkStats(
    const Graph *graph,
    const AdjList *list,
    NetworkStats *stats
) {

    if (graph == NULL || list == NULL || stats == NULL) {
        return 0;
    }

    // Verify that adjacency list matches the graph
    if (!validateAdjacencyList(graph, list)) {
        return 0;
    }

    // Initialize statistics
    *stats = (NetworkStats){0};

    stats->totalNodes = graph->nodeCount;
    stats->totalEdges = graph->edgeCount;

    // Step 1: Process nodes
    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        stats->totalSupply += node->supply;
        stats->totalDemand += node->demand;

        switch (node->type) {

            case GENERATOR:
                stats->generators++;
                break;

            case SUBSTATION:
                stats->substations++;
                break;

            case CONSUMER:
                stats->consumers++;
                break;

            default:
                return 0;
        }
    }

    // Step 2: Process transmission lines
    for (int i = 0; i < graph->edgeCount; i++) {

        const Edge *edge = &graph->edges[i];

        if (edge->active == 1) {

            stats->activeLines++;
            stats->totalActiveCapacity += edge->capacity;
            stats->totalActiveCost += edge->cost;

        } else {
            stats->failedLines++;
        }
    }

    // Step 3: Find generator reachability
    int reachable[MAX_NODES];

    findGeneratorReachability(graph, list, reachable);

    // Step 4: Calculate reachable/disconnected demand
    for (int i = 0; i < graph->nodeCount; i++) {

        const Node *node = &graph->nodes[i];

        if (node->type != CONSUMER) {
            continue;
        }

        if (reachable[i]) {

            stats->reachableDemand += node->demand;

        } else {

            stats->disconnectedDemand += node->demand;
            stats->disconnectedConsumers++;

            if (node->priority == 1) {
                stats->criticalDisconnected++;
            }
        }
    }

    return 1;
}



void displayNetworkReport(const NetworkStats *stats) {

    if (stats == NULL) {
        printf("Error: Invalid statistics.\n");
        return;
    }

    printf("\n========================================\n");
    printf("        SMART GRID NETWORK REPORT\n");
    printf("========================================\n");

    printf("\n--- Network Components ---\n");

    printf("Total Nodes          : %d\n", stats->totalNodes);
    printf("Generators           : %d\n", stats->generators);
    printf("Substations          : %d\n", stats->substations);
    printf("Consumers            : %d\n", stats->consumers);

    printf("\n--- Transmission Lines ---\n");

    printf("Total Lines          : %d\n", stats->totalEdges);
    printf("Active Lines         : %d\n", stats->activeLines);
    printf("Failed Lines         : %d\n", stats->failedLines);

    printf("\n--- Power Statistics ---\n");

    printf("Generation Supply    : %.2f MW\n",
           stats->totalSupply);

    printf("Total Demand         : %.2f MW\n",
           stats->totalDemand);

    printf("Supply - Demand      : %.2f MW\n",
           stats->totalSupply - stats->totalDemand);

    printf("Reachable Demand     : %.2f MW\n",
           stats->reachableDemand);

    printf("Disconnected Demand  : %.2f MW\n",
           stats->disconnectedDemand);

    printf("\n--- Failure Impact ---\n");

    printf("Disconnected Loads   : %d\n",
           stats->disconnectedConsumers);

    printf("Critical Disconnected: %d\n",
           stats->criticalDisconnected);

    printf("\n--- Line Statistics ---\n");

    printf("Sum Active Capacity  : %.2f MW\n",
           stats->totalActiveCapacity);

    printf("Sum Active Cost      : %.2f\n",
           stats->totalActiveCost);

    printf("\nNote: Reachability is not power allocation.\n");
    printf("========================================\n");
}


