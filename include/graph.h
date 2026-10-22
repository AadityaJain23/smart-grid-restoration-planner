
#ifndef GRAPH_H
#define GRAPH_H

#define MAX_NODES 50
#define MAX_EDGES 200
#define NAME_SIZE 50

// Types of components in the smart grid
typedef enum {
    GENERATOR,
    SUBSTATION,
    CONSUMER
} NodeType;

// Structure representing a node
typedef struct {
    int id;
    char name[NAME_SIZE];
    NodeType type;
    double demand;
    double supply;
    int priority;
} Node;

// Structure representing a transmission line
typedef struct {
    int from;
    int to;
    double distance;
    double capacity;
    double cost;
    int active;
} Edge;

// Structure representing the complete graph
typedef struct {
    Node nodes[MAX_NODES];
    Edge edges[MAX_EDGES];
    int nodeCount;
    int edgeCount;
} Graph;

// Graph function declarations
void initializeGraph(Graph *graph);
int addNode(Graph *graph, Node node);
int addEdge(Graph *graph, Edge edge);
void displayNodes(const Graph *graph);
void displayEdges(const Graph *graph);
int loadNetworkFromFile(Graph *graph, const char *filename);


typedef struct {
    int neighborIndex;
    int edgeIndex;
    int next;
} AdjEntry;

typedef struct {
    int head[MAX_NODES];
    AdjEntry entries[2 * MAX_EDGES];
    int entryCount;
} AdjList;



void initializeAdjList(AdjList *list);

int buildAdjacencyList(
    const Graph *graph,
    AdjList *list
);

void displayAdjacencyList(
    const Graph *graph,
    const AdjList *list
);

void displayNetworkConnections(
    const Graph *graph,
    const AdjList *list
);



int areDirectlyConnected(
    const Graph *graph,
    const AdjList *list,
    int fromId,
    int toId
);

int getActiveDegree(
    const Graph *graph,
    const AdjList *list,
    int nodeId
);

void displayIsolatedNodes(
    const Graph *graph,
    const AdjList *list
);

int validateAdjacencyList(
    const Graph *graph,
    const AdjList *list
);

int validateNodeData(const Node *node);

int validateEdgeData(const Edge *edge);


// int validateEdgeData(const Edge *edge) {

//     if (edge == NULL) {
//         return 0;
//     }

//     if (edge->from < 0 || edge->to < 0) {
//         return 0;
//     }

//     if (edge->from == edge->to) {
//         return 0;
//     }

//     if (!isfinite(edge->distance) ||
//         !isfinite(edge->capacity) ||
//         !isfinite(edge->cost)) {
//         return 0;
//     }

//     if (edge->distance <= 0 ||
//         edge->capacity <= 0 ||
//         edge->cost < 0) {
//         return 0;
//     }

//     if (edge->active != 0 && edge->active != 1) {
//         return 0;
//     }

//     return 1;
// }

void findGeneratorReachability(
    const Graph *graph,
    const AdjList *list,
    int reachable[MAX_NODES]
);

void displayDisconnectedLoads(
    const Graph *graph,
    const AdjList *list
);


int validateNodeData(const Node *node);

int validateEdgeData(const Edge *edge);

void findGeneratorReachability(
    const Graph *graph,
    const AdjList *list,
    int reachable[MAX_NODES]
);

void displayDisconnectedLoads(
    const Graph *graph,
    const AdjList *list
);


typedef struct {

    int totalNodes;
    int totalEdges;

    int generators;
    int substations;
    int consumers;

    int activeLines;
    int failedLines;

    int disconnectedConsumers;
    int criticalDisconnected;

    double totalSupply;
    double totalDemand;

    double reachableDemand;
    double disconnectedDemand;

    double totalActiveCapacity;
    double totalActiveCost;

} NetworkStats;


int calculateNetworkStats(
    const Graph *graph,
    const AdjList *list,
    NetworkStats *stats
);

void displayNetworkReport(
    const NetworkStats *stats
);


#endif
