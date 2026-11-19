# Week 5 — Algorithm Design and Complexity Comparison

## 1. Overview

The Smart Grid Power Distribution and Restoration Planner provides two network-flow algorithms.

**Week 4 — Edmonds–Karp Maximum Flow**

Calculates the maximum total electricity that can be delivered through the capacity-constrained smart-grid network.

**Week 5 — Priority-Aware Maximum Flow**

Calculates maximum feasible electricity delivery and then lexicographically optimizes allocation to consumers according to priority classes.

Both algorithms operate on the same graph representation and respect active transmission-line states, generator supply and consumer demand.

## 2. Optimization Objectives

The Maximum Flow algorithm maximizes total delivered electricity.

The Priority-Aware Flow algorithm first maximizes total throughput and then optimizes the vector:

\[
(P_1,P_2,P_3)
\]

in lexicographic order.

Priority 1 is optimized before Priority 2, and Priority 2 before Priority 3.

## 3. Maximum Flow Algorithm

The Week 4 implementation uses Edmonds–Karp-style augmenting paths.

A super-source represents generator supply, while a super-sink represents consumer demand.

BFS identifies augmenting paths in the residual network.

The algorithm augments flow until no further source-to-sink path with positive residual capacity exists.

The implementation uses a residual-capacity matrix.

The standard Edmonds–Karp algorithm has worst-case complexity:

\[
O(VE^2)
\]

The matrix-scanning implementation performs approximately:

\[
O((K_4+1)V^2)
\]

for its augmenting-path search phase, where \(K_4\) is the number of successful augmentations.

## 4. Priority-Aware Flow Algorithm

The Week 5 implementation uses a residual network containing directed arcs and reverse-arc indices.

The algorithm operates in three main phases.

### Phase 1 — Maximum Flow

Construct the priority residual network and use BFS-based augmentation to establish maximum feasible total throughput.

### Phase 2 — Lexicographic Optimization

Assign lexicographic cost vectors to consumer-related residual arcs.

Detect negative-cost residual cycles using Bellman–Ford-style relaxation.

Cancel improving cycles while preserving total throughput.

Continue until no improving cycle remains, subject to the implementation's safety limit.

### Phase 3 — Validation and Extraction

Verify generator supply, consumer demand, transmission capacity, flow conservation and residual-network consistency.

Extract total delivered electricity, unserved demand, priority totals and individual consumer allocations.

## 5. Complexity Analysis

Let:

- \(V\): residual-network vertices.
- \(A\): residual arcs.
- \(K\): successful maximum-flow augmentations.
- \(C\): canceled negative cycles.

For the current arc-scanning implementation, a BFS search may require \(O(VA)\) operations.

A Bellman–Ford-style negative-cycle detection pass requires \(O(VA)\) operations.

The combined iterative work is approximately:

\[
O((K+C+2)VA)
\]

This expression depends on the observed numbers of augmentations and cycle cancellations.

It is not a proven polynomial worst-case bound because the number of cycle cancellations has not been bounded polynomially.

Residual-network construction, validation and extraction contribute additional work.

## 6. Correctness Conditions

Both algorithms must satisfy:

- Generator supply limits.
- Consumer demand limits.
- Transmission-line capacity constraints.
- Flow conservation.
- Active-line connectivity restrictions.

Priority Flow must additionally preserve maximum total throughput and produce a lexicographically optimal priority allocation.

For successful calculations:

\[
F_{\text{priority}}=F_{\text{maximum}}
\]

and:

\[
P_1+P_2+P_3=F_{\text{priority}}
\]

## 7. Empirical Performance

The original 100 MW network produced the following measured CPU times:

| Algorithm | CPU Time |
|---|---:|
| Maximum Flow | 0.006600 ms |
| Priority Flow | 0.110400 ms |

The limited 60 MW network produced:

| Algorithm | CPU Time |
|---|---:|
| Maximum Flow | 0.006400 ms |
| Priority Flow | 0.094000 ms |

The Priority Flow implementation required more CPU time in both tested scenarios because it performs additional optimization and validation work.

These measurements do not establish asymptotic complexity.

## 8. Practical Interpretation

Maximum Flow is suitable when the primary objective is maximizing aggregate electricity delivery.

Priority-Aware Flow is appropriate when electricity shortages require critical consumers to receive preference over lower-priority consumers.

The implementation is a capacity-constrained graph optimization model rather than an AC electrical power-flow simulator.

## 9. Limitations

- Numerical tolerances can affect extremely small capacities.
- Negative-cycle cancellation uses an iteration safety limit.
- Multiple equally optimal individual allocations may exist within a priority class.
- A general polynomial worst-case bound for the current cycle-cancellation implementation has not been established.
- Historical prototype test compatibility remains partially unresolved.

## 10. Conclusion

The Week 5 algorithm extends the Week 4 Maximum Flow model by adding lexicographic consumer-priority optimization while preserving maximum feasible throughput.

The design demonstrates the trade-off between aggregate throughput optimization and priority-aware allocation in a constrained smart-grid network.