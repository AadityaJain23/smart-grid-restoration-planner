# Week 5 — Limitations, Known Issues and Future Improvements

## 1. Purpose

This document records the known limitations, outstanding compatibility issues and potential future improvements of the Smart Grid Power Distribution and Restoration Planner.

The project is implemented in offline C11 and uses graph algorithms to study capacity-constrained electricity delivery and priority-aware restoration.

## 2. Network Model Limitations

The system represents electricity infrastructure as a graph consisting of generators, substations, consumers and transmission lines.

Transmission lines have capacities, costs and active or inactive states.

The model supports network-flow calculations but does not implement physical AC power-flow equations.

Consequently, it does not calculate:

- Voltage magnitudes or phase angles.
- Reactive power.
- Frequency stability.
- Electrical impedance and line losses.
- Three-phase power-system behavior.
- Thermal or voltage stability constraints beyond the modeled capacities.

The results should therefore be interpreted as graph-based allocation decisions rather than physically validated electrical operating points.

## 3. Numerical Precision

The implementation uses floating-point values for generation, demand, transmission capacity and delivered power.

Numerical tolerances are used to compare flow values and avoid instability caused by small floating-point errors.

However, very small positive capacities may fall below the configured tolerance and be treated as zero.

This behavior can affect networks with extremely small flow values.

A possible improvement is to define a consistent absolute and relative tolerance policy across all flow-related modules.

## 4. Priority Optimization Limitations

The Priority-Aware Flow Solver maximizes total feasible throughput and then performs lexicographic optimization using residual negative-cycle cancellation.

The implementation includes an iteration safety limit to prevent uncontrolled optimization loops.

If the safety limit is reached before the required optimality condition is established, the solver must not report an unverified allocation as successfully optimized.

A future improvement could investigate alternative minimum-cost-flow formulations or algorithms with more clearly established complexity guarantees.

## 5. Complexity Limitations

The current implementation's iterative work can be described approximately using:

\[
O((K+C+2)VA)
\]

where:

- \(V\) is the number of residual vertices.
- \(A\) is the number of residual arcs.
- \(K\) is the number of successful augmentations.
- \(C\) is the number of canceled negative cycles.

This expression does not establish a polynomial worst-case bound because the number of cycle cancellations has not been bounded polynomially.

Additional theoretical analysis would be needed to establish stronger complexity guarantees.

## 6. Allocation Uniqueness

The lexicographic objective prioritizes total delivery to Priority 1, then Priority 2, then Priority 3 consumers.

However, consumers within the same priority class are not necessarily assigned a unique allocation.

Multiple feasible allocations may produce the same optimal priority totals.

The current implementation uses deterministic traversal behavior, but uniqueness of individual allocations is not guaranteed by the mathematical objective.

## 7. Benchmarking Limitations

The reported experiments cover the original nine-node network and generated networks containing 10, 20 and 30 nodes.

The experiments compare sparse and dense transmission configurations.

Although the results demonstrate measurable computational overhead for priority-aware optimization, they do not establish performance on very large networks.

Additional experiments could investigate:

- Larger graph sizes.
- Different transmission densities.
- Multiple generator configurations.
- Different demand distributions.
- Highly constrained network cuts.
- Networks containing many competing priority routes.

Benchmark results may also vary with compiler optimization, processor characteristics and measurement conditions.

## 8. Historical Test Compatibility Issue

The historical `test_priority_negative_cycle.c` program currently fails to link because `runPriorityFlowPrototype()` is unavailable.

The issue arose during cleanup of earlier development code.

The production solver and the six Lecture 10 integration regression programs have passed their reported verification checks.

However, the historical test compatibility issue remains unresolved.

### Required Follow-Up

1. Inspect the current `runPriorityFlowPrototype()` source section.
2. Determine whether its implementation is commented out or otherwise excluded.
3. Restore test-only compatibility without modifying the working production solver.
4. Recompile the affected historical test.
5. Run the complete historical regression suite.
6. Record the final outcome in the code-quality audit.

The issue must be resolved before claiming complete historical regression success.

## 9. Future Improvements

### 9.1 Stronger Optimization Guarantees

Investigate alternative lexicographic minimum-cost-flow approaches with established termination and complexity properties.

### 9.2 Larger-Scale Performance Evaluation

Benchmark substantially larger networks and investigate the relationship between residual-network size, augmentation count, cycle cancellations and runtime.

### 9.3 Improved Numerical Robustness

Standardize floating-point tolerance handling and test additional fractional-capacity scenarios.

### 9.4 Enhanced Restoration Modeling

Extend restoration analysis to include additional operational constraints where they are genuinely relevant to the smart-grid problem.

### 9.5 Better Test Organization

Separate production source compilation from internal unit-test instrumentation.

Preserve historical algorithm tests while avoiding unnecessary legacy code in production builds.

### 9.6 Documentation and Reproducibility

Maintain consistent build instructions, regression reports, benchmark evidence and known-issue records.

## 10. Current Verification Status

| Component | Status |
|---|---|
| Priority-Aware Flow production solver | Verified on reported test scenarios |
| Strict production compilation | Passed |
| CLI option 24 integration | Passed |
| Lecture 10 integration regression | Six programs passed |
| Original and limited network allocations | Passed |
| Historical prototype compatibility | Unresolved |
| Large-network scalability guarantee | Not established |

## 11. Conclusion

The Smart Grid Power Distribution and Restoration Planner demonstrates graph-based electricity allocation using maximum-flow and lexicographic priority optimization.

The production implementation has passed the reported correctness, integration and performance checks.

Its limitations are documented to distinguish demonstrated functionality from unverified guarantees and future development opportunities.