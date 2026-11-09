# Sorting Algorithm Complexity Comparison

## Project
Smart Grid Power Distribution and Restoration Planner

## Objective
Compare Merge Sort and Quick Sort in terms of
correctness, key comparisons, execution time,
and theoretical complexity.

## Algorithms

### Merge Sort
- Technique: Divide and Conquer
- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n log n)
- Stable: Yes
- Auxiliary buffer: O(n)

### Quick Sort
- Technique: Divide and Conquer
- Pivot: First element
- Partition: Two-pointer method
- Best: O(n log n)
- Average: O(n log n)
- Worst: O(n^2)
- Stable: No
- Recursion: O(log n) average, O(n) worst

## Smart Grid Mapping
Merge Sort organizes consumer indices by demand.
Quick Sort organizes transmission-line indices by cost.

## Benchmark Methodology
- Language: C11
- Compiler: GCC
- Optimization: -O2
- Trials: 5
- Inputs: Random, sorted, reverse, duplicate-heavy
- Sizes: 100, 1000, 5000, 10000
- Metrics: CPU time and key comparisons

The benchmark uses standalone integer-array
implementations of the same sorting techniques.

## Experimental Results

Insert actual measured results here.

## Analysis

Discuss:
1. Correctness
2. Runtime differences
3. Comparison-count differences
4. Input-order sensitivity
5. Space complexity
6. Suitability for smart-grid records

## Conclusion

Write the conclusion after completing the experiment.