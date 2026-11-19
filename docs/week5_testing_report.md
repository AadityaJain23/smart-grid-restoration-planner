# Week 5 — Testing and Regression Verification Report

## 1. Project Overview

**Project:** Smart Grid Power Distribution and Restoration Planner

**Language:** C11

**Environment:** GCC, VS Code and Windows PowerShell

**Module:** Priority-Aware Maximum Flow

The purpose of this report is to document the verification methodology, observed test results and remaining limitations of the Week 5 implementation.

## 2. Testing Objectives

The testing process evaluates whether the Priority-Aware Flow Solver:

- Maximizes feasible total electricity delivery.
- Allocates power according to lexicographic consumer priorities.
- Respects generator supply limits.
- Respects consumer demand limits.
- Respects transmission-line capacities and active states.
- Preserves the original graph.
- Produces consistent results across repeated executions.
- Integrates correctly with the existing CLI.

## 3. Testing Methodology

The implementation was tested using deterministic network configurations and C11 assertion-based test programs.

Tests were compiled using GCC with warning flags including:

`-std=c11 -Wall -Wextra -Wpedantic`

Strict production verification additionally used `-Werror`.

The test programs compare observed allocations with analytically expected values and verify mathematical invariants.

## 4. Mathematical Invariants

### Maximum Throughput

Priority Flow must preserve the maximum feasible total delivery:

\[
F_{\text{priority}}=F_{\text{maximum}}
\]

### Generator Supply

\[
0\le F_{\text{delivered}}\le S
\]

### Consumer Demand

For each consumer:

\[
0\le f_i\le d_i
\]

### Priority Accounting

\[
P_1+P_2+P_3=F_{\text{delivered}}
\]

### Demand Accounting

\[
F_{\text{delivered}}+F_{\text{unserved}}=D
\]

The implementation also verifies flow conservation and transmission-capacity feasibility.

## 5. Automated Integration Regression

The Lecture 10 regression suite executes six independent test programs.

| Test Program | Verification Focus | Reported Status |
|---|---|---|
| `test_priority_system_invariants` | Throughput and accounting | PASS |
| `test_priority_cross_algorithm` | Maximum Flow consistency | PASS |
| `test_priority_restoration_consistency` | Failure and recovery | PASS |
| `test_priority_capacity_boundaries` | Supply and capacity limits | PASS |
| `test_priority_state_preservation` | Graph immutability | PASS |
| `test_priority_determinism` | Repeated execution consistency | PASS |

**Reported regression summary**

- Total programs: 6
- Passed: 6
- Failed: 0

The results are recorded in:

`docs/test_results/week5_lecture10.csv`

## 6. Original Network Verification

The original network has 100 MW of generation and 100 MW of consumer demand.

The reported Priority Flow results were:

| Metric | Result |
|---|---:|
| Total delivered | 100 MW |
| Priority 1 | 20 MW |
| Priority 2 | 10 MW |
| Priority 3 | 70 MW |
| Unserved demand | 0 MW |

The ordinary Maximum Flow algorithm also delivered 100 MW.

## 7. Limited-Supply Network Verification

The limited network has 60 MW of generation and 100 MW of consumer demand.

The reported results were:

| Metric | Result |
|---|---:|
| Total delivered | 60 MW |
| Priority 1 | 20 MW |
| Priority 2 | 10 MW |
| Priority 3 | 30 MW |
| Unserved demand | 40 MW |

The ordinary Maximum Flow algorithm also delivered 60 MW.

The Priority-Aware Flow Solver preserved maximum throughput while prioritizing critical consumers.

## 8. Failure and Restoration Testing

The integrated CLI was tested under transmission-line failure and restoration scenarios.

Reported observations included:

| Scenario | Total Delivered |
|---|---:|
| Original network | 100 MW |
| Hospital line failed | 80 MW |
| Hospital line restored | 100 MW |
| Redundant network after primary-line failure | 100 MW |

An invalid transmission-line operation was rejected without corrupting the network.

## 9. Determinism and State Preservation

The original and limited networks each passed 100 repeated Priority Flow executions.

The tests compared total delivery, priority totals, individual consumer allocations and augmenting-path counts.

The graph-state preservation tests also verified that flow calculations did not modify the original graph representation.

## 10. Production Compilation

The production implementation and integrated CLI passed strict GCC compilation using:

`-std=c11 -Wall -Wextra -Wpedantic -Werror`

The public functions `calculatePriorityFlow()` and `displayPriorityFlowReport()` were verified as externally visible.

## 11. Outstanding Historical Test Issue

The historical `test_priority_negative_cycle.c` program currently fails to link because `runPriorityFlowPrototype()` is unavailable.

This issue arose during legacy helper isolation.

The production implementation and six Lecture 10 integration regression programs passed their reported checks.

However, the complete historical regression suite cannot be declared fully passing until this compatibility issue is resolved.

## 12. Testing Limitations

- Tests cover selected deterministic networks, not every possible graph.
- Floating-point tolerances can affect extremely small capacities.
- The solver uses an iteration safety limit for negative-cycle cancellation.
- Repeated successful executions do not prove thread safety.
- The implementation is a capacity-constrained network-flow abstraction, not an AC power-flow simulator.
- The historical prototype test issue remains unresolved.

## 13. Conclusion

The reported testing results provide substantial evidence that the production Priority-Aware Flow Solver correctly maximizes total electricity delivery while optimizing allocations across consumer priority classes.

The integrated application passed the documented production regression and CLI checks.

Complete historical regression compatibility remains an outstanding cleanup task.