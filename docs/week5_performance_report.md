# Week 5 — Performance Benchmark and Scalability Report

## 1. Objective

Evaluate the computational performance of Edmonds–Karp Maximum Flow and Priority-Aware Maximum Flow in the Smart Grid Power Distribution and Restoration Planner.

The experiments compare CPU time per solver call across the original smart-grid networks and six generated network configurations.

## 2. Experimental Environment

The implementation was developed in C11 and compiled using GCC on Windows.

The benchmark programs used `-O2` optimization.

Measurements were obtained using CPU-time measurements over repeated executions.

The original-network benchmark used five measurement batches with 1,000 executions per batch.

The generated-network scaling benchmark used five batches with 100 executions per batch.

Reported values represent measured CPU time in milliseconds per function call.

## 3. Original Network Results

| Network | Maximum Flow | Priority Flow | Runtime Ratio | Overhead |
|---|---:|---:|---:|---:|
| Original 100 MW | 0.006600 ms | 0.110400 ms | 16.727× | 1572.73% |
| Limited 60 MW | 0.006400 ms | 0.094000 ms | 14.688× | 1368.75% |

Priority-Aware Flow required more CPU time in both scenarios.

The absolute measured differences were 0.103800 ms and 0.087600 ms, respectively.

## 4. Scalability Results

| Nodes | Edges | Network Type | Maximum Flow (ms) | Priority Flow (ms) | Ratio |
|---|---:|---|---:|---:|---:|
| 10 | 9 | Sparse | 0.010000 | 0.104000 | 10.400× |
| 10 | 10 | Dense | 0.010000 | 0.090000 | 9.000× |
| 20 | 19 | Sparse | 0.010000 | 0.372000 | 37.200× |
| 20 | 25 | Dense | 0.018000 | 0.398000 | 22.111× |
| 30 | 29 | Sparse | 0.022000 | 0.950000 | 43.182× |
| 30 | 50 | Dense | 0.022000 | 1.574000 | 71.545× |

## 5. Scaling Analysis

### Sparse Networks

Priority Flow CPU time increased from 0.104 ms for 10 nodes to 0.950 ms for 30 nodes.

The measured growth factor was approximately 9.13×.

### Dense Networks

Priority Flow CPU time increased from 0.090 ms for 10 nodes to 1.574 ms for 30 nodes.

The measured growth factor was approximately 17.49×.

### Effect of Density

For the 30-node network, Priority Flow required 0.950 ms in the sparse configuration and 1.574 ms in the dense configuration.

The dense configuration took approximately 1.66× as much CPU time.

These observations describe the tested configurations only.

## 6. Algorithmic Complexity

Standard Edmonds–Karp Maximum Flow has worst-case complexity:

\[
O(VE^2)
\]

The current Week 4 matrix-scanning implementation performs approximately:

\[
O((K_4+1)V^2)
\]

for its augmenting-path search phase.

For the Week 5 Priority Flow implementation, let:

- \(V\) represent residual-network vertices.
- \(A\) represent residual arcs.
- \(K\) represent successful augmentations.
- \(C\) represent canceled negative cycles.

The approximate combined iterative work is:

\[
O((K+C+2)VA)
\]

This expression does not establish a polynomial worst-case bound because the number of negative-cycle cancellations has not been bounded polynomially.

## 7. Interpretation

The Priority-Aware Flow algorithm performs additional optimization to improve electricity allocation among consumer priority classes.

The measured runtime overhead is the computational trade-off associated with this additional functionality.

However, the measurements do not separately quantify residual-network construction, maximum-flow augmentation, negative-cycle detection, validation or result extraction.

The experiment also does not establish how the algorithms would perform on substantially larger networks.

## 8. Limitations

- The experiments use relatively small graphs.
- CPU measurements may vary across runs and environments.
- The number of benchmark configurations is limited.
- Graph topology and capacity distribution affect augmentation behavior.
- Timing ratios do not prove asymptotic complexity.
- The implementation models capacity-constrained network flow, not AC electrical power flow.

## 9. Conclusion

The experiments show that Priority-Aware Flow requires additional CPU time compared with ordinary Maximum Flow on the tested smart-grid networks.

The additional computation enables lexicographic consumer-priority optimization while preserving maximum feasible throughput.

The measured results provide an empirical basis for discussing the trade-off between computational cost and priority-aware electricity allocation.