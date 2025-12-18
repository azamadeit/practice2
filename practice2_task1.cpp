#include <iostream>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <omp.h>

void fill(int* a, int n) {
    for (int i = 0; i < n; ++i)
        a[i] = std::rand() % 100000;
}

// Последовательный пузырёк
void bubble_seq(int* a, int n) {
    for (int i = 0; i < n - 1; ++i)
        for (int j = 0; j < n - i - 1; ++j)
            if (a[j] > a[j + 1])
                std::swap(a[j], a[j + 1]);
}

// Параллельный пузырёк (odd-even)
void bubble_par(int* a, int n) {
    for (int i = 0; i < n; ++i) {
        #pragma omp parallel for
        for (int j = i % 2; j < n - 1; j += 2)
            if (a[j] > a[j + 1])
                std::swap(a[j], a[j + 1]);
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

    std::cout << "Bubble seq: " << measure(bubble_seq, a, n) << " ms\n";

    fill(a, n);
    std::cout << "Bubble par: " << measure(bubble_par, a, n) << " ms\n";

    delete[] a;
    return 0;
}
