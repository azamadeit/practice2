#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

void fill(int* a, int n) {
    for (int i = 0; i < n; ++i)
        a[i] = std::rand() % 100000;
}

// Последовательная сортировка вставками
void insertion_seq(int* a, int n) {
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

// Параллельная сортировка вставками (учебный вариант)
void insertion_par(int* a, int n) {
    #pragma omp parallel for
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

long long measure(void (*sort)(int*, int), int* a, int n) {
    auto start = std::chrono::high_resolution_clock::now();
    sort(a, n);
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

int main() {
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    int n = 10000;

    int* a = new int[n];
    fill(a, n);

    std::cout << "Insertion seq: " << measure(insertion_seq, a, n) << " ms\n";

    fill(a, n);
    std::cout << "Insertion par: " << measure(insertion_par, a, n) << " ms\n";

    delete[] a;
    return 0;
}
