
#include <math.h>
#include <stdio.h>

#define SCENARIOS 6

typedef struct {
    int nodes;
    int edges;
    int dense;
    double maxflowMs;
    double priorityMs;
} BenchmarkRow;

int main(void)
{
    BenchmarkRow rows[SCENARIOS];

    printf("Enter six benchmark rows.\n");
    printf("Format: nodes edges dense(0/1) "
           "maxflow_ms priority_ms\n\n");

    for (int i = 0; i < SCENARIOS; i++) {

        printf("Scenario %d: ", i + 1);

        if (scanf("%d %d %d %lf %lf",
                  &rows[i].nodes,
                  &rows[i].edges,
                  &rows[i].dense,
                  &rows[i].maxflowMs,
                  &rows[i].priorityMs) != 5) {
            printf("[FAIL] Invalid input\n");
            return 1;
        }

        if (rows[i].nodes <= 0 ||
            rows[i].edges < 0 ||
            (rows[i].dense != 0 &&
             rows[i].dense != 1) ||
            !isfinite(rows[i].maxflowMs) ||
            !isfinite(rows[i].priorityMs) ||
            rows[i].maxflowMs <= 0.0 ||
            rows[i].priorityMs <= 0.0) {

            printf("[FAIL] Invalid measurement\n");
            return 1;
        }
    }

    printf("\n");
    printf("========== SCALING ANALYSIS ==========\n");

    printf("%-6s %-6s %-8s %-11s %-11s "
           "%-8s %s\n",
           "Nodes", "Edges", "Type",
           "MaxFlow", "Priority",
           "Ratio", "Overhead");

    for (int i = 0; i < SCENARIOS; i++) {

        double ratio =
            rows[i].priorityMs /
            rows[i].maxflowMs;

        double overhead =
            (ratio - 1.0) * 100.0;

        printf("%-6d %-6d %-8s %-11.6f "
               "%-11.6f %-8.3f %+.2f%%\n",
               rows[i].nodes,
               rows[i].edges,
               rows[i].dense ? "Dense" : "Sparse",
               rows[i].maxflowMs,
               rows[i].priorityMs,
               ratio,
               overhead);
    }

    printf("======================================\n");
    printf("[PASS] Scaling analysis completed\n");

    return 0;
}
