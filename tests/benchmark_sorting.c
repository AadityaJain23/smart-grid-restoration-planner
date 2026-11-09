#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define TRIALS 5

typedef struct {
    unsigned long long comparisons;
} SortStats;

static void merge(
    int a[],
    int temp[],
    int left,
    int mid,
    int right,
    SortStats *stats
) {
    int i = left;
    int j = mid + 1;
    int k = left;

    while (i <= mid && j <= right) {

        stats->comparisons++;

        if (a[i] <= a[j]) {
            temp[k++] = a[i++];
        } else {
            temp[k++] = a[j++];
        }
    }

    while (i <= mid) {
        temp[k++] = a[i++];
    }

    while (j <= right) {
        temp[k++] = a[j++];
    }

    for (int p = left; p <= right; p++) {
        a[p] = temp[p];
    }
}

static void mergeSort(
    int a[],
    int temp[],
    int left,
    int right,
    SortStats *stats
) {
    if (left >= right) {
        return;
    }

    int mid = left + (right - left) / 2;

    mergeSort(a, temp, left, mid, stats);

    mergeSort(a, temp, mid + 1, right, stats);

    merge(a, temp, left, mid, right, stats);
}

static int partition(
    int a[],
    int left,
    int right,
    SortStats *stats
) {
    int pivot = a[left];

    int i = left;
    int j = right + 1;

    while (1) {

        do {
            i++;

            if (i > right) {
                break;
            }

            stats->comparisons++;

        } while (a[i] < pivot);

        do {
            j--;

            stats->comparisons++;

        } while (a[j] > pivot);

        if (i >= j) {
            break;
        }

        int t = a[i];
        a[i] = a[j];
        a[j] = t;
    }

    int t = a[left];
    a[left] = a[j];
    a[j] = t;

    return j;
}

static void quickSort(
    int a[],
    int left,
    int right,
    SortStats *stats
) {
    if (left >= right) {
        return;
    }

    int pivotIndex = partition(
        a, left, right, stats
    );

    quickSort(
        a, left, pivotIndex - 1, stats
    );

    quickSort(
        a, pivotIndex + 1, right, stats
    );
}

static void generateData(
    int a[],
    int n,
    int pattern
) {
    for (int i = 0; i < n; i++) {

        switch (pattern) {

            case 0:
                a[i] = rand() % 100000;
                break;

            case 1:
                a[i] = i;
                break;

            case 2:
                a[i] = n - i;
                break;

            case 3:
                a[i] = rand() % 5;
                break;
        }
    }
}

static int isSorted(
    const int a[],
    int n
) {
    for (int i = 1; i < n; i++) {

        if (a[i - 1] > a[i]) {
            return 0;
        }
    }

    return 1;
}

static void runBenchmark(
    int n,
    int pattern
) {
    int *original = malloc((size_t)n * sizeof(int));
    int *mergeArray = malloc((size_t)n * sizeof(int));
    int *quickArray = malloc((size_t)n * sizeof(int));
    int *temp = malloc((size_t)n * sizeof(int));

    if (!original || !mergeArray ||
        !quickArray || !temp) {

        fprintf(stderr, "Memory allocation failed.\n");

        free(original);
        free(mergeArray);
        free(quickArray);
        free(temp);

        exit(EXIT_FAILURE);
    }

    double mergeTotal = 0.0;
    double quickTotal = 0.0;

    unsigned long long mergeComparisons = 0;
    unsigned long long quickComparisons = 0;

    for (int trial = 0; trial < TRIALS; trial++) {

        generateData(original, n, pattern);

        for (int i = 0; i < n; i++) {
            mergeArray[i] = original[i];
            quickArray[i] = original[i];
        }

        SortStats ms = {0};
        SortStats qs = {0};

        clock_t start = clock();

        mergeSort(
            mergeArray, temp, 0, n - 1, &ms
        );

        clock_t end = clock();

        mergeTotal +=
            (double)(end - start) / CLOCKS_PER_SEC;

        start = clock();

        quickSort(
            quickArray, 0, n - 1, &qs
        );

        end = clock();

        quickTotal +=
            (double)(end - start) / CLOCKS_PER_SEC;

        assert(isSorted(mergeArray, n));
        assert(isSorted(quickArray, n));

        for (int i = 0; i < n; i++) {
            assert(mergeArray[i] == quickArray[i]);
        }

        mergeComparisons += ms.comparisons;
        quickComparisons += qs.comparisons;
    }

    printf(
        "%-8d %-10d %-14.6f %-14.6f %-16llu %-16llu\n",
        n,
        pattern,
        mergeTotal / TRIALS,
        quickTotal / TRIALS,
        mergeComparisons / TRIALS,
        quickComparisons / TRIALS
    );

    free(original);
    free(mergeArray);
    free(quickArray);
    free(temp);
}

int main(void) {

    int sizes[] = {
        100, 1000, 5000, 10000
    };

    srand(42);

    printf("MERGE SORT VS QUICK SORT BENCHMARK\n\n");

    printf(
        "%-8s %-10s %-14s %-14s %-16s %-16s\n",
        "N",
        "Pattern",
        "Merge(s)",
        "Quick(s)",
        "Merge Comp",
        "Quick Comp"
    );

    printf("------------------------------------------------"
           "------------------------------------------------\n");

    for (int i = 0; i < 4; i++) {

        for (int pattern = 0; pattern < 4; pattern++) {

            runBenchmark(
                sizes[i],
                pattern
            );
        }
    }

    return 0;
}