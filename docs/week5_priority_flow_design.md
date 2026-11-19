# Week 5 — Priority-Aware Maximum Flow: Technical Design

## 1. Overview

The Smart Grid Power Distribution and Restoration Planner implements a priority-aware maximum-flow algorithm in C11.

The algorithm determines the maximum feasible electricity delivery while preferentially allocating power to higher-priority consumers.

The system uses a graph-based abstraction of the electricity distribution network. It does not model physical AC power flow, voltage, frequency or reactive power.

## 2. Optimization Objectives

The solver has two objectives:

1. Maximize total feasible electricity delivery.
2. Among maximum-throughput allocations, lexicographically maximize delivery to Priority 1, then Priority 2, then Priority 3 consumers.

The second objective is represented by the allocation vector:

\[
(P_1,P_2,P_3)
\]

Priority 1 is compared first. Lower-priority allocations are considered only when all higher-priority totals are equal.

## 3. Data Structures

The implementation uses:

- `Graph` — Original network representation.
- `PriorityNetwork` — Residual network containing directed arcs.
- `PriorityArc` — Residual capacity, endpoints, reverse-arc index and cost.
- `PriorityCost` — Three-component lexicographic cost vector.
- `PriorityFlowResult` — Final electricity allocation and reporting data.

The original graph remains authoritative and is not modified by the solver.

## 4. Phase 1 — Maximum Feasible Flow

The solver constructs a residual network using the original graph.

A super-source connects to generators according to available supply.

Consumer nodes connect to a super-sink according to their demand.

Active transmission lines contribute residual connections respecting shared physical-line capacities.

The solver repeatedly performs BFS to discover augmenting paths and increases flow until no additional source-to-sink augmentation is possible.

## 5. Phase 2 — Priority Optimization

After maximum throughput is established, the solver improves the consumer allocation.

Each consumer-to-sink connection receives a lexicographic cost corresponding to its priority class.

Negative-cost residual cycles represent opportunities to improve the priority allocation while preserving total throughput.

The solver repeatedly detects and cancels improving cycles.

Optimization succeeds only when no improving negative cycle remains, subject to the implementation's safety limit.

## 6. Phase 3 — Validation and Result Extraction

The implementation verifies:

- Generator supply constraints.
- Consumer demand constraints.
- Transmission-line capacity constraints.
- Flow conservation.
- Residual-network consistency.
- Maximum-throughput preservation.

The final result contains total supply, total demand, total delivered power, unserved demand, delivered power per priority class, individual consumer allocations and the number of augmenting paths.

## 7. Correctness Invariants

The following conditions must hold for a successful result:

**Maximum throughput preservation**

\[
F_{\text{priority}}=F_{\text{maximum}}
\]

**Supply constraint**

\[
0\le F_{\text{delivered}}\le S
\]

**Individual consumer demand**

\[
0\le f_i\le d_i
\]

**Priority accounting**

\[
P_1+P_2+P_3=F_{\text{delivered}}
\]

**Demand accounting**

\[
F_{\text{delivered}}+F_{\text{unserved}}=D
\]

Physical transmission flows must respect their shared capacity limits, and flow must be conserved at every network node.

Floating-point comparisons use numerical tolerances where appropriate.

## 8. Complexity Analysis

Let:

- \(V\) = residual-network vertices.
- \(A\) = residual arcs.
- \(K\) = successful maximum-flow augmentations.
- \(C\) = canceled negative cycles.

The implementation's arc-scanning BFS requires approximately \(O(VA)\) work per search.

The negative-cycle detector uses Bellman–Ford-style relaxation requiring \(O(VA)\) work per detection pass.

An approximate expression for the combined iterative work is:

\[
O((K+C+2)VA)
\]

This is an operation-count expression rather than a proven polynomial worst-case bound. The implementation has not established a polynomial bound on the number of cycle cancellations.

## 9. Public API

The public interface is declared in `include/priority_flow.h`.

```c
int calculatePriorityFlow(
    const Graph *graph,
    PriorityFlowResult *result
);

void displayPriorityFlowReport(
    const Graph *graph,
    const PriorityFlowResult *result
);
```

The implementation is located in `src/priority_flow.c`.

CLI option 24 invokes the production solver and displays the resulting allocation.

## 10. Testing and Validation

The implementation has undergone unit, integration, robustness and performance testing.

The reported tests include:

- Maximum Flow and Priority Flow consistency.
- Priority allocation correctness.
- Generator supply and consumer demand validation.
- Transmission-capacity boundaries.
- Failure and restoration behavior.
- Original graph preservation.
- Repeated deterministic execution.
- CLI integration.
- Sparse and dense network benchmarks.

The original 100 MW network delivered 100 MW.

The limited 60 MW network delivered 60 MW with priority totals of 20 MW, 10 MW and 30 MW.

These tests provide evidence of correctness for the tested scenarios but do not constitute a formal proof for every possible graph.

## 11. Known Limitations

- The model is a capacity-constrained network-flow abstraction, not an AC power-flow simulator.
- Floating-point tolerances can affect extremely small capacities.
- Negative-cycle cancellation uses an iteration safety limit.
- General worst-case polynomial complexity has not been established.
- Multiple equally optimal allocations may exist within the same priority class.
- Legacy prototype tests require separate compatibility handling.

## 12. Conclusion

The Priority-Aware Maximum Flow implementation extends the existing Smart Grid Restoration Planner by incorporating consumer priority into maximum-throughput allocation.

The modular design separates graph representation, residual-network optimization, feasibility validation and result reporting.

The implementation supports critical-load prioritization while respecting generator supply, consumer demand, transmission capacity and network connectivity.