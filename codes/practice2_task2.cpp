#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

void fill(int* a, int n) {
    for (int i = 0; i < n; ++i)
        a[i] = std::rand() % 100000;
}

// Последовательная сортировка выбором
void selection_seq(int* a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        int min_i = i;
        for (int j = i + 1; j < n; ++j)
            if (a[j] < a[min_i])
                min_i = j;
        std::swap(a[i], a[min_i]);
    }
}

// Параллельная сортировка выбором
void selection_par(int* a, int n) {
    for (int i = 0; i < n - 1; ++i) {
        int min_i = i;

        #pragma omp parallel for
        for (int j = i + 1; j < n; ++j) {
            #pragma omp critical
            {
                if (a[j] < a[min_i])
                    min_i = j;
            }
        }
        std::swap(a[i], a[min_i]);
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

    std::cout << "Selection seq: " << measure(selection_seq, a, n) << " ms\n";

    fill(a, n);
    std::cout << "Selection par: " << measure(selection_par, a, n) << " ms\n";

    delete[] a;
    return 0;
}
