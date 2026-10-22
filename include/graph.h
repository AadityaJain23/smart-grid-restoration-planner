
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

#endif
