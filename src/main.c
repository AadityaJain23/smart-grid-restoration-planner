#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "graph.h"

/* Read a complete input line. Return 1 on success, 0 on EOF/error. */
static int readLine(const char *prompt, char *buffer, size_t size) {
    int ch;
    size_t length;

    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL) {
        return 0;
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    } else if (!feof(stdin)) {
        /* The line may be exactly buffer-sized: detect remaining input. */
        ch = getchar();
        if (ch != '\n' && ch != EOF) {
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
            printf("Error: Input is too long.\n");
            buffer[0] = '\0';
            return 1;
        }
    }
    return 1;
}

/* Return 1 only when the entire input is a valid int. */
static int readInt(const char *prompt, int *value) {
    char buffer[128];
    char *end;
    long parsed;

    if (value == NULL || !readLine(prompt, buffer, sizeof(buffer))) {
        return 0;
    }
    errno = 0;
    parsed = strtol(buffer, &end, 10);
    if (buffer == end || errno == ERANGE ||
        parsed < INT_MIN || parsed > INT_MAX) {
        printf("Error: Enter a valid integer.\n");
        return -1;
    }
    while (*end == ' ' || *end == '\t' || *end == '\r') {
        end++;
    }
    if (*end != '\0') {
        printf("Error: Enter only one integer.\n");
        return -1;
    }
    *value = (int)parsed;
    return 1;
}

static void displayMenu(void) {
    printf("\n========== SMART GRID PLANNER ==========\n");
    printf(" 1. Load network from file\n");
    printf(" 2. Display nodes\n");
    printf(" 3. Display transmission lines\n");
    printf(" 4. Display network connections\n");
    printf(" 5. Check direct connectivity\n");
    printf(" 6. Display isolated nodes\n");
    printf(" 7. Simulate line failure\n");
    printf(" 8. Restore transmission line\n");
    printf(" 9. Comprehensive network report\n");
    printf("10. Display adjacency list\n");
    printf("11. Check active degree\n");
    printf("12. Display disconnected loads\n");
    printf(" 0. Exit\n");
    printf("========================================\n");
}

// static void displayNetworkSummary(const Graph *graph) {
//     double supply = 0.0, demand = 0.0;
//     int active = 0;
//     int i;

//     for (i = 0; i < graph->nodeCount; i++) {
//         supply += graph->nodes[i].supply;
//         demand += graph->nodes[i].demand;
//     }
//     for (i = 0; i < graph->edgeCount; i++) {
//         if (graph->edges[i].active == 1) {
//             active++;
//         }
//     }
//     printf("\n========== NETWORK SUMMARY ==========\n");
//     printf("Nodes:             %d\n", graph->nodeCount);
//     printf("Transmission lines:%d\n", graph->edgeCount);
//     printf("Active lines:      %d\n", active);
//     printf("Inactive lines:    %d\n", graph->edgeCount - active);
//     printf("Generation supply: %.2f MW\n", supply);
//     printf("Load demand:       %.2f MW\n", demand);
//     printf("Supply - demand:   %.2f MW\n", supply - demand);
//     printf("Note: This is not a power-allocation calculation.\n");
// }

/* Change a line by endpoint IDs and keep the adjacency list synchronized. */
static int changeLineStatus(Graph *graph, AdjList *list,
                            int fromId, int toId, int newStatus) {
    int i;
    if (newStatus != 0 && newStatus != 1) {
        return 0;
    }
    for (i = 0; i < graph->edgeCount; i++) {
        Edge *edge = &graph->edges[i];
        if ((edge->from == fromId && edge->to == toId) ||
            (edge->from == toId && edge->to == fromId)) {
            int previousStatus = edge->active;
            AdjList updated;

            if (previousStatus == newStatus) {
                printf("Line %d-%d is already %s.\n", fromId, toId,
                       newStatus ? "active" : "inactive");
                return 1;
            }
            edge->active = newStatus;
            if (!buildAdjacencyList(graph, &updated) ||
                !validateAdjacencyList(graph, &updated)) {
                edge->active = previousStatus;
                printf("Error: Line change rejected; network preserved.\n");
                return 0;
            }
            *list = updated;
            printf("Line %d-%d is now %s.\n", fromId, toId,
                   newStatus ? "ACTIVE" : "INACTIVE");
            return 1;
        }
    }
    printf("Error: Transmission line %d-%d not found.\n", fromId, toId);
    return 0;
}

