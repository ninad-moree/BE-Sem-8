#include <iostream>
#include <vector>
#include <chrono>
#include <omp.h>

using namespace std;
using namespace std::chrono;

void bubbleSort(vector<int> &arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

void merge(vector<int> &arr, int l, int m, int r) {
    int n1 = m - l + 1;
    int n2 = r - m;

    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; ++i)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; ++j)
        R[j] = arr[m + 1 + j];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            ++i;
        } else {
            arr[k] = R[j];
            ++j;
        }
        ++k;
    }

    while (i < n1) {
        arr[k] = L[i];
        ++i;
        ++k;
    }

    while (j < n2) {
        arr[k] = R[j];
        ++j;
        ++k;
    }
}

void mergeSort(vector<int> &arr, int l, int r)  {
    if (l < r) {
        int m = l + (r - l) / 2;

        mergeSort(arr, l, m);
        mergeSort(arr, m + 1, r);

        merge(arr, l, m, r);
    }
}

void parallelBubbleSort(vector<int> &arr) {
    int n = arr.size();
    for (int i = 0; i < n - 1; ++i) {
        #pragma omp parallel for
        for (int j = 0; j < n - i - 1; ++j) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

void parallelMergeSort(vector<int> &arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        
        #pragma omp parallel sections
        {
            #pragma omp section
            parallelMergeSort(arr, l, m);
            #pragma omp section
            parallelMergeSort(arr, m + 1, r);
        }
        merge(arr, l, m, r);
    }
}

int main() {
    const int size = 10000; 
    vector<int> arr(size), arr_copy(size);

    for (int i = 0; i < size; ++i) {
        arr[i] = rand() % 1000;
        arr_copy[i] = arr[i];
    }

    auto start = high_resolution_clock::now();
    bubbleSort(arr_copy);
    auto stop = high_resolution_clock::now();
    auto seq_duration_bubble = duration_cast<milliseconds>(stop - start);

    start = high_resolution_clock::now();
    parallelBubbleSort(arr);
    stop = high_resolution_clock::now();
    auto par_duration_bubble = duration_cast<milliseconds>(stop - start);

    start = high_resolution_clock::now();
    mergeSort(arr_copy, 0, size - 1);
    stop = high_resolution_clock::now();
    auto seq_duration_merge = duration_cast<milliseconds>(stop - start);

    start = high_resolution_clock::now();
    parallelMergeSort(arr, 0, size - 1);
    stop = high_resolution_clock::now();
    auto par_duration_merge = duration_cast<milliseconds>(stop - start);

    cout << "Sequential Bubble Sort Time: " << seq_duration_bubble.count() << " milliseconds" << endl;
    cout << "Parallel Bubble Sort Time: " << par_duration_bubble.count() << " milliseconds" << endl;
    cout << "Sequential Merge Sort Time: " << seq_duration_merge.count() << " milliseconds" << endl;
    cout << "Parallel Merge Sort Time: " << par_duration_merge.count() << " milliseconds" << endl;

    return 0;
}
