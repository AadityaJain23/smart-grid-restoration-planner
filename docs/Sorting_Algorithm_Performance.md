### Sorting Algorithm Performance Analysis

Merge Sort and Quick Sort were experimentally compared using C11 implementations on random, sorted, reverse-sorted and duplicate-heavy datasets containing 100, 1,000, 5,000 and 10,000 elements.

The results showed that both algorithms exhibited behavior consistent with \(O(n\log n)\) complexity on random inputs. However, the first-element-pivot Quick Sort demonstrated quadratic behavior on sorted and reverse-sorted inputs.

For 10,000 already sorted elements, Merge Sort performed 69,008 key comparisons, whereas Quick Sort performed 50,014,998 comparisons. When the input size increased from 5,000 to 10,000, Quick Sort's comparison count increased by approximately four times, supporting its theoretical \(O(n^2)\) worst-case complexity.

On duplicate-heavy datasets, Quick Sort performed substantially better than on sorted distinct inputs because the two-pointer partitioning scheme handled repeated pivot values more evenly.

The execution-time results were broadly consistent with these findings, although some measurements were too small for reliable interpretation using the available timer resolution.

**Conclusion:** Merge Sort offers predictable \(O(n\log n)\) performance and stability, making it suitable for consistently organizing consumer demand records. Quick Sort can be efficient for transmission-line cost sorting, but its performance depends on pivot selection and input ordering. The benchmark highlights the importance of choosing algorithms based on both theoretical complexity and observed performance.