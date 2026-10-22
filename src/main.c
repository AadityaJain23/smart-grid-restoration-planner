
#include <stdio.h>
#include "graph.h"

int main() {

    Graph grid;

    // Step 1: Initialize graph
    initializeGraph(&grid);

    // Step 2: Load graph from file
    if (!loadNetworkFromFile(&grid, "data/network.txt")) {

        printf("Failed to load smart grid network.\n");
        return 1;
    }

    // Step 3: Display loaded network
    printf("\nSMART GRID POWER DISTRIBUTION NETWORK\n");

    displayNodes(&grid);

    displayEdges(&grid);

    return 0;
}
