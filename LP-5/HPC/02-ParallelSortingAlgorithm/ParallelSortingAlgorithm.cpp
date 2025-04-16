#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

void bubbleSortSequential(vector<int>& arr) {
    int n = arr.size();
    for(int i = 0; i < n-1; ++i)
        for(int j = 0; j < n-i-1; ++j)
            if(arr[j] > arr[j+1])
                swap(arr[j], arr[j+1]);
}

void bubbleSortParallel(vector<int>& arr) {
    int n = arr.size();
    for(int i = 0; i < n; ++i) {
        #pragma omp parallel for
        for(int j = i % 2; j < n - 1; j += 2) {
            if(arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

// Merge function
void merge(vector<int>& arr, int l, int m, int r) {
    vector<int> left(arr.begin() + l, arr.begin() + m + 1);
    vector<int> right(arr.begin() + m + 1, arr.begin() + r + 1);

    int i = 0, j = 0, k = l;
    while(i < left.size() && j < right.size()) {
        arr[k++] = (left[i] <= right[j]) ? left[i++] : right[j++];
    }

    while(i < left.size()) arr[k++] = left[i++];
    while(j < right.size()) arr[k++] = right[j++];
}

void mergeSortSequential(vector<int>& arr, int l, int r) {
    if(l < r) {
        int m = l + (r - l) / 2;
        mergeSortSequential(arr, l, m);
        mergeSortSequential(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void mergeSortParallel(vector<int>& arr, int l, int r, int depth = 0) {
    if(l < r) {
        int m = l + (r - l) / 2;

        if(depth < 4) {
            #pragma omp parallel sections
            {
                #pragma omp section
                mergeSortParallel(arr, l, m, depth + 1);
                #pragma omp section
                mergeSortParallel(arr, m + 1, r, depth + 1);
            }
        } else {
            mergeSortSequential(arr, l, m);
            mergeSortSequential(arr, m + 1, r);
        }

        merge(arr, l, m, r);
    }
}

// Utility to generate random array
vector<int> generateRandomArray(int size, int maxVal = 10000) {
    vector<int> arr(size);
    for(int i = 0; i < size; ++i)
        arr[i] = rand() % maxVal;
    return arr;
}

// Utility to measure and compare sort functions
template<typename Func>
double measureTime(Func sortFunc, vector<int> arr) {
    double start = omp_get_wtime();
    sortFunc(arr);
    return omp_get_wtime() - start;
}

int main() {
    srand(time(0));
    const int SIZE = 10000;

    vector<int> original = generateRandomArray(SIZE);

    auto testAndPrint = [&](const string& label, auto func) {
        vector<int> arr = original;
        double time = measureTime(func, arr);
        cout << label << ": " << time << " seconds" << endl;
    };

    cout << "Running on " << omp_get_max_threads() << " threads\n\n";

    testAndPrint("Sequential Bubble Sort", bubbleSortSequential);
    testAndPrint("Parallel Bubble Sort", bubbleSortParallel);

    testAndPrint("Sequential Merge Sort", [&](vector<int>& arr){
        mergeSortSequential(arr, 0, arr.size() - 1);
    });

    testAndPrint("Parallel Merge Sort", [&](vector<int>& arr){
        mergeSortParallel(arr, 0, arr.size() - 1);
    });

    return 0;
}