/* Keep the existing network if a new file fails to load or validate. */
static int loadNetworkSafely(Graph *graph, AdjList *list,
                             const char *filename) {
    Graph candidate;
    AdjList candidateList;

    initializeGraph(&candidate);
    if (!loadNetworkFromFile(&candidate, filename)) {
        printf("Load failed; previous network preserved.\n");
        return 0;
    }
    if (!buildAdjacencyList(&candidate, &candidateList) ||
        !validateAdjacencyList(&candidate, &candidateList)) {
        printf("Error: Invalid connectivity; previous network preserved.\n");
        return 0;
    }
    *graph = candidate;
    *list = candidateList;
    printf("Network is ready.\n");
    return 1;
}

int main(void) {
    Graph grid;
    AdjList list;
    int loaded = 0;
    int choice, fromId, toId, result;
    char filename[256];

    initializeGraph(&grid);
    initializeAdjList(&list);
    printf("Welcome to the Smart Grid Distribution Planner!\n");

    for (;;) {
        displayMenu();
        result = readInt("Enter your choice: ", &choice);
        if (result == 0) {
            printf("\nInput closed. Exiting.\n");
            break;
        }
        if (result < 0) {
            continue;
        }
        if (choice == 0) {
            printf("Exiting Smart Grid Planner.\n");
            break;
        }
        if (choice == 1) {
            if (!readLine("Network file path (e.g. data/network.txt): ",
                          filename, sizeof(filename))) {
                printf("\nInput closed. Exiting.\n");
                break;
            }
            if (filename[0] == '\0') {
                printf("Error: File path cannot be empty.\n");
                continue;
            }
            if (loadNetworkSafely(&grid, &list, filename)) {
                loaded = 1;
            }
            continue;
        }
        if (choice < 0 || choice > 12) {
            printf("Invalid menu choice.\n");
            continue;
        }
        if (!loaded) {
            printf("Please load a network first (option 1).\n");
            continue;
        }

        switch (choice) {
            case 2:
                displayNodes(&grid);
                break;
            case 3:
                displayEdges(&grid);
                break;
            case 4:
                displayNetworkConnections(&grid, &list);
                break;
            case 5:
                result = readInt("First node ID: ", &fromId);
                if (result != 1) break;
                result = readInt("Second node ID: ", &toId);
                if (result != 1) break;
                printf("Direct active connection: %s\n",
                       areDirectlyConnected(&grid, &list, fromId, toId)
                       ? "YES" : "NO");
                break;
            case 6:
                displayIsolatedNodes(&grid, &list);
                break;
            case 7:
            case 8:
                result = readInt("First endpoint ID: ", &fromId);
                if (result != 1) break;
                result = readInt("Second endpoint ID: ", &toId);
                if (result != 1) break;
                changeLineStatus(&grid, &list, fromId, toId,
                                 choice == 8 ? 1 : 0);
                break;
            case 9: {

                NetworkStats stats;

                if (calculateNetworkStats(&grid, &list, &stats)) {

                    displayNetworkReport(&stats);

                } else {

                    printf("Error: Failed to calculate network statistics.\n");
                }

                break;
            }
            case 10:
                displayAdjacencyList(&grid, &list);
                break;
            case 11:
                result = readInt("Node ID: ", &fromId);
                if (result != 1) break;
                result = getActiveDegree(&grid, &list, fromId);
                if (result < 0) {
                    printf("Error: Node not found.\n");
                } else {
                    printf("Active degree of node %d: %d\n", fromId, result);
                }
                break;
            case 12:
                displayDisconnectedLoads(&grid, &list);
            break;
        }
    }
    return 0;
}
