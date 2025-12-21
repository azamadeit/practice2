#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <omp.h>

using namespace std;

// -------------------- SEQ --------------------

void bubbleSort(vector<int>& a) {
    int n = (int)a.size();
    for (int i = 0; i < n - 1; ++i) {
        for (int j = 0; j < n - i - 1; ++j) {
            if (a[j] > a[j + 1]) swap(a[j], a[j + 1]);
        }
    }
}

void selectionSort(vector<int>& a) {
    int n = (int)a.size();
    for (int i = 0; i < n - 1; ++i) {
        int minIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (a[j] < a[minIdx]) minIdx = j;
        }
        swap(a[i], a[minIdx]);
    }
}

void insertionSort(vector<int>& a) {
    int n = (int)a.size();
    for (int i = 1; i < n; ++i) {
        int key = a[i];
        int j = i - 1;
        while (j >= 0 && a[j] > key) {
            a[j + 1] = a[j];
            --j;
        }
        a[j + 1] = key;
    }
}

// -------------------- OMP --------------------

// Odd-even transposition sort (параллельный "пузырёк")
void bubbleSortOMP(vector<int>& a) {
    int n = (int)a.size();
    for (int i = 0; i < n; ++i) {
#pragma omp parallel for
        for (int j = i % 2; j < n - 1; j += 2) {
            if (a[j] > a[j + 1]) swap(a[j], a[j + 1]);
        }
    }
}

// Параллельная сортировка выбором (аккуратная версия без гонок)
void selectionSortOMP(vector<int>& a) {
    int n = (int)a.size();
    for (int i = 0; i < n - 1; ++i) {
        int globalMinIdx = i;
        int globalMinVal = a[i];

#pragma omp parallel
        {
            int localMinIdx = globalMinIdx;
            int localMinVal = globalMinVal;

#pragma omp for nowait
            for (int j = i + 1; j < n; ++j) {
                if (a[j] < localMinVal) {
                    localMinVal = a[j];
                    localMinIdx = j;
                }
            }

#pragma omp critical
            {
                if (localMinVal < globalMinVal) {
                    globalMinVal = localMinVal;
                    globalMinIdx = localMinIdx;
                }
            }
        }

        swap(a[i], a[globalMinIdx]);
    }
}

// Вставки почти не параллелятся напрямую — оставим как “OMP-версию” без распараллеливания
// (по времени будет близко к seq, как в твоём примере)
void insertionSortOMP(vector<int>& a) {
    insertionSort(a);
}

// -------------------- utils --------------------

vector<int> generateArray(int n) {
    vector<int> a(n);
    mt19937 gen(42); // фиксированный seed (повторяемость результатов)
    uniform_int_distribution<int> dis(0, 100000);
    for (int i = 0; i < n; ++i) a[i] = dis(gen);
    return a;
}

void testSort(void (*sortFunc)(vector<int>&), const vector<int>& original, const string& name) {
    vector<int> a = original; // чтобы каждый раз сортировать одинаковые данные
    auto start = chrono::high_resolution_clock::now();
    sortFunc(a);
    auto end = chrono::high_resolution_clock::now();
    auto ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
    cout << name << ": " << ms << " ms\n";
}

int main() {
    vector<int> sizes = {1000, 10000, 100000};

    for (int size : sizes) {
        cout << "\nArray's size: " << size << "\n";

        vector<int> data = generateArray(size);

        testSort(bubbleSort,       data, "Bubble Sort (seq)");
        testSort(bubbleSortOMP,    data, "Bubble Sort (OMP)");

        testSort(selectionSort,    data, "Selection Sort (seq)");
        testSort(selectionSortOMP, data, "Selection Sort (OMP)");

        testSort(insertionSort,    data, "Insertion Sort (seq)");
        testSort(insertionSortOMP, data, "Insertion Sort (OMP)");
    }

    return 0;
}
